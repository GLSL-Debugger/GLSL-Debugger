/******************************************************************************

Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
(VIS), Universität Stuttgart.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or
    other materials provided with the distribution.

  * Neither the name of the name of VIS, Universität Stuttgart nor the names
    of its contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#define GLX_GLXEXT_PROTOTYPES
#define GLH_EXT_SINGLE_FILE
#include <GL/glew.h>

/* get rid of some symbols defined in X.h included from glx.h that conflict e.g. with QT */
#undef None
#undef Unsorted
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef CursorShape
#undef Bool
#undef Status
#undef FontChange

#include "glScatter.qt.h"
#include <GL/glu.h>
#include <QtCore/QFile>
#include <QtGui/QMouseEvent>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define FOVY      60.0f
#define NEAR_CLIP 0.001f
#define FAR_CLIP  10.0f

#define MOUSE_SCALE (1.0f/128.0f)

GLScatter::GLScatter(QWidget *parent) :
		QGLWidget(parent)
{
	m_Rotation.x = m_Rotation.y = m_Rotation.z = 0.0;
	m_Rotation.w = 1.0;
	m_Translation.x = m_Translation.y = m_Translation.z = 0.0f;
	m_CamDist = 5.0f;
	m_qClearColor = Qt::white;

	m_iNumPoints = 0;
	m_pVertices = NULL;
	m_pColors = NULL;
	m_psize = 0.01;
}

GLScatter::~GLScatter()
{

}

void GLScatter::resetView(void)
{
	m_Rotation.x = m_Rotation.y = m_Rotation.z = 0.0;
	m_Rotation.w = 1.0;
	m_Translation.x = m_Translation.y = m_Translation.z = 0.0f;
	m_CamDist = 5.0f;
	updateGL();
}

void GLScatter::setClearColor(QColor i_qColor)
{
	m_qClearColor = i_qColor;
	qglClearColor(m_qClearColor);
	updateGL();
}

void GLScatter::setData(float *positions, float *colors, int numPoints)
{
	m_iNumPoints = numPoints;
	m_pVertices = positions;
	m_pColors = colors;
}

static void printShaderInfoLog(GLuint shader)
{
	int length;
	GLchar *log;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) {
		if (!(log = (GLchar*) malloc(length * sizeof(GLchar)))) {
			fprintf(stderr, "Allocation of mem for GLSL info log failed\n");
			exit(1);
		}
		glGetShaderInfoLog(shader, length, NULL, log);
		fprintf(stderr, "SHADER INFOLOG:\n%s\n", log);
		free(log);
	}
}

static int initGLSLShader(const char *src, GLuint *shader, GLenum type)
{
	GLint pstatus;

	if (!src | !shader) {
		return 0;
	}

	if (!(*shader = glCreateShader(type))) {
		fprintf(stderr, "Failed to create GLSL shader object\n");
		return 0;
	}

	glShaderSource(*shader, 1, (const GLchar**) &src, NULL);
	glCompileShader(*shader);
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &pstatus);
	if (!pstatus) {
		fprintf(stderr, "Compilation of glsl shader failed!\n");
		printShaderInfoLog(*shader);
		return 0;
	}

	return 1;
}

static void printProgramInfoLog(GLuint shader)
{
	int length;
	GLchar *log;

	glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
	if (length > 1) {
		if (!(log = (GLchar*) malloc(length * sizeof(GLchar)))) {
			fprintf(stderr, "Allocation of mem for GLSL info log failed\n");
			exit(1);
		}
		glGetProgramInfoLog(shader, length, NULL, log);
		fprintf(stderr, "PROGRAM INFOLOG:\n%s\n", log);
		free(log);
	}
}

void GLScatter::initializeGL()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "E: Unable to initialize glew\n");
		exit(1);
	}

	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "E: GL_VERSION 2.0 required\n");
		exit(1);
	}

	qglClearColor(m_qClearColor);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	GLuint vertShader;
	GLuint fragShader;
	QFile vertProg(":/shaders/shaders/pointbased_spheres.vp");
	QFile fragProg(":/shaders/shaders/pointbased_spheres.fp");
	if (!vertProg.open(QIODevice::ReadOnly | QIODevice::Text)) {
		fprintf(stderr, "E: Failed to open vertex shader ressource\n");
		exit(1);
	}
	if (!fragProg.open(QIODevice::ReadOnly | QIODevice::Text)) {
		fprintf(stderr, "E: Failed to open fragment shader ressource\n");
		exit(1);
	}
	QByteArray vertProgSrc = vertProg.readAll();
	QByteArray fragProgSrc = fragProg.readAll();
	vertProg.close();
	fragProg.close();

	initGLSLShader(vertProgSrc.data(), &vertShader, GL_VERTEX_SHADER);
	initGLSLShader(fragProgSrc.data(), &fragShader, GL_FRAGMENT_SHADER);

	if (!(m_Shader = glCreateProgram())) {
		fprintf(stderr, "E: creating glsl shader program failed\n");
		exit(1);
	}
	glAttachShader(m_Shader, vertShader);
	glAttachShader(m_Shader, fragShader);
	glLinkProgram(m_Shader);
	GLint status;
	glGetProgramiv(m_Shader, GL_LINK_STATUS, &status);
	if (!status) {
		fprintf(stderr, "Linking glsl failed!\n");
		printProgramInfoLog(m_Shader);
		exit(1);
	}
}

void GLScatter::paintGL()
{
	Vector3 rotAxis;
	float rotAngle;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	float cDist = 1.0f / tan(FOVY / 360.0f * M_PI);
	glTranslatef(0.0f, 0.0f, -cDist);

	// Camera
	glTranslatef(0.0, 0.0, -m_CamDist);
	// Translation
	glTranslatef(m_Translation.x, m_Translation.y, m_Translation.z);
	// Roatation
	Quaternion_getAngleAxis(m_Rotation, &rotAngle, &rotAxis);
	glRotatef(rotAngle * 180.0 / M_PI, rotAxis.x, rotAxis.y, rotAxis.z);
	glTranslatef(-0.5f, -0.5f, -0.5f);

	// Dummy axis
	glBegin(GL_LINES);
	// X - red
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	// Y - green
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	// Z - blue
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);

	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);

	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glEnd();

	glUseProgram(m_Shader);
	glUniform4f(glGetUniformLocation(m_Shader, "vpParams"), 2.0 / width(),
			2.0 / height(), 0.5 * width(), 0.5 * height());
	glUniform1f(glGetUniformLocation(m_Shader, "psize"), m_psize);

#ifdef DEBUG
	fprintf(stderr, "m_iNumPoints = %i m_pVertices=%p m_pColors=%p m_psize=%f\n",
			m_iNumPoints, m_pVertices, m_pColors, m_psize);
#endif

	if (m_iNumPoints && m_pVertices && m_pColors) {
		glEnable(GL_VERTEX_ARRAY);
		glEnable(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, m_pVertices);
		glColorPointer(3, GL_FLOAT, 0, m_pColors);
		glDrawArrays(GL_POINTS, 0, m_iNumPoints);
		glDisable(GL_VERTEX_ARRAY);
		glDisable(GL_COLOR_ARRAY);
	}
	glUseProgram(0);
}

void GLScatter::resizeGL(int width, int height)
{
	float aspect = width / (float) height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOVY, aspect, NEAR_CLIP, FAR_CLIP);

	glMatrixMode(GL_MODELVIEW);
}

void GLScatter::mousePressEvent(QMouseEvent *event)
{
	m_qLastMousePos = event->pos();
}

void GLScatter::mouseMoveEvent(QMouseEvent *event)
{
	Vector3 mouseVector, rotAxis, view;

	mouseVector.x = MOUSE_SCALE * (event->x() - m_qLastMousePos.x());
	mouseVector.y = -MOUSE_SCALE * (event->y() - m_qLastMousePos.y());
	mouseVector.z = 0.0;

	view.x = 0.0;
	view.y = 0.0;
	view.z = -1.0;

	rotAxis = Vector3_cross(mouseVector, view);
	Vector3_normalize(&rotAxis);

	if (event->buttons() & Qt::LeftButton) {
		Quaternion rot = Quaternion_fromAngleAxis(
				SQR(mouseVector.x) + SQR(mouseVector.y), rotAxis);
		m_Rotation = Quaternion_mult(rot, m_Rotation);
		Quaternion_normalize(&m_Rotation);
	} else if (event->buttons() & Qt::RightButton) {
		m_CamDist += 0.5f * (mouseVector.x + mouseVector.y);
	} else if (event->buttons() & Qt::MidButton) {
		m_Translation = Vector3_add(m_Translation, mouseVector);
	}
	m_qLastMousePos = event->pos();

	updateGL();
}

