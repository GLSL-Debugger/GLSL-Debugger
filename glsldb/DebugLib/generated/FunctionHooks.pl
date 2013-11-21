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

require prePostExecuteList;
require genTools;
require genTypes;
our %regexps;
our %typeMap;


if ($^O =~ /Win32/) {
    $WIN32 = 1;
}


sub check_error_string
{
    my ($check, $void, $fname, $indent) = (@_);
    my $ret =;
    $indent = "    " x $indent;

    if ($check and (not $void or $fname ne "glBegin")) {
        if (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
                $ret = "error = ORIG_GL(glGetError)();\n";
        } else {
            $ret = "if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
} else {
    error = GL_NO_ERROR;
}\n";
        }
    } else {
        # never check error after glBegin
        $ret = "error = GL_NO_ERROR;\n";
    }
    $ret =~ s/^/$indent/;
    return $ret;
}


#==============================================
#   READING BELOW THIS LINE MAY BE HARMFUL
#               STOP HERE
#==============================================


sub check_error_store
{
    my ($check, $void, $fname, $postexec, $return_type, $indent) = (@_);
    return "" if $void and not $check;

    my $ret = "";
    my ($storeResult, $storeResultErr);
    $indent = "    " x $indent;

    if (not $void){
        my $storeResult = "storeResult(&result, $return_type);";
        my $storeResultErr = "storeResultOrError(error, &result, $return_type);";
    }

    if ($check) {
        if ($fname eq "glBegin") {
            # never check error after glBegin
            $ret = "error = GL_NO_ERROR;
$postexec
";
        }elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
            $ret = "error = ORIG_GL(glGetError)();
$postexec
$storeResultErr
";
        } else {
            $ret = "if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();
    $postexec
    $storeResultErr
} else {
    error = GL_NO_ERROR;
    $postexec
    $storeResult
}
";
        }
        $ret .= "setErrorCode(error);\n" if $void;
    } else {
        $ret = "error = GL_NO_ERROR;
$postexec
$storeResult
";
    }

    $ret =~ s/^/$indent/;
    return $ret;
}


sub thread_statement
{
    my ($fname, $retval, $retval_assign, $return_name, @arguments) = (@_);
    my ($lockStatement, $unlockStatement);
    if (defined $WIN32) {
        my $preexec = pre_execute($fname, @arguments);
        my $postexec = post_execute($fname, $retval, @arguments);
        my $argstring = arguments_string(@arguments);

        my $check_allowed = "";
        $check_allowed = "G.errorCheckAllowed = 1;" if $fname eq "glEnd";
        $check_allowed = "G.errorCheckAllowed = 0;" if $fname eq "glBegin";

        $lockStatement = "DbgRec *rec;
    dbgPrint(DBGLVL_DEBUG, \"entering $fname\\n\");
    rec = getThreadRecord(GetCurrentProcessId());
    $check_allowed
    if(rec->isRecursing) {
        dbgPrint(DBGLVL_DEBUG, \"stopping recursion\\n\");
        $preexec
        $retval_assign ORIG_GL($fname)($argstring);
        /* no way to check errors in recursive calls! */
        error = GL_NO_ERROR;
        $postexec
        return $return_name;
    }
    rec->isRecursing = 1;
    EnterCriticalSection(&G.lock);
";
        $unlockStatement = "LeaveCriticalSection(&G.lock);";
    } else {
        $lockStatement = "pthread_mutex_lock(&G.lock);";
        $unlockStatement = "pthread_mutex_unlock(&G.lock);";
    }

    return $lockStatement, $unlockStatement;
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

    my $return_void = $retval =~ /^void$|^$/i;
    my $return_type = getTypeId($retval);
    my $return_name =  $return_void ? "" : " result";
    my $retval_assign = $return_void ? "" : "result =";
    my $retval_init = $return_void ? "" : "${retval}${return_name};";
    my $argstring = arguments_string(@arguments);
    my $argrefstring = arguments_references(@arguments);
    my $argtypes = join("", map { ", &arg$_, " . getTypeId(
                            @arguments[$_]) } (0..$#arguments));
    my $argsizes = arguments_sizes($fname, @arguments);

    my $argcount = 0;
    if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
        $argcount = $#arguments + 1;
    }

    my $ucfname = uc($fname);
    my $win_recursing = defined $WIN32 ? "rec->isRecursing = 0;" : "";
    my $preexec = pre_execute($fname, @arguments);
    my $postexec = post_execute($fname, $retval, @arguments);

    my ($lockStatement, $unlockStatement) = thread_statement($fname,
                    $retval, $retval_assign, $return_name, @arguments);

    my $errstr4 = check_error_string($checkError, $return_void, $fname, 4);
    my $errpostexec4 = error_postexec($fname, $postexec, $win_recursing,
                        $return_name 4, 1);

    ###########################################################################
    # create function head
    ###########################################################################
    if (defined $WIN32) {
        print "__declspec(dllexport) $retval APIENTRY Detoured$fname (";
    } else {
        print "$retval $fname (";
    }
    # add arguments to function head
    if (@arguments[0] ~= /^void$|^$/i){
        print "void"
    } else {
        my $i = 0;
        print join(", ", map {
                    (/(.*)(\[\d+\])/ ? "$1 arg$i$2" : "$_ arg$i"), ++$i
                } @arguments);
    }

    print ")
{
    // temp. variable that holds the return value of the function
    $retval_init
    //##########################################################################
    // first store name of called function and its arguments in the shared memory
    // segment, then call dbgFunctionCall that stops the process/thread and waits
    // for the debugger to handle the actual debugging
    int op, error;
    $lockStatement
    if (keepExecuting(\"$fname\")) {
        $unlockStatement
        $preexec
        $retval_assign ORIG_GL($fname)($argstring);
        if (checkGLErrorInExecution()) {
            $errstr4
            $postexec
            if (error != GL_NO_ERROR) {
                setErrorCode(error);
                stop();
            } else {
                $win_recursing
                return $return_name;
            }
        } else {
            $errpostexec4
        }
    }
    //fprintf(stderr, \"ThreadID: %li\\n\", (unsigned long)pthread_self());
    storeFunctionCall(\"$fname\", ${argcount}${argtypes});
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
            case DBG_CALL_FUNCTION:
                $retval_assign (($pfname)getDbgFunction())($argstring);",
    if (not $return_void) {
        print "
                storeResult(&result, $return_type);";
    }
    printf "
                break;
            case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_$ucfname
#  if DBG_STREAM_HINT_$ucfname != DBG_NO_RECORD
                recordFunctionCall(&G.recordedStream, \"$fname\",
                            ${argcount}${argsizes});
#  endif
#  if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL
                break;
#  else
                /* FALLTHROUGH!!!! */
#  endif
#endif
            case DBG_CALL_ORIGFUNCTION:
                $preexec
                $retval_assign ORIG_GL($fname)($argstring);
                %s
                break;
            case DBG_EXECUTE:
                setExecuting();
                stop();
                $unlockStatement
                $preexec
                $win_recursing
                $retval_assign ORIG_GL($fname)($argstring);
                if (checkGLErrorInExecution()) {
                    %s
                    $postexec
                    if (error != GL_NO_ERROR) {
                        setErrorCode(error);
                    } else {
                        $win_recursing
                        return $return_name;
                    }
                } else {
                    %s
                }
            default:
                executeDefaultDbgOperation(op);
            }
            stop();
            op = getDbgOperation();
        }
        setErrorCode(DBG_NO_ERROR);
        $unlockStatement
        $win_recursing
        return $return_name;
    }
", check_error_store($checkError, $return_void, $fname, $postexec, $return_type, 4),
    check_error_string($checkError, $return_void, $fname, 5),
    error_postexec($fname, $postexec, $win_recursing, $return_name 5);
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
