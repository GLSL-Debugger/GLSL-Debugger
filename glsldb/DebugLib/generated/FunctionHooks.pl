################################################################################
#
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

require "functionsAllowedInBeginEnd.pl";
require "justCopyPointersList.pl";

require prePostExecuteList;
require genTools;
require genTypes;
our %regexps;
our %typeMap;


if ($^O =~ /Win32/) {
    $WIN32 = 1;
}





# TODO: check position of unlock statements!!!
@defined_funcs = ();

sub createBody
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my $checkError = shift;
    $fname =~ s/^\s+|\s+$//g;
    $retval =~ s/^\s+|\s+$//g;
    # No mesa functions
    if ($fname =~ /MESA$/ || grep(/^$fname$/i, @defined_types)) {
        return;
    }
    push(@defined_types, $fname);

    my @arguments = buildArgumentList($argString);
    my $pfname = join("","PFN",uc($fname),"PROC");

    my $unlockStatement;
    if (defined $WIN32) {
        $unlockStatement = "LeaveCriticalSection(&G.lock);\n";
    } else {
        $unlockStatement = "pthread_mutex_unlock(&G.lock);\n";
    }

    ###########################################################################
    # create function head
    ###########################################################################
    if (defined $WIN32) {
        print "__declspec(dllexport) $retval APIENTRY Detoured$fname (";
    } else {
        print "$retval $fname (";
    }
    # add arguments to function head
    my $i = 0;
    if (@arguments[0] ~= /^void$|^$/i){
        print "void"
    } else {
        my $i = 0;
        print join(", ", map {
                    (/(.*)(\[\d+\])/ ? "$1 arg$i$2" : "$_ arg$i"), ++$i
                } @arguments);
    }

    print ")\n{\n";
    # temp. variable that holds the return value of the function
    if ($retval !~ /^void$|^$/i) {
        print "\t\t$retval result;\n";
    }

    ###########################################################################
    # first store name of called function and its arguments in the shared memory
    # segment, then call dbgFunctionCall that stops the process/thread and waits
    # for the debugger to handle the actual debugging
    print "\t\tint op, error;\n";
    if (defined $WIN32) {
        print "     DbgRec *rec;
        dbgPrint(DBGLVL_DEBUG, \"entering $fname\\n\");
        rec = getThreadRecord(GetCurrentProcessId());
";
        if ($fname eq "glEnd") {
            print "\t\tG.errorCheckAllowed = 1;\n";
        }elsif ($fname eq "glBegin") {
            print "\t\tG.errorCheckAllowed = 0;\n";
        }
        print "        if(rec->isRecursing) {
        dbgPrint(DBGLVL_DEBUG, \"stopping recursion\\n\");
";
        print pre_execute("\t\t\t", $fname, @arguments);
        if ($retval !~ /^void$|^$/i) {
            print "\t\t\tresult = ";
        } else {
            print "\t\t\t";
        }
        printf "ORIG_GL($fname)(%s);
            /* no way to check errors in recursive calls! */
            error = GL_NO_ERROR;
", arguments_string(@arguments);
        print post_execute("\t\t\t", $fname, $retval, @arguments);
        if ($retval !~ /^void$|^$/i) {
            print "\t\t\treturn result;\n";
        } else {
            print "\t\t\treturn;\n";
        }
        print "        }
        rec->isRecursing = 1;
        EnterCriticalSection(&G.lock);
";
    } else {
        print "        pthread_mutex_lock(&G.lock);\n";
    }
    print "        if (keepExecuting(\"$fname\")) {
            $unlockStatement";
    print pre_execute("\t\t\t", $fname, @arguments);
    if ($retval !~ /^void$|^$/i) {
        print "\t\t\tresult = ";
    } else {
        print "\t\t\t";
    }
    printf "ORIG_GL($fname)(%s);\n", arguments_string(@arguments);
    print "\t\t\tif (checkGLErrorInExecution()) {\n";

    if ($retval !~ /^void$|^$/i) {
        if ($checkError) {
            if (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
                print "\t\t\t\terror = ORIG_GL(glGetError)();\n";

            } else {
                print "\t\t\t\tif (G.errorCheckAllowed) {\n";
                print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
                print "\t\t\t\t} else {\n";
                print "\t\t\t\t\terror = GL_NO_ERROR;\n";
                print "\t\t\t\t}\n";
            }
        } else {
            print "\t\t\t\terror = GL_NO_ERROR;\n";
        }
    } elsif ($checkError) {
        if ($fname eq "glBegin") {
            # never check error after glBegin
            print "\t\t\t\terror = GL_NO_ERROR;\n";
        } elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
            print "\t\t\t\terror = ORIG_GL(glGetError)();\n";
        } else {
            print "\t\t\t\tif (G.errorCheckAllowed) {\n";
            print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
            print "\t\t\t\t} else {\n\t\t\t\t\terror = GL_NO_ERROR;\n\t\t\t\t}\n";
        }
    } else {
        print "\t\t\t\terror = GL_NO_ERROR;\n";
    }
    print post_execute("\t\t\t\t", $fname, $retval, @arguments);
    print "\t\t\t\tif (error != GL_NO_ERROR) {\n";
    print "\t\t\t\t\tsetErrorCode(error);\n";
    print "\t\t\t\t\tstop();\n";
    print "\t\t\t\t} else {\n";
    if (defined $WIN32) {
        print "\t\t\t\t\trec->isRecursing = 0;\n";
    }
    if ($retval =~ /^void$|^$/i) {
        print "\t\t\t\t\treturn;\n";
    } else {
        print "\t\t\t\t\treturn result;\n";
    }
    print "\t\t\t\t}\n";
    print "\t\t\t} else {\n";
    if (scalar grep {$fname eq $_} @postExecutionList) {
        print "\t\t\t\t\terror = GL_NO_ERROR;\n";
        print post_execute("\t\t\t\t", $fname, $retval, @arguments);
        print "\t\t\t\tif (error != GL_NO_ERROR) {\n";
        print "\t\t\t\t\tsetErrorCode(error);\n";
        print "\t\t\t\t\tstop();\n";
        print "\t\t\t\t} else {\n";
        if(defined $WIN32) {
            print "\t\t\t\t\trec->isRecursing = 0;\n";
        }
        if ($retval =~ /^void$|^$/i) {
            print "\t\t\t\t\treturn;\n";
        } else {
            print "\t\t\t\t\treturn result;\n";
        }
        print "\t\t\t\t}\n";
    } else {
        if(defined $WIN32) {
            print "\t\t\t\trec->isRecursing = 0;\n";
        }
        if ($retval =~ /^void$|^$/i) {
            print "\t\t\t\treturn;\n";
        } else {
            print "\t\t\t\treturn result;\n";
        }
    }
    print "\t\t\t}";

    print "\n\t\t}";

    #print "\t\tfprintf(stderr, \"ThreadID: %li\\n\", (unsigned long)pthread_self());\n";
    print "
        storeFunctionCall(\"$fname\", ";
    if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
        printf("%i, ", $#arguments + 1);
        for (my $i = 0; $i <= $#arguments; $i++) {
            print "&arg$i, ";
            print getTypeId(@arguments[$i]);
            if ($i != $#arguments) {
                print ", ";
            }
        }
    } else {
        print "0";
    }
    print ");
        stop();
        op = getDbgOperation();
        while (op != DBG_DONE) {
            switch (op) {
            case DBG_CALL_FUNCTION:\n";
    if ($retval !~ /^void$|^$/i) {
        print "\t\t\t\tresult = ";
    } else {
        print "\t\t\t\t";
    }
    print "(($pfname)getDbgFunction())(";
    if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
        for (my $i = 0; $i <= $#arguments; $i++) {
            print "arg$i";
            if ($i != $#arguments) {
                print ", ";
            }
        }
    }
    print ");";
    if ($retval !~ /^void$|^$/i) {
        print "\n\t\t\t\tstoreResult(&result, ";
        print getTypeId($retval);
        print ");"
    }
    print "
                break;
            case DBG_RECORD_CALL:\n#ifdef DBG_STREAM_HINT_";
    printf("%s", uc($fname));
    print "\n#  if DBG_STREAM_HINT_";
    printf("%s", uc($fname));
    print " != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, \"$fname\", ";
    if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
        printf("%i, ", $#arguments + 1);
        for (my $i = 0; $i <= $#arguments; $i++) {
            if (@arguments[$i] =~ /[*]$/) {
                if (scalar grep {$fname eq $_} @justCopyPointersList) {
                    print "&arg$i, sizeof(void*)";
                } else {
                    print "arg$i, ";
                    if ($fname =~ /gl\D+([1234])\D{1,3}v[A-Z]*/ &&
                        $fname !~ /glProgramNamedParameter\SvNV/) {
                        print "$1*sizeof(";
                        my $type = stripStorageQualifiers(@arguments[$i]);
                        $type =~ s/\*//;
                        $type =~ s/\s*$//;
                        print "$type)";
                    } else {
                        print "$fname";
                        print "_getArg$i";
                        print "Size(";
                        for (my $j = 0; $j <= $#arguments; $j++) {
                            print "arg$j";
                            if ($j != $#arguments) {
                                print ", ";
                            }
                        }
                        print ")"
                    }
                }
            } else {
                print "&arg$i, ";
                printf("sizeof(%s)", stripStorageQualifiers(@arguments[$i]));
            }
            if ($i != $#arguments) {
                print ", ";
            }
        }
    } else {
        print "0";
    }
    print ");\n#  endif\n#  if DBG_STREAM_HINT_";
    printf("%s", uc($fname));
    print " == DBG_RECORD_AND_FINAL
                break;\n#  else
                /* FALLTHROUGH!!!! */\n#  endif\n#endif
            case DBG_CALL_ORIGFUNCTION:\n";
    printPreExecute("\t\t\t\t", $fname, @arguments);
    if ($retval !~ /^void$|^$/i) {
        print "\t\t\t\tresult = ";
    } else {
        print "\t\t\t\t";
    }
    print "ORIG_GL($fname)(";
    print arguments_string(@arguments);
    print ");\n";
    if ($retval !~ /^void$|^$/i) {
        if ($checkError) {
            if (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
                print "\t\t\t\terror = ORIG_GL(glGetError)();\n";
                print post_execute("\t\t\t\t", $fname, $retval, @arguments);
                print "\t\t\t\tstoreResultOrError(error, &result, ";
                print getTypeId($retval);
                print ");";
            } else {
                print "\t\t\t\tif (G.errorCheckAllowed) {\n";
                print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
                print post_execute("\t\t\t\t\t", $fname, $retval, @arguments);
                print "\t\t\t\t\tstoreResultOrError(error, &result,";
                print getTypeId($retval);
                print ");\n\t\t\t\t} else {";
                print "\n\t\t\t\t\terror = GL_NO_ERROR;\n";
                print post_execute("\t\t\t\t\t", $fname, $retval, @arguments);
                print "\t\t\t\t\tstoreResult(&result, ";
                print getTypeId($retval);
                print ");\n\t\t\t\t}";
            }
        } else {
            print "\t\t\t\terror = GL_NO_ERROR;\n";
            print post_execute("\t\t\t\t", $fname, $retval, @arguments);
            print "\t\t\t\tstoreResult(&result, ";
            print getTypeId($retval);
            print ");\n";
        }
    } elsif ($checkError) {
        if ($fname eq "glBegin") {
            # never check error after glBegin
            print "\n\t\t\t\terror = GL_NO_ERROR;\n";
            print post_execute("\t\t\t\t", $fname, $retval, @arguments);
        } elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
            print "\n\t\t\t\terror = ORIG_GL(glGetError)();\n";
            print post_execute("\t\t\t\t", $fname, $retval, @arguments);
        } else {
            print "\t\t\t\tif (G.errorCheckAllowed) {\n";
            print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
            print "\t\t\t\t} else {\n\t\t\t\t\terror = GL_NO_ERROR;\n\t\t\t\t}\n";
            print post_execute("\t\t\t\t", $fname, $retval, @arguments);
        }
        print "\t\t\t\tsetErrorCode(error);";
    }
    print "\n\t\t\t\tbreak;
            case DBG_EXECUTE:
                setExecuting();
                stop();\n";
    print "\t\t\t\t$unlockStatement";
    print pre_execute("\t\t\t\t", $fname, @arguments);
    if ($retval !~ /^void$|^$/i) {
        if(defined $WIN32) {
            print "\t\t\t\trec->isRecursing = 0;\n";
        }
        print "\t\t\t\tresult = ";
    } else {
        print "\t\t\t\t";
    }
    print "ORIG_GL($fname)(";
    print arguments_string(@arguments);
    print ");";

    print "\n\t\t\t\tif (checkGLErrorInExecution()) {\n";

    if ($retval !~ /^void$|^$/i) {
        if ($checkError) {
            if (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
                print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";

            } else {
                print "\t\t\t\t\tif (G.errorCheckAllowed) {\n";
                print "\t\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
                print "\t\t\t\t\t} else {\n";
                print "\t\t\t\t\t\terror = GL_NO_ERROR;\n";
                print "\t\t\t\t\t}\n";
            }
        } else {
            print "\t\t\t\terror = GL_NO_ERROR;\n";
        }
    } elsif($checkError) {
        if ($fname eq "glBegin") {
            # never check error after glBegin
            print "\t\t\t\t\terror = GL_NO_ERROR;\n";
        } elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
            print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
        } else {
            print "\t\t\t\t\tif (G.errorCheckAllowed) {\n";
            print "\t\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
            print "\t\t\t\t\t} else {\n\t\t\t\t\terror = GL_NO_ERROR;\n\t\t\t\t}\n";
        }
    } else {
        print "\t\t\t\t\terror = GL_NO_ERROR;\n";
    }
    print post_execute("\t\t\t\t\t", $fname, $retval, @arguments);
    print "\t\t\t\t\tif (error != GL_NO_ERROR) {\n";
    print "\t\t\t\t\t\tsetErrorCode(error);\n";
    print "\t\t\t\t\t} else {\n";
    if(defined $WIN32) {
        print "\t\t\t\t\t\trec->isRecursing = 0;\n";
    }
    if ($retval =~ /^void$|^$/i) {
        print "\t\t\t\t\t\treturn;\n";
    } else {
        print "\t\t\t\t\t\treturn result;\n";
    }
    print "\t\t\t\t\t}\n";
    print "\t\t\t\t} else {\n";

    if (scalar grep {$fname eq $_} @postExecutionList) {
        print "\t\t\t\t\terror = GL_NO_ERROR;\n";
        print post_execute("\t\t\t\t\t", $fname, $retval, @arguments);
        print "\t\t\t\t\tif (error != GL_NO_ERROR) {\n";
        print "\t\t\t\t\t\tsetErrorCode(error);\n";
        print "\t\t\t\t\t} else {\n";
        if(defined $WIN32) {
            print "\t\t\t\t\t\trec->isRecursing = 0;\n";
        }
        if ($retval =~ /^void$|^$/i) {
            print "\t\t\t\t\t\treturn;\n";
        } else {
            print "\t\t\t\t\t\treturn result;\n";
        }
        print "\t\t\t\t\t}\n"
    } else {
        if(defined $WIN32) {
            print "\t\t\t\t\trec->isRecursing = 0;\n";
        }
        if ($retval =~ /^void$|^$/i) {
            print "\t\t\t\t\treturn;\n";
        } else {
            print "\t\t\t\t\treturn result;\n";
        }
    }
    print "\t\t\t\t}";

    print "
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);\n";
    print "\t\t$unlockStatement";
    if(defined $WIN32) {
        print "\t\trec->isRecursing = 0;\n";
    }
    if ($retval !~ /^void$|^$/i) {
        print "\t\treturn result;\n"
    }
    print "}\n\n";
}



sub createBodyError {
    createBody(@_, 1);
}

my $actions = {
    $regexps{"typegl"} => \&addTypeMapping,
    $regexps{"wingdi"} => \&createBodyError,
    $regexps{"glapi"} => \&createBodyError,
}

my $add_actions;
if (defined $WIN32) {
    $add_actions = {
        $regexps{"typewgl"} => \&addTypeMapping,
        $regexps{"winapifunc"} => sub {
            my $fname = $_[3];
            createBody(@_, 0) if $fname ne "wglGetProcAddress";
        },
    }
} else {
    $add_actions = {
        $regexps{"typeglx"} => \&addTypeMapping,
        $regexps{"glxfunc"} => sub {
            my $extname = $_[1];
            my $fname = $_[3];

            return if $extname eq "GLX_SGIX_dm_buffer" ||
                      $extname eq "GLX_SGIX_video_source";

            # No way to parse functions returning a not "typedef'ed"
            # function pointer in a simple way :-(
            my @params = ($fname eq "glXGetProcAddressARB") ? (0, 0,
                            "__GLXextFuncPtr", "glXGetProcAddressARB",
                            "const GLubyte *") : @_;
            createBody(@params, 0);
        },
    }
}

header_generated();
parse_gl_files($gl_actions, $add_actions, defined $WIN32, \&createBody);
