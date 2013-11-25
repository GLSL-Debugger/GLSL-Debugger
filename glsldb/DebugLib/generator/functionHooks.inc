////////////////////////////////////////////////////////
//
//   THIS FILE IS GENERATED AUTOMATICALLY 2013.11.0022 15:45:20
//
// Copyright (c) 2013 Perl generator
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
////////////////////////////////////////////////////////

void glActiveTextureARB (GLenum arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glActiveTextureARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glActiveTextureARB)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glActiveTextureARB", 1, &arg0, DBG_TYPE_ENUM);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLACTIVETEXTUREARBPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLACTIVETEXTUREARB
#  if DBG_STREAM_HINT_GLACTIVETEXTUREARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glActiveTextureARB", 1);
#  endif
#  if DBG_STREAM_HINT_GLACTIVETEXTUREARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glActiveTextureARB)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glActiveTextureARB)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glClientActiveTextureARB (GLenum arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glClientActiveTextureARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glClientActiveTextureARB)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glClientActiveTextureARB", 1, &arg0, DBG_TYPE_ENUM);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCLIENTACTIVETEXTUREARBPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCLIENTACTIVETEXTUREARB
#  if DBG_STREAM_HINT_GLCLIENTACTIVETEXTUREARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glClientActiveTextureARB", 1);
#  endif
#  if DBG_STREAM_HINT_GLCLIENTACTIVETEXTUREARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glClientActiveTextureARB)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glClientActiveTextureARB)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1dARB (GLenum arg0, GLdouble arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1dARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1dARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1dARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1DARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1DARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1DARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1dARB", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1DARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1dARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1dARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1dvARB (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1dvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1dvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1dvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1DVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1DVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1DVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1dvARB", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1DVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1dvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1dvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1fARB (GLenum arg0, GLfloat arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1fARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1fARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1fARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1FARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1FARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1FARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1fARB", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1FARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1fARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1fARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1fvARB (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1fvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1fvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1fvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1FVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1FVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1FVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1fvARB", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1FVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1fvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1fvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1iARB (GLenum arg0, GLint arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1iARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1iARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1iARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1IARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1IARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1IARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1iARB", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1IARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1iARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1iARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1ivARB (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1ivARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1ivARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1ivARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1IVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1IVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1IVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1ivARB", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1IVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1ivARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1ivARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1sARB (GLenum arg0, GLshort arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1sARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1sARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1sARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1SARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1SARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1SARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1sARB", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1SARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1sARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1sARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1svARB (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1svARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1svARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1svARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1SVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1SVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1SVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1svARB", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1SVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1svARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1svARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2dARB (GLenum arg0, GLdouble arg1, GLdouble arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2dARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2dARB)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2dARB", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE, &arg2, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2DARBPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2DARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2DARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2dARB", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble), &arg2, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2DARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2dARB)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2dARB)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2dvARB (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2dvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2dvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2dvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2DVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2DVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2DVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2dvARB", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2DVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2dvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2dvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2fARB (GLenum arg0, GLfloat arg1, GLfloat arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2fARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2fARB)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2fARB", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2FARBPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2FARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2FARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2fARB", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2FARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2fARB)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2fARB)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2fvARB (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2fvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2fvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2fvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2FVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2FVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2FVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2fvARB", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2FVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2fvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2fvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2iARB (GLenum arg0, GLint arg1, GLint arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2iARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2iARB)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2iARB", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2IARBPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2IARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2IARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2iARB", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2IARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2iARB)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2iARB)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2ivARB (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2ivARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2ivARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2ivARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2IVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2IVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2IVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2ivARB", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2IVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2ivARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2ivARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2sARB (GLenum arg0, GLshort arg1, GLshort arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2sARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2sARB)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2sARB", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT, &arg2, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2SARBPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2SARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2SARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2sARB", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort), &arg2, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2SARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2sARB)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2sARB)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2svARB (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2svARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2svARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2svARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2SVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2SVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2SVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2svARB", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2SVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2svARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2svARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3dARB (GLenum arg0, GLdouble arg1, GLdouble arg2, GLdouble arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3dARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3dARB)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3dARB", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE, &arg2, DBG_TYPE_DOUBLE, &arg3, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3DARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3DARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3DARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3dARB", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble), &arg2, sizeof(GLdouble), &arg3, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3DARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3dARB)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3dARB)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3dvARB (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3dvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3dvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3dvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3DVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3DVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3DVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3dvARB", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3DVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3dvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3dvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3fARB (GLenum arg0, GLfloat arg1, GLfloat arg2, GLfloat arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3fARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3fARB)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3fARB", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT, &arg3, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3FARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3FARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3FARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3fARB", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat), &arg3, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3FARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3fARB)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3fARB)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3fvARB (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3fvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3fvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3fvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3FVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3FVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3FVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3fvARB", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3FVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3fvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3fvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3iARB (GLenum arg0, GLint arg1, GLint arg2, GLint arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3iARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3iARB)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3iARB", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3IARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3IARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3IARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3iARB", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3IARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3iARB)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3iARB)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3ivARB (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3ivARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3ivARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3ivARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3IVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3IVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3IVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3ivARB", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3IVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3ivARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3ivARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3sARB (GLenum arg0, GLshort arg1, GLshort arg2, GLshort arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3sARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3sARB)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3sARB", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT, &arg2, DBG_TYPE_SHORT_INT, &arg3, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3SARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3SARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3SARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3sARB", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort), &arg2, sizeof(GLshort), &arg3, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3SARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3sARB)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3sARB)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3svARB (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3svARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3svARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3svARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3SVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3SVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3SVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3svARB", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3SVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3svARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3svARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4dARB (GLenum arg0, GLdouble arg1, GLdouble arg2, GLdouble arg3, GLdouble arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4dARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4dARB)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4dARB", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE, &arg2, DBG_TYPE_DOUBLE, &arg3, DBG_TYPE_DOUBLE, &arg4, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4DARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4DARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4DARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4dARB", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble), &arg2, sizeof(GLdouble), &arg3, sizeof(GLdouble), &arg4, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4DARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4dARB)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4dARB)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4dvARB (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4dvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4dvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4dvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4DVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4DVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4DVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4dvARB", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4DVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4dvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4dvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4fARB (GLenum arg0, GLfloat arg1, GLfloat arg2, GLfloat arg3, GLfloat arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4fARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4fARB)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4fARB", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT, &arg3, DBG_TYPE_FLOAT, &arg4, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4FARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4FARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4FARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4fARB", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat), &arg3, sizeof(GLfloat), &arg4, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4FARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4fARB)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4fARB)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4fvARB (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4fvARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4fvARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4fvARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4FVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4FVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4FVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4fvARB", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4FVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4fvARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4fvARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4iARB (GLenum arg0, GLint arg1, GLint arg2, GLint arg3, GLint arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4iARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4iARB)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4iARB", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4IARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4IARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4IARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4iARB", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint), &arg4, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4IARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4iARB)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4iARB)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4ivARB (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4ivARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4ivARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4ivARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4IVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4IVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4IVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4ivARB", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4IVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4ivARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4ivARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4sARB (GLenum arg0, GLshort arg1, GLshort arg2, GLshort arg3, GLshort arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4sARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4sARB)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4sARB", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT, &arg2, DBG_TYPE_SHORT_INT, &arg3, DBG_TYPE_SHORT_INT, &arg4, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4SARBPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4SARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4SARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4sARB", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort), &arg2, sizeof(GLshort), &arg3, sizeof(GLshort), &arg4, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4SARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4sARB)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4sARB)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4svARB (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4svARB")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4svARB)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4svARB", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4SVARBPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4SVARB
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4SVARB != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4svARB", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4SVARB == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4svARB)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4svARB)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glFramebufferTextureLayerEXT (GLenum arg0, GLenum arg1, GLuint arg2, GLint arg3, GLint arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glFramebufferTextureLayerEXT")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glFramebufferTextureLayerEXT)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glFramebufferTextureLayerEXT", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_ENUM, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLFRAMEBUFFERTEXTURELAYEREXT
#  if DBG_STREAM_HINT_GLFRAMEBUFFERTEXTURELAYEREXT != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glFramebufferTextureLayerEXT", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLenum), &arg2, sizeof(GLuint), &arg3, sizeof(GLint), &arg4, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLFRAMEBUFFERTEXTURELAYEREXT == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glFramebufferTextureLayerEXT)(arg0, arg1, arg2, arg3, arg4);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glFramebufferTextureLayerEXT)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glBlendEquationSeparateATI (GLenum arg0, GLenum arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glBlendEquationSeparateATI")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glBlendEquationSeparateATI)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glBlendEquationSeparateATI", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_ENUM);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLBLENDEQUATIONSEPARATEATIPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLBLENDEQUATIONSEPARATEATI
#  if DBG_STREAM_HINT_GLBLENDEQUATIONSEPARATEATI != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glBlendEquationSeparateATI", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLenum));
#  endif
#  if DBG_STREAM_HINT_GLBLENDEQUATIONSEPARATEATI == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glBlendEquationSeparateATI)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glBlendEquationSeparateATI)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glEGLImageTargetTexture2DOES (GLenum arg0, GLeglImageOES arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glEGLImageTargetTexture2DOES")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glEGLImageTargetTexture2DOES)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glEGLImageTargetTexture2DOES", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLEGLIMAGETARGETTEXTURE2DOES
#  if DBG_STREAM_HINT_GLEGLIMAGETARGETTEXTURE2DOES != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glEGLImageTargetTexture2DOES", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLeglImageOES));
#  endif
#  if DBG_STREAM_HINT_GLEGLIMAGETARGETTEXTURE2DOES == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glEGLImageTargetTexture2DOES)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glEGLImageTargetTexture2DOES)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glEGLImageTargetRenderbufferStorageOES (GLenum arg0, GLeglImageOES arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glEGLImageTargetRenderbufferStorageOES")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glEGLImageTargetRenderbufferStorageOES)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glEGLImageTargetRenderbufferStorageOES", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLEGLIMAGETARGETRENDERBUFFERSTORAGEOES
#  if DBG_STREAM_HINT_GLEGLIMAGETARGETRENDERBUFFERSTORAGEOES != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glEGLImageTargetRenderbufferStorageOES", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLeglImageOES));
#  endif
#  if DBG_STREAM_HINT_GLEGLIMAGETARGETRENDERBUFFERSTORAGEOES == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glEGLImageTargetRenderbufferStorageOES)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glEGLImageTargetRenderbufferStorageOES)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glBlendColor (GLfloat arg0, GLfloat arg1, GLfloat arg2, GLfloat arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glBlendColor")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glBlendColor)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glBlendColor", 4, &arg0, DBG_TYPE_FLOAT, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT, &arg3, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLBLENDCOLORPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLBLENDCOLOR
#  if DBG_STREAM_HINT_GLBLENDCOLOR != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glBlendColor", 4, &arg0, sizeof(GLfloat), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat), &arg3, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLBLENDCOLOR == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glBlendColor)(arg0, arg1, arg2, arg3);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glBlendColor)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glBlendEquation (GLenum arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glBlendEquation")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glBlendEquation)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glBlendEquation", 1, &arg0, DBG_TYPE_ENUM);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLBLENDEQUATIONPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLBLENDEQUATION
#  if DBG_STREAM_HINT_GLBLENDEQUATION != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glBlendEquation", 1);
#  endif
#  if DBG_STREAM_HINT_GLBLENDEQUATION == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glBlendEquation)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glBlendEquation)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glDrawRangeElements (GLenum arg0, GLuint arg1, GLuint arg2, GLsizei arg3, GLenum arg4, const GLvoid * arg5)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glDrawRangeElements")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glDrawRangeElements)(arg0, arg1, arg2, arg3, arg4, arg5);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glDrawRangeElements", 6, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_ENUM, &arg5, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLDRAWRANGEELEMENTSPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLDRAWRANGEELEMENTS
#  if DBG_STREAM_HINT_GLDRAWRANGEELEMENTS != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glDrawRangeElements", 6, &arg0, sizeof(GLenum), &arg1, sizeof(GLuint), &arg2, sizeof(GLuint), &arg3, sizeof(GLsizei), &arg4, sizeof(GLenum), &arg5, sizeof(void*));
#  endif
#  if DBG_STREAM_HINT_GLDRAWRANGEELEMENTS == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glDrawRangeElements)(arg0, arg1, arg2, arg3, arg4, arg5);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glDrawRangeElements)(arg0, arg1, arg2, arg3, arg4, arg5);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glTexImage3D (GLenum arg0, GLint arg1, GLint arg2, GLsizei arg3, GLsizei arg4, GLsizei arg5, GLint arg6, GLenum arg7, GLenum arg8, const GLvoid * arg9)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glTexImage3D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glTexImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glTexImage3D", 10, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_INT, &arg7, DBG_TYPE_ENUM, &arg8, DBG_TYPE_ENUM, &arg9, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLTEXIMAGE3DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLTEXIMAGE3D
#  if DBG_STREAM_HINT_GLTEXIMAGE3D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glTexImage3D", 10, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLsizei), &arg4, sizeof(GLsizei), &arg5, sizeof(GLsizei), &arg6, sizeof(GLint), &arg7, sizeof(GLenum), &arg8, sizeof(GLenum), arg9, glTexImage3D_getArg9Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
#  endif
#  if DBG_STREAM_HINT_GLTEXIMAGE3D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glTexImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glTexImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glTexSubImage3D (GLenum arg0, GLint arg1, GLint arg2, GLint arg3, GLint arg4, GLsizei arg5, GLsizei arg6, GLsizei arg7, GLenum arg8, GLenum arg9, const GLvoid * arg10)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glTexSubImage3D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glTexSubImage3D", 11, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_INT, &arg7, DBG_TYPE_INT, &arg8, DBG_TYPE_ENUM, &arg9, DBG_TYPE_ENUM, &arg10, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLTEXSUBIMAGE3DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLTEXSUBIMAGE3D
#  if DBG_STREAM_HINT_GLTEXSUBIMAGE3D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glTexSubImage3D", 11, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint), &arg4, sizeof(GLint), &arg5, sizeof(GLsizei), &arg6, sizeof(GLsizei), &arg7, sizeof(GLsizei), &arg8, sizeof(GLenum), &arg9, sizeof(GLenum), arg10, glTexSubImage3D_getArg10Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));
#  endif
#  if DBG_STREAM_HINT_GLTEXSUBIMAGE3D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glCopyTexSubImage3D (GLenum arg0, GLint arg1, GLint arg2, GLint arg3, GLint arg4, GLint arg5, GLint arg6, GLsizei arg7, GLsizei arg8)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glCopyTexSubImage3D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glCopyTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glCopyTexSubImage3D", 9, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_INT, &arg7, DBG_TYPE_INT, &arg8, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCOPYTEXSUBIMAGE3DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCOPYTEXSUBIMAGE3D
#  if DBG_STREAM_HINT_GLCOPYTEXSUBIMAGE3D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glCopyTexSubImage3D", 9, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint), &arg4, sizeof(GLint), &arg5, sizeof(GLint), &arg6, sizeof(GLint), &arg7, sizeof(GLsizei), &arg8, sizeof(GLsizei));
#  endif
#  if DBG_STREAM_HINT_GLCOPYTEXSUBIMAGE3D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glCopyTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glCopyTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glActiveTexture (GLenum arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glActiveTexture")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glActiveTexture)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glActiveTexture", 1, &arg0, DBG_TYPE_ENUM);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLACTIVETEXTUREPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLACTIVETEXTURE
#  if DBG_STREAM_HINT_GLACTIVETEXTURE != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glActiveTexture", 1);
#  endif
#  if DBG_STREAM_HINT_GLACTIVETEXTURE == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glActiveTexture)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glActiveTexture)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSampleCoverage (GLfloat arg0, GLboolean arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSampleCoverage")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSampleCoverage)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSampleCoverage", 2, &arg0, DBG_TYPE_FLOAT, &arg1, DBG_TYPE_BOOLEAN);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSAMPLECOVERAGEPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSAMPLECOVERAGE
#  if DBG_STREAM_HINT_GLSAMPLECOVERAGE != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSampleCoverage", 2, &arg0, sizeof(GLfloat), &arg1, sizeof(GLboolean));
#  endif
#  if DBG_STREAM_HINT_GLSAMPLECOVERAGE == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSampleCoverage)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSampleCoverage)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glCompressedTexImage3D (GLenum arg0, GLint arg1, GLenum arg2, GLsizei arg3, GLsizei arg4, GLsizei arg5, GLint arg6, GLsizei arg7, const GLvoid * arg8)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glCompressedTexImage3D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glCompressedTexImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glCompressedTexImage3D", 9, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_ENUM, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_INT, &arg7, DBG_TYPE_INT, &arg8, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCOMPRESSEDTEXIMAGE3DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE3D
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE3D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glCompressedTexImage3D", 9, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLenum), &arg3, sizeof(GLsizei), &arg4, sizeof(GLsizei), &arg5, sizeof(GLsizei), &arg6, sizeof(GLint), &arg7, sizeof(GLsizei), arg8, glCompressedTexImage3D_getArg8Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
#  endif
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE3D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glCompressedTexImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glCompressedTexImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glCompressedTexImage2D (GLenum arg0, GLint arg1, GLenum arg2, GLsizei arg3, GLsizei arg4, GLint arg5, GLsizei arg6, const GLvoid * arg7)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glCompressedTexImage2D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glCompressedTexImage2D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glCompressedTexImage2D", 8, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_ENUM, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_INT, &arg7, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCOMPRESSEDTEXIMAGE2DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE2D
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE2D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glCompressedTexImage2D", 8, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLenum), &arg3, sizeof(GLsizei), &arg4, sizeof(GLsizei), &arg5, sizeof(GLint), &arg6, sizeof(GLsizei), arg7, glCompressedTexImage2D_getArg7Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7));
#  endif
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE2D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glCompressedTexImage2D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glCompressedTexImage2D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glCompressedTexImage1D (GLenum arg0, GLint arg1, GLenum arg2, GLsizei arg3, GLint arg4, GLsizei arg5, const GLvoid * arg6)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glCompressedTexImage1D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glCompressedTexImage1D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glCompressedTexImage1D", 7, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_ENUM, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCOMPRESSEDTEXIMAGE1DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE1D
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE1D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glCompressedTexImage1D", 7, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLenum), &arg3, sizeof(GLsizei), &arg4, sizeof(GLint), &arg5, sizeof(GLsizei), arg6, glCompressedTexImage1D_getArg6Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6));
#  endif
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXIMAGE1D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glCompressedTexImage1D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glCompressedTexImage1D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glCompressedTexSubImage3D (GLenum arg0, GLint arg1, GLint arg2, GLint arg3, GLint arg4, GLsizei arg5, GLsizei arg6, GLsizei arg7, GLenum arg8, GLsizei arg9, const GLvoid * arg10)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glCompressedTexSubImage3D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glCompressedTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glCompressedTexSubImage3D", 11, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_INT, &arg7, DBG_TYPE_INT, &arg8, DBG_TYPE_ENUM, &arg9, DBG_TYPE_INT, &arg10, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE3D
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE3D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glCompressedTexSubImage3D", 11, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint), &arg4, sizeof(GLint), &arg5, sizeof(GLsizei), &arg6, sizeof(GLsizei), &arg7, sizeof(GLsizei), &arg8, sizeof(GLenum), &arg9, sizeof(GLsizei), arg10, glCompressedTexSubImage3D_getArg10Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));
#  endif
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE3D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glCompressedTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glCompressedTexSubImage3D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glCompressedTexSubImage2D (GLenum arg0, GLint arg1, GLint arg2, GLint arg3, GLsizei arg4, GLsizei arg5, GLenum arg6, GLsizei arg7, const GLvoid * arg8)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glCompressedTexSubImage2D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glCompressedTexSubImage2D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glCompressedTexSubImage2D", 9, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_ENUM, &arg7, DBG_TYPE_INT, &arg8, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE2D
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE2D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glCompressedTexSubImage2D", 9, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint), &arg4, sizeof(GLsizei), &arg5, sizeof(GLsizei), &arg6, sizeof(GLenum), &arg7, sizeof(GLsizei), arg8, glCompressedTexSubImage2D_getArg8Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
#  endif
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE2D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glCompressedTexSubImage2D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glCompressedTexSubImage2D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glCompressedTexSubImage1D (GLenum arg0, GLint arg1, GLint arg2, GLsizei arg3, GLenum arg4, GLsizei arg5, const GLvoid * arg6)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glCompressedTexSubImage1D")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glCompressedTexSubImage1D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glCompressedTexSubImage1D", 7, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_ENUM, &arg5, DBG_TYPE_INT, &arg6, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE1D
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE1D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glCompressedTexSubImage1D", 7, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLsizei), &arg4, sizeof(GLenum), &arg5, sizeof(GLsizei), arg6, glCompressedTexSubImage1D_getArg6Size(arg0, arg1, arg2, arg3, arg4, arg5, arg6));
#  endif
#  if DBG_STREAM_HINT_GLCOMPRESSEDTEXSUBIMAGE1D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glCompressedTexSubImage1D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glCompressedTexSubImage1D)(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glGetCompressedTexImage (GLenum arg0, GLint arg1, GLvoid * arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glGetCompressedTexImage")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glGetCompressedTexImage)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glGetCompressedTexImage", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLGETCOMPRESSEDTEXIMAGEPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLGETCOMPRESSEDTEXIMAGE
#  if DBG_STREAM_HINT_GLGETCOMPRESSEDTEXIMAGE != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glGetCompressedTexImage", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), arg2, glGetCompressedTexImage_getArg2Size(arg0, arg1, arg2));
#  endif
#  if DBG_STREAM_HINT_GLGETCOMPRESSEDTEXIMAGE == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glGetCompressedTexImage)(arg0, arg1, arg2);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glGetCompressedTexImage)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glClientActiveTexture (GLenum arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glClientActiveTexture")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glClientActiveTexture)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glClientActiveTexture", 1, &arg0, DBG_TYPE_ENUM);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLCLIENTACTIVETEXTUREPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLCLIENTACTIVETEXTURE
#  if DBG_STREAM_HINT_GLCLIENTACTIVETEXTURE != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glClientActiveTexture", 1);
#  endif
#  if DBG_STREAM_HINT_GLCLIENTACTIVETEXTURE == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glClientActiveTexture)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glClientActiveTexture)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1d (GLenum arg0, GLdouble arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1d")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1d)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1d", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1DPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1D
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1d", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1d)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1d)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1dv (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1dv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1dv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1dv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1DVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1DV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1DV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1dv", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1DV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1dv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1dv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1f (GLenum arg0, GLfloat arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1f")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1f)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1f", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1FPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1F
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1F != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1f", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1F == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1f)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1f)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1fv (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1fv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1fv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1fv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1FVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1FV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1FV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1fv", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1FV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1fv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1fv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1i (GLenum arg0, GLint arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1i")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1i)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1i", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1IPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1I
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1I != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1i", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1I == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1i)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1i)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1iv (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1iv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1iv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1iv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1IVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1IV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1IV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1iv", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1IV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1iv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1iv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1s (GLenum arg0, GLshort arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1s")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1s)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1s", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1SPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1S
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1S != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1s", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1S == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1s)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1s)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord1sv (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord1sv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord1sv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord1sv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD1SVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD1SV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1SV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord1sv", 2, &arg0, sizeof(GLenum), arg1, 1*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD1SV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord1sv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord1sv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2d (GLenum arg0, GLdouble arg1, GLdouble arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2d")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2d)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2d", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE, &arg2, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2DPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2D
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2d", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble), &arg2, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2d)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2d)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2dv (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2dv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2dv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2dv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2DVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2DV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2DV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2dv", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2DV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2dv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2dv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2f (GLenum arg0, GLfloat arg1, GLfloat arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2f")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2f)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2f", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2FPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2F
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2F != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2f", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2F == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2f)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2f)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2fv (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2fv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2fv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2fv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2FVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2FV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2FV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2fv", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2FV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2fv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2fv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2i (GLenum arg0, GLint arg1, GLint arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2i")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2i)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2i", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2IPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2I
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2I != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2i", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2I == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2i)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2i)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2iv (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2iv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2iv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2iv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2IVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2IV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2IV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2iv", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2IV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2iv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2iv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2s (GLenum arg0, GLshort arg1, GLshort arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2s")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2s)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2s", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT, &arg2, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2SPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2S
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2S != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2s", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort), &arg2, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2S == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2s)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2s)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord2sv (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord2sv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord2sv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord2sv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD2SVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD2SV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2SV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord2sv", 2, &arg0, sizeof(GLenum), arg1, 2*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD2SV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord2sv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord2sv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3d (GLenum arg0, GLdouble arg1, GLdouble arg2, GLdouble arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3d")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3d)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3d", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE, &arg2, DBG_TYPE_DOUBLE, &arg3, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3DPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3D
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3d", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble), &arg2, sizeof(GLdouble), &arg3, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3d)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3d)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3dv (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3dv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3dv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3dv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3DVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3DV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3DV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3dv", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3DV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3dv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3dv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3f (GLenum arg0, GLfloat arg1, GLfloat arg2, GLfloat arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3f")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3f)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3f", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT, &arg3, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3FPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3F
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3F != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3f", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat), &arg3, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3F == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3f)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3f)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3fv (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3fv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3fv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3fv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3FVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3FV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3FV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3fv", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3FV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3fv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3fv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3i (GLenum arg0, GLint arg1, GLint arg2, GLint arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3i")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3i)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3i", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3IPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3I
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3I != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3i", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3I == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3i)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3i)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3iv (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3iv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3iv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3iv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3IVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3IV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3IV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3iv", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3IV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3iv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3iv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3s (GLenum arg0, GLshort arg1, GLshort arg2, GLshort arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3s")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3s)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3s", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT, &arg2, DBG_TYPE_SHORT_INT, &arg3, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3SPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3S
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3S != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3s", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort), &arg2, sizeof(GLshort), &arg3, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3S == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3s)(arg0, arg1, arg2, arg3);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3s)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord3sv (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord3sv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord3sv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord3sv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD3SVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD3SV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3SV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord3sv", 2, &arg0, sizeof(GLenum), arg1, 3*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD3SV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord3sv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord3sv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4d (GLenum arg0, GLdouble arg1, GLdouble arg2, GLdouble arg3, GLdouble arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4d")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4d)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4d", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_DOUBLE, &arg2, DBG_TYPE_DOUBLE, &arg3, DBG_TYPE_DOUBLE, &arg4, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4DPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4D
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4d", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLdouble), &arg2, sizeof(GLdouble), &arg3, sizeof(GLdouble), &arg4, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4d)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4d)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4dv (GLenum arg0, const GLdouble * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4dv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4dv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4dv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4DVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4DV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4DV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4dv", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLdouble *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4DV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4dv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4dv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4f (GLenum arg0, GLfloat arg1, GLfloat arg2, GLfloat arg3, GLfloat arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4f")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4f)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4f", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT, &arg3, DBG_TYPE_FLOAT, &arg4, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4FPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4F
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4F != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4f", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat), &arg3, sizeof(GLfloat), &arg4, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4F == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4f)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4f)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4fv (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4fv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4fv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4fv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4FVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4FV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4FV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4fv", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLfloat *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4FV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4fv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4fv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4i (GLenum arg0, GLint arg1, GLint arg2, GLint arg3, GLint arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4i")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4i)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4i", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT, &arg3, DBG_TYPE_INT, &arg4, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4IPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4I
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4I != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4i", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLint), &arg2, sizeof(GLint), &arg3, sizeof(GLint), &arg4, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4I == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4i)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4i)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4iv (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4iv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4iv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4iv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4IVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4IV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4IV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4iv", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLint *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4IV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4iv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4iv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4s (GLenum arg0, GLshort arg1, GLshort arg2, GLshort arg3, GLshort arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4s")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4s)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4s", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_SHORT_INT, &arg2, DBG_TYPE_SHORT_INT, &arg3, DBG_TYPE_SHORT_INT, &arg4, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4SPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4S
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4S != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4s", 5, &arg0, sizeof(GLenum), &arg1, sizeof(GLshort), &arg2, sizeof(GLshort), &arg3, sizeof(GLshort), &arg4, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4S == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4s)(arg0, arg1, arg2, arg3, arg4);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4s)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiTexCoord4sv (GLenum arg0, const GLshort * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiTexCoord4sv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiTexCoord4sv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiTexCoord4sv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTITEXCOORD4SVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTITEXCOORD4SV
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4SV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiTexCoord4sv", 2, &arg0, sizeof(GLenum), arg1, 4*sizeof(GLshort *));
#  endif
#  if DBG_STREAM_HINT_GLMULTITEXCOORD4SV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiTexCoord4sv)(arg0, arg1);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiTexCoord4sv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glLoadTransposeMatrixf (const GLfloat * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glLoadTransposeMatrixf")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glLoadTransposeMatrixf)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glLoadTransposeMatrixf", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLLOADTRANSPOSEMATRIXFPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLLOADTRANSPOSEMATRIXF
#  if DBG_STREAM_HINT_GLLOADTRANSPOSEMATRIXF != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glLoadTransposeMatrixf", 1);
#  endif
#  if DBG_STREAM_HINT_GLLOADTRANSPOSEMATRIXF == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glLoadTransposeMatrixf)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glLoadTransposeMatrixf)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glLoadTransposeMatrixd (const GLdouble * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glLoadTransposeMatrixd")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glLoadTransposeMatrixd)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glLoadTransposeMatrixd", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLLOADTRANSPOSEMATRIXDPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLLOADTRANSPOSEMATRIXD
#  if DBG_STREAM_HINT_GLLOADTRANSPOSEMATRIXD != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glLoadTransposeMatrixd", 1);
#  endif
#  if DBG_STREAM_HINT_GLLOADTRANSPOSEMATRIXD == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glLoadTransposeMatrixd)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glLoadTransposeMatrixd)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultTransposeMatrixf (const GLfloat * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultTransposeMatrixf")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultTransposeMatrixf)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultTransposeMatrixf", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTTRANSPOSEMATRIXFPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTTRANSPOSEMATRIXF
#  if DBG_STREAM_HINT_GLMULTTRANSPOSEMATRIXF != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultTransposeMatrixf", 1);
#  endif
#  if DBG_STREAM_HINT_GLMULTTRANSPOSEMATRIXF == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultTransposeMatrixf)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultTransposeMatrixf)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultTransposeMatrixd (const GLdouble * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultTransposeMatrixd")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultTransposeMatrixd)(arg0);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultTransposeMatrixd", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTTRANSPOSEMATRIXDPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTTRANSPOSEMATRIXD
#  if DBG_STREAM_HINT_GLMULTTRANSPOSEMATRIXD != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultTransposeMatrixd", 1);
#  endif
#  if DBG_STREAM_HINT_GLMULTTRANSPOSEMATRIXD == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultTransposeMatrixd)(arg0);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultTransposeMatrixd)(arg0);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glBlendFuncSeparate (GLenum arg0, GLenum arg1, GLenum arg2, GLenum arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glBlendFuncSeparate")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glBlendFuncSeparate)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glBlendFuncSeparate", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_ENUM, &arg2, DBG_TYPE_ENUM, &arg3, DBG_TYPE_ENUM);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLBLENDFUNCSEPARATEPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLBLENDFUNCSEPARATE
#  if DBG_STREAM_HINT_GLBLENDFUNCSEPARATE != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glBlendFuncSeparate", 4, &arg0, sizeof(GLenum), &arg1, sizeof(GLenum), &arg2, sizeof(GLenum), &arg3, sizeof(GLenum));
#  endif
#  if DBG_STREAM_HINT_GLBLENDFUNCSEPARATE == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glBlendFuncSeparate)(arg0, arg1, arg2, arg3);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glBlendFuncSeparate)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiDrawArrays (GLenum arg0, const GLint * arg1, const GLsizei * arg2, GLsizei arg3)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiDrawArrays")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiDrawArrays)(arg0, arg1, arg2, arg3);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiDrawArrays", 4, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER, &arg2, DBG_TYPE_POINTER, &arg3, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTIDRAWARRAYSPROC)getDbgFunction())(arg0, arg1, arg2, arg3);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTIDRAWARRAYS
#  if DBG_STREAM_HINT_GLMULTIDRAWARRAYS != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiDrawArrays", 4, &arg0, sizeof(GLenum), &arg1, sizeof(void*), &arg2, sizeof(void*), &arg3, sizeof(GLsizei));
#  endif
#  if DBG_STREAM_HINT_GLMULTIDRAWARRAYS == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiDrawArrays)(arg0, arg1, arg2, arg3);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiDrawArrays)(arg0, arg1, arg2, arg3);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glMultiDrawElements (GLenum arg0, const GLsizei * arg1, GLenum arg2, const GLvoid * const * arg3, GLsizei arg4)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glMultiDrawElements")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glMultiDrawElements)(arg0, arg1, arg2, arg3, arg4);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glMultiDrawElements", 5, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER, &arg2, DBG_TYPE_ENUM, &arg3, DBG_TYPE_POINTER, &arg4, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLMULTIDRAWELEMENTSPROC)getDbgFunction())(arg0, arg1, arg2, arg3, arg4);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLMULTIDRAWELEMENTS
#  if DBG_STREAM_HINT_GLMULTIDRAWELEMENTS != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glMultiDrawElements", 5, &arg0, sizeof(GLenum), &arg1, sizeof(void*), &arg2, sizeof(GLenum), &arg3, sizeof(void*), &arg4, sizeof(GLsizei));
#  endif
#  if DBG_STREAM_HINT_GLMULTIDRAWELEMENTS == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glMultiDrawElements)(arg0, arg1, arg2, arg3, arg4);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glMultiDrawElements)(arg0, arg1, arg2, arg3, arg4);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glPointParameterf (GLenum arg0, GLfloat arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glPointParameterf")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glPointParameterf)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glPointParameterf", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLPOINTPARAMETERFPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLPOINTPARAMETERF
#  if DBG_STREAM_HINT_GLPOINTPARAMETERF != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glPointParameterf", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLPOINTPARAMETERF == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glPointParameterf)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glPointParameterf)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glPointParameterfv (GLenum arg0, const GLfloat * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glPointParameterfv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glPointParameterfv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glPointParameterfv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLPOINTPARAMETERFVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLPOINTPARAMETERFV
#  if DBG_STREAM_HINT_GLPOINTPARAMETERFV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glPointParameterfv", 2, &arg0, sizeof(GLenum), arg1, glPointParameterfv_getArg1Size(arg0, arg1));
#  endif
#  if DBG_STREAM_HINT_GLPOINTPARAMETERFV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glPointParameterfv)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glPointParameterfv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glPointParameteri (GLenum arg0, GLint arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glPointParameteri")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glPointParameteri)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glPointParameteri", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLPOINTPARAMETERIPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLPOINTPARAMETERI
#  if DBG_STREAM_HINT_GLPOINTPARAMETERI != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glPointParameteri", 2, &arg0, sizeof(GLenum), &arg1, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLPOINTPARAMETERI == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glPointParameteri)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glPointParameteri)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glPointParameteriv (GLenum arg0, const GLint * arg1)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glPointParameteriv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glPointParameteriv)(arg0, arg1);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glPointParameteriv", 2, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLPOINTPARAMETERIVPROC)getDbgFunction())(arg0, arg1);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLPOINTPARAMETERIV
#  if DBG_STREAM_HINT_GLPOINTPARAMETERIV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glPointParameteriv", 2, &arg0, sizeof(GLenum), arg1, glPointParameteriv_getArg1Size(arg0, arg1));
#  endif
#  if DBG_STREAM_HINT_GLPOINTPARAMETERIV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glPointParameteriv)(arg0, arg1);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glPointParameteriv)(arg0, arg1);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glFogCoordf (GLfloat arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glFogCoordf")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glFogCoordf)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glFogCoordf", 1, &arg0, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLFOGCOORDFPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLFOGCOORDF
#  if DBG_STREAM_HINT_GLFOGCOORDF != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glFogCoordf", 1);
#  endif
#  if DBG_STREAM_HINT_GLFOGCOORDF == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glFogCoordf)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glFogCoordf)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glFogCoordfv (const GLfloat * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glFogCoordfv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glFogCoordfv)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glFogCoordfv", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLFOGCOORDFVPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLFOGCOORDFV
#  if DBG_STREAM_HINT_GLFOGCOORDFV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glFogCoordfv", 1);
#  endif
#  if DBG_STREAM_HINT_GLFOGCOORDFV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glFogCoordfv)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glFogCoordfv)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glFogCoordd (GLdouble arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glFogCoordd")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glFogCoordd)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glFogCoordd", 1, &arg0, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLFOGCOORDDPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLFOGCOORDD
#  if DBG_STREAM_HINT_GLFOGCOORDD != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glFogCoordd", 1);
#  endif
#  if DBG_STREAM_HINT_GLFOGCOORDD == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glFogCoordd)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glFogCoordd)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glFogCoorddv (const GLdouble * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glFogCoorddv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glFogCoorddv)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glFogCoorddv", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLFOGCOORDDVPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLFOGCOORDDV
#  if DBG_STREAM_HINT_GLFOGCOORDDV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glFogCoorddv", 1);
#  endif
#  if DBG_STREAM_HINT_GLFOGCOORDDV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glFogCoorddv)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glFogCoorddv)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glFogCoordPointer (GLenum arg0, GLsizei arg1, const GLvoid * arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glFogCoordPointer")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glFogCoordPointer)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            error = ORIG_GL(glGetError)();
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glFogCoordPointer", 3, &arg0, DBG_TYPE_ENUM, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLFOGCOORDPOINTERPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLFOGCOORDPOINTER
#  if DBG_STREAM_HINT_GLFOGCOORDPOINTER != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glFogCoordPointer", 3, &arg0, sizeof(GLenum), &arg1, sizeof(GLsizei), arg2, glFogCoordPointer_getArg2Size(arg0, arg1, arg2));
#  endif
#  if DBG_STREAM_HINT_GLFOGCOORDPOINTER == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glFogCoordPointer)(arg0, arg1, arg2);
                error = ORIG_GL(glGetError)();
setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glFogCoordPointer)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    error = ORIG_GL(glGetError)();
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3b (GLbyte arg0, GLbyte arg1, GLbyte arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3b")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3b)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3b", 3, &arg0, DBG_TYPE_CHAR, &arg1, DBG_TYPE_CHAR, &arg2, DBG_TYPE_CHAR);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3BPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3B
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3B != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3b", 3, &arg0, sizeof(GLbyte), &arg1, sizeof(GLbyte), &arg2, sizeof(GLbyte));
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3B == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3b)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3b)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3bv (const GLbyte * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3bv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3bv)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3bv", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3BVPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3BV
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3BV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3bv", 1);
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3BV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3bv)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3bv)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3d (GLdouble arg0, GLdouble arg1, GLdouble arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3d")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3d)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3d", 3, &arg0, DBG_TYPE_DOUBLE, &arg1, DBG_TYPE_DOUBLE, &arg2, DBG_TYPE_DOUBLE);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3DPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3D
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3D != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3d", 3, &arg0, sizeof(GLdouble), &arg1, sizeof(GLdouble), &arg2, sizeof(GLdouble));
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3D == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3d)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3d)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3dv (const GLdouble * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3dv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3dv)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3dv", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3DVPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3DV
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3DV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3dv", 1);
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3DV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3dv)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3dv)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3f (GLfloat arg0, GLfloat arg1, GLfloat arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3f")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3f)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3f", 3, &arg0, DBG_TYPE_FLOAT, &arg1, DBG_TYPE_FLOAT, &arg2, DBG_TYPE_FLOAT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3FPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3F
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3F != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3f", 3, &arg0, sizeof(GLfloat), &arg1, sizeof(GLfloat), &arg2, sizeof(GLfloat));
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3F == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3f)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3f)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3fv (const GLfloat * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3fv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3fv)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3fv", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3FVPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3FV
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3FV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3fv", 1);
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3FV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3fv)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3fv)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3i (GLint arg0, GLint arg1, GLint arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3i")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3i)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3i", 3, &arg0, DBG_TYPE_INT, &arg1, DBG_TYPE_INT, &arg2, DBG_TYPE_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3IPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3I
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3I != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3i", 3, &arg0, sizeof(GLint), &arg1, sizeof(GLint), &arg2, sizeof(GLint));
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3I == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3i)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3i)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3iv (const GLint * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3iv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3iv)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3iv", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3IVPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3IV
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3IV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3iv", 1);
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3IV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3iv)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3iv)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3s (GLshort arg0, GLshort arg1, GLshort arg2)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3s")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3s)(arg0, arg1, arg2);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3s", 3, &arg0, DBG_TYPE_SHORT_INT, &arg1, DBG_TYPE_SHORT_INT, &arg2, DBG_TYPE_SHORT_INT);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3SPROC)getDbgFunction())(arg0, arg1, arg2);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3S
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3S != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3s", 3, &arg0, sizeof(GLshort), &arg1, sizeof(GLshort), &arg2, sizeof(GLshort));
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3S == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3s)(arg0, arg1, arg2);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3s)(arg0, arg1, arg2);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
void glSecondaryColor3sv (const GLshort * arg0)
{
    
    int op, error;
    pthread_mutex_lock(&G.lock);
    if (keepExecuting("glSecondaryColor3sv")) {
        pthread_mutex_unlock(&G.lock);
         ORIG_GL(glSecondaryColor3sv)(arg0);
        if (checkGLErrorInExecution()) {
            if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                return ;
            }
        } else {
            return ;
        }
    }
    //fprintf(stderr, "ThreadID: %li\n", (unsigned long)pthread_self());
    storeFunctionCall("glSecondaryColor3sv", 1, &arg0, DBG_TYPE_POINTER);
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                 ((PFNGLSECONDARYCOLOR3SVPROC)getDbgFunction())(arg0);
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_GLSECONDARYCOLOR3SV
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3SV != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, "glSecondaryColor3sv", 1);
#  endif
#  if DBG_STREAM_HINT_GLSECONDARYCOLOR3SV == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                 ORIG_GL(glSecondaryColor3sv)(arg0);
                if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}setErrorCode(error);
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                pthread_mutex_unlock(&G.lock);
                 ORIG_GL(glSecondaryColor3sv)(arg0);
                if (checkGLErrorInExecution()) {
                    if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        return ;
                    }
                } else {
                    return ;
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        pthread_mutex_unlock(&G.lock);
        return ;
    }
}
