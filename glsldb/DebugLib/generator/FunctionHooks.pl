################################################################################
#
# Copyright (c) 2013, 2014 SirAnthony <anthony at adsorbtion.org>
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

use FindBin;
use lib "$FindBin::Bin";

use Getopt::Std;
require prePostExecuteList;
require genTools;
require genTypes;

# Use path provided by generator
getopt('p');
require "$opt_p/functionsAllowedInBeginEnd.pm";

our %regexps;
our %typeMap;
my %beginEnd = map {$_ => 1} @allowedInBeginEnd;


if ($^O =~ /Win32/) {
    $WIN32 = 1;
}

sub print_defines
{
	if (defined $WIN32)	{
		print "#define ENTER_CS EnterCriticalSection
#define EXIT_CS LeaveCriticalSection
#define RECURSING(n) rec->isRecursing = n;
"
	} else {
		print "#define ENTER_CS pthread_mutex_lock
#define EXIT_CS pthread_mutex_unlock
#define RECURSING(n)
"
	}

	print "
int check_error(int check_allowed, int dummy)
{
	if (!dummy && (!check_allowed || G.errorCheckAllowed))
		return ORIG_GL(glGetError)();
	return GL_NO_ERROR;
}

int check_error_code(int error, int do_stop)
{
	if (error != GL_NO_ERROR) {
		setErrorCode(error);
		if (do_stop)
			stop();
	} else {
		return 1;
	}
	return 0;
}
"
}

sub check_error_string
{
    my ($check, $void, $fname) = @_;
	# never check error after glBegin
    return sprintf "check_error(%i, %i)",
		int($beginEnd{$fname}),
		($check and (not $void or $fname ne "glBegin")) ? 0 : 1;
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
    $postexec = "\n" . $postexec if $postexec;

    if (not $void){
        my $storeResult = "storeResult(&result, $return_type);";
        my $storeResultErr = "storeResultOrError(error, &result, $return_type);";
    }

    if ($check) {
        if ($fname eq "glBegin") {
            # never check error after glBegin
            $ret = "error = GL_NO_ERROR;${postexec}";
        }elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
            $ret = "error = ORIG_GL(glGetError)();${postexec}${storeResultErr}";
        } else {
            $ret = "if (G.errorCheckAllowed) {
    error = ORIG_GL(glGetError)();${postexec}${storeResultErr}
} else {
    error = GL_NO_ERROR;${postexec}${storeResult}
}";
        }
        $ret = "$ret
setErrorCode(error);" if $void;
    } else {
        $ret = "error = GL_NO_ERROR;${postexec}${storeResult}";
    }


    $ret =~ s/^/$indent/g;
    $ret =~ s/\n/\n$indent/g;
    return $ret;
}

sub thread_statement
{
    my ($fname, $retval, $retval_assign, $return_name, @arguments) = (@_);
    my $statement;
    if (defined $WIN32) {
        my $preexec = pre_execute($fname, @arguments);
        my $postexec = post_execute($fname, $retval, @arguments);
        my $argstring = arguments_string(@arguments);

        my $check_allowed = "";
        $check_allowed = "G.errorCheckAllowed = 1;\n" if $fname eq "glEnd";
        $check_allowed = "G.errorCheckAllowed = 0;\n" if $fname eq "glBegin";
        $statement = "
	DbgRec *rec;
    dbgPrint(DBGLVL_DEBUG, \"entering $fname\\n\");
    rec = getThreadRecord(GetCurrentProcessId());
    ${check_allowed}if(rec->isRecursing) {
        dbgPrint(DBGLVL_DEBUG, \"stopping recursion\\n\");
        ${preexec}${retval_assign}ORIG_GL($fname)($argstring);
        /* no way to check errors in recursive calls! */
        error = GL_NO_ERROR;
        ${postexec}return $return_name;
    }
    rec->isRecursing = 1;";
    }
    return $statement;
}


# TODO: check position of unlock statements!!!
@defined_funcs = (
	"wglGetProcAddress"	# Hooked
);

sub createBody
{
    my $isExtension = shift;
    my $extname = shift;
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my $checkError = shift;
    $fname =~ s/^\s+|\s+$//g;
    $retval =~ s/^\s+|\s+$//g;
    return if grep(/^$fname$/i, @defined_funcs);
    push(@defined_funcs, $fname);

    my @arguments = buildArgumentList($argString);
    my $pfname = join("","PFN",uc($fname),"PROC");

    my $return_void = $retval =~ /^void$|^$/i;
    my $return_type = $return_void ? "void" : getTypeId($retval);
    my $return_name =  $return_void ? "" : "result";
    my $retval_assign = $return_void ? "" : "${return_name} = ";
    # temp. variable that holds the return value of the function
    my $retval_init = $return_void ? "" : "${retval} ${return_name};\n    ";

    my $argstring = arguments_string(@arguments);
    my $argrefstring = arguments_references(@arguments);
    my $argsizes = arguments_sizes($fname, @arguments);
    $argsizes = ", " . $argsizes if $argsizes;

    my $argcount = 0;
    my $argtypes = "";
    if ($#arguments > 0 || @arguments[0] !~ /^void$|^$/i) {
        $argcount = $#arguments + 1;
        $argtypes = join("", map { ", &arg$_, " . getTypeId(
                            @arguments[$_]) } (0..$#arguments));
    }

    my $ucfname = uc($fname);
    my $win_recursing = defined $WIN32 ? "rec->isRecursing = 0;\n" : "";
    my $preexec = pre_execute($fname, @arguments);
    my $postexec = post_execute($fname, $retval, @arguments);

    my $thread_statement = thread_statement($fname,
                    $retval, $retval_assign, $return_name, @arguments);
    my $errstr = check_error_string($checkError, $return_void, $fname);

    my $output = "";
    ###########################################################################
    # create function head
    ###########################################################################
    if (defined $WIN32) {
        $output .= "__declspec(dllexport) $retval APIENTRY Hooked$fname (";
    } else {
        $output .= "$retval $fname (";
    }
    # add arguments to function head
    if (@arguments[0] =~ /^void$|^$/i){
        $output .= "void"
    } else {
        my $i = 0;
        $output .= join(", ", map { (@arguments[$_] =~ /(.*)(\[\d+\])/ ?
                "$1 arg$_$2" : (@arguments[$_] . " arg$_"))
                } (0..$#arguments));
    }

    ###########################################################################
    # first store name of called function and its arguments in the shared memory
    # segment, then call dbgFunctionCall that stops the process/thread and waits
    # for the debugger to handle the actual debugging
    $output .= ")
{
    ${retval_init}int op, error;${thread_statement}
	ENTER_CS(&G.lock);
    if (keepExecuting(\"$fname\")) {
        EXIT_CS(&G.lock);
        ${preexec}${retval_assign}ORIG_GL($fname)($argstring);
		error = GL_NO_ERROR;
        if (checkGLErrorInExecution())
			error = ${errstr};
		${postexec}if (check_error_code(error, 1)){
			RECURSING(0)
			return  $return_name;
		}
    }
    //fprintf(stderr, \"ThreadID: %li\\n\", (unsigned long)pthread_self());
    storeFunctionCall(\"$fname\", ${argcount}${argtypes});
    stop();
    op = getDbgOperation();
    while (op != DBG_DONE) {
        switch (op) {
        case DBG_CALL_FUNCTION:
            ${retval_assign}(($pfname)getDbgFunction())($argstring);";
    if (not $return_void) {
        $output .= "
            storeResult(&result, $return_type);";
    }
    $output .= sprintf "
            break;
        case DBG_RECORD_CALL:
#ifdef DBG_STREAM_HINT_$ucfname
#  if DBG_STREAM_HINT_$ucfname != DBG_NO_RECORD
            recordFunctionCall(&G.recordedStream, \"$fname\", ${argcount}${argsizes});
#  endif
#  if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL
            break;
#  else
            /* FALLTHROUGH!!!! */
#  endif
#endif
        case DBG_CALL_ORIGFUNCTION:
            ${preexec}${retval_assign}ORIG_GL($fname)($argstring);
%s
            break;
        case DBG_EXECUTE:
            setExecuting();
            stop();
            EXIT_CS(&G.lock);
            ${preexec}${win_recursing}${retval_assign}ORIG_GL($fname)($argstring);
			error = GL_NO_ERROR;
            if (checkGLErrorInExecution())
				error = ${errstr};
			${postexec}if (check_error_code(error, 0)){
				RECURSING(0)
				return  $return_name;
			}
        default:
            executeDefaultDbgOperation(op);
        }
        stop();
        op = getDbgOperation();
    }
    setErrorCode(DBG_NO_ERROR);
    EXIT_CS(&G.lock);
    ${win_recursing}return $return_name;
}

", check_error_store($checkError, $return_void, $fname, $postexec, $return_type, 3);

    $output =~ s/    /\t/g;
    $output =~ s/return ;/return;/g;
    print $output;
}



sub createBodyError {
    createBody(@_, 1);
}

my $gl_actions = {
    $regexps{"typegl"} => \&addTypeMapping,
    $regexps{"glapi"} => \&createBodyError,
};

my $add_actions;
if (defined $WIN32) {
    $add_actions = {
        $regexps{"wingdi"} => \&createBodyError,
        $regexps{"typewgl"} => \&addTypeMapping,
        $regexps{"winapifunc"} => \&createBody,
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
print_defines();
parse_gl_files($gl_actions, $add_actions, defined $WIN32, \&createBody);
