################################################################################
#
# Copyright (c) 2013 SirAnthony <anthony at adsorbtion.org>
# Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
# (VIS), Universität Stuttgart.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#
#   * Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
#   * Neither the name of the name of VIS, Universität Stuttgart nor the names
#   of its contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
################################################################################

require genTypes;
require genTools;
our %regexps;
our %files;


if ($^O =~ /Win32/) {
	$WIN32 = 1;
}

sub createHeader
{
	print "#ifdef _WIN32
	#include <windows.h>
	#include <excpt.h>
#else /* _WIN32 */
	#include <signal.h>
	#include <sys/types.h>
	#include <setjmp.h>
	#include \"GL/glx.h\"
#endif /* _WIN32 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include \"GL/gl.h\"
#include \"GL/glext.h\"
#include <GL/glut.h>
#include \"debuglibInternal.h\"


#ifdef _WIN32
	int dirtyFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {
		if (code == EXCEPTION_ACCESS_VIOLATION) {
			return EXCEPTION_EXECUTE_HANDLER;
		} else {
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}
#else /* _WIN32 */
	const char *currentFname = NULL;
	static jmp_buf check_env;
	void catch_segfault(int sig_num) {
		printf(\"# ACCESS VIOLATION WHEN CALLING '%s'\\n\", currentFname);
		fflush(stdout);
		siglongjmp(check_env, sig_num);
	}
//SIGILL,SIGBUS
#endif /* _WIN32 */

void testFunc(void) {
	int dirtyHack[4096];
	memset(dirtyHack, 0, 4096);

";
}

sub createBody
{
	my $line = shift;
	my $extname = shift;
	my $retval = shift;
	my $fname = shift;
	my $argString = shift;

	my $isExtension = $line !~ /WINGDIAPI/;
	my @arguments = buildArgumentList($argString);
	my $pfname = join("","PFN",uc($fname),"PROC");

	my $funcstring = "($pfname)glXGetProcAddressARB((const GLubyte *)\"$fname\")";
	if (defined $WIN32) {
		if ($isExtension) {
			$funcstring = "($pfname)wglGetProcAddress((const GLubyte *)\"$fname\")";
		} else {
			$funcstring = $fname;
		}
	}

	printf "    {
		$pfname func = $funcstring;
		if (func) {
#ifdef _WIN32
			/* This is an evil hack to catch an access violation in Nvidia's
			 * Windows driver. */
			__try {
				glBegin(GL_POINTS);
#else /* _WIN32 */
			if (!sigsetjmp(check_env, 1)) {
				glBegin(GL_POINTS);
					currentFname = \"$fname\";
					signal(SIGSEGV, catch_segfault);
#endif /* _WIN32 */
					func(%s);
#ifndef _WIN32
					currentFname = \"WTF\";
					signal(SIGSEGV, SIG_DFL);
#endif /* !_WIN32 */
				glEnd();
				if (glGetError() != GL_INVALID_OPERATION) {
					printf(\"${fname},\\n\");
				}
#ifdef _WIN32
			} __except(dirtyFilter(GetExceptionCode(), GetExceptionInformation())) {
				printf(\"# ACCESS VIOLATION WHEN CALLING '$fname'\\n\");
			}
#else /* _WIN32 */
			}
#endif /* _WIN32 */
		}
	}
", join(", ", map {getDummyValue($_)} @arguments);
}

my $actions = {
	$regexps{"typegl"} => \&addTypeMapping,
	$regexps{"wingdi"} => \&createBody,
	$regexps{"glapi"} => \&createBody
};


header_generated();
createHeader();

foreach my $filename (@{$files{"gl"}}) {
	parse_output($filename, "GL_VERSION_1_0", "GL_", $actions);
}

print "
		exit(0);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(128, 128);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(testFunc);
	glutMainLoop();
	return 0;
}";

