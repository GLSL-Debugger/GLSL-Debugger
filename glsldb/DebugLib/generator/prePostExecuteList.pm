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

# this list contains functions for which a special pre-execution function exits
# which has to be called just before executing the original functions in a
# function hook. These functions have to be provided in preExecution.c and have
# to be called *_PREEXECUTE. They take pointer arguments to all parameters of
# the original function
my @preExecutionList = (
    glBeginQuery,
    glBeginQueryARB,
    glBeginOcclusionQueryNV,
);

# this list contains functions for which a special post-execution function exits
# which has to be called just after executing the original functions in a
# function hook. These functions have to be provided in postExecution.c and have
# to be called *_POSTEXECUTE. They take pointer arguments to all parameters of
# the original function, the result variable, and the error code of the original
# call
my @postExecutionList = (
    glGetQueryObjectiv,
    glGetQueryObjectuiv,
    glGetQueryObjectivARB,
    glGetQueryObjectuivARB,
    glGetOcclusionQueryivNV,
    glGetOcclusionQueryuivNV,
);

# this list contains functions for which a special treatment of pointer
# arguments is necessary, i.e. we do not want to copy the content the pointer
# references but the pointer value instead.
my @justCopyPointersList = (
    glDrawElements,
    glDrawElementsARB,
    glDrawElementsInstancedEXT,
    glDrawRangeElements,
    glDrawRangeElementsEXT,
    glMultiDrawElements,
    glMultiDrawElementsEXT,
    glMultiDrawArrays,
    glMultiDrawArraysEXT
);


sub arguments_references
{
    return if @_[0] =~ /^void$|^$/i;
    return join(", ", map {"&arg$_"} (0..$#_));
}

sub arguments_string
{
    return if @_[0] =~ /^void$|^$/i;
    return join(", ", map {"arg$_"} (0..$#_));
}

sub arguments_sizes
{
    my ($fname, @arguments) = (@_);
    my @converted_args = ();

    return "" if not $#arguments or @arguments[0] =~ /^void$|^$/i;

    for (my $i = 0; $i <= $#arguments; $i++) {
        my $type = stripStorageQualifiers(@arguments[$i]);
        if (@arguments[$i] =~ /[*]$/) {
            if (scalar grep {$fname eq $_} @justCopyPointersList) {
                push @converted_args, "&arg$i, sizeof(void*)";
            } else {
                push @converted_args, "arg$i";
                if ($fname =~ /gl\D+([1234])\D{1,3}v[A-Z]*/ &&
                    $fname !~ /glProgramNamedParameter\SvNV/) {
                    push @converted_args, "$1*sizeof($type)";
                } else {
                    push @converted_args, sprintf(
                            "${fname}_getArg${i}Size(%s)",
                            arguments_string(@arguments));
                }
            }
        } else {
            push @converted_args, "&arg$i, sizeof($type)"
        }
    }

    return join(", ", @converted_args);
}

sub error_postexec
{
    my ($fname, $postexec, $win_recursing, $return_name, $indent, $need_stop) = (@_);
    my $stop = $need_stop ? "stop();" : "";
    my $ret = "";
    $indent = "    " x $indent;
    if (scalar grep {$fname eq $_} @postExecutionList) {
        $ret = "error = GL_NO_ERROR;
${postexec}if (error != GL_NO_ERROR) {
    setErrorCode(error);
} else {
    ${win_recursing}return $return_name;
}";
    } else {
        $ret = "${win_recursing}return $return_name;";
    }
    $ret =~ s/^/$indent/;
    return $ret;
}

sub pre_execute {
    my ($fname, @arguments) = @_;
    if (scalar grep {$fname eq $_} @preExecutionList) {
        return sprintf "${fname}_PREEXECUTE(%s);\n",
                            arguments_references(@arguments);
    }
    return "";
}


sub post_execute
{
    my ($fname, $retval, @arguments) = @_;
    if (scalar grep {$fname eq $_} @postExecutionList) {
        return sprintf "${fname}_POSTEXECUTE(%s%s, &error);\n",
                    arguments_references(@arguments),
                    ($retval !~ /^void$|^$/i ? ", &result" : "");
    }
    return "";
}
