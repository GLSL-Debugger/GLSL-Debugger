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

my @storageQualifiers = (
    "register",
    "restricted",
    "volatile",
    "const",
    "CONST"
);
my @structTypes = (
    "struct",
    "union",
    "enum"
);

my @typeQualifiers = (
    "signed",
    "unsigned",
    "short",
    "long"
);

my @cTypes = (
    "char",
    "int",
    "float",
    "double"
);

sub stripStorageQualifiers
{
    my $arg = shift;
    my $stripedArg = "";
    my @argItems = grep { not /^\s*$/ } split(/(\s|[*\[\]])/, $arg);
    foreach(@argItems) {
        my $argItem = $_;
        if (not scalar grep {$argItem eq $_} @storageQualifiers) {
            $stripedArg .= "$argItem ";
        }
    }
    chop $stripedArg;
    return $stripedArg;
}

sub getBasicTypeId
{
    my $arg = shift;
    my $id;
    if ($arg =~ /[*]|[\[]/) {
        $id = "DBG_TYPE_POINTER";
    } elsif ($arg =~ /struct|union/) {
        $id = "DBG_TYPE_STRUCT";
    } elsif ($arg =~ /float/) {
        $id = "DBG_TYPE_FLOAT";
    } elsif ($arg =~ /double/) {
        $id = "DBG_TYPE";
        my $N = $arg =~ s/(long)/$1/g;
        for ($i = 0; $i < $N; $i++) {
            $id .= "_LONG";
        }
        $id .= "_DOUBLE";
    } elsif ($arg =~ /char/) {
        $id = "DBG_TYPE";
        if ($arg =~ /unsigned/) {
            $id .= "_UNSIGNED";
        }
        $id .= "_CHAR";
    } elsif ($arg =~ /short/) {
            $id = "DBG_TYPE";
        if ($arg =~ /unsigned/) {
            $id .= "_UNSIGNED";
        }
        $id .= "_SHORT_INT";
    } elsif ($arg =~ /int|long|unsigned|signed/) {
        $id = "DBG_TYPE";
        if ($arg =~ /unsigned/) {
            $id .= "_UNSIGNED";
        }
        my $N = $arg =~ s/(long)/$1/g;
        for ($i = 0; $i < $N; $i++) {
            $id .= "_LONG";
        }
        $id .= "_INT";
    } else {
        die "getBasicTypeId: Cannot determine argument type of \"$arg\"\n";
    }
    return $id;
}

sub buildArgumentList
{
    my $argString = shift;
    my @rawArgList = split(/,/, $argString);
    my @argList = ();
    foreach (@rawArgList) {
        s/^\s*//;
        s/\s+/ /;
        my @argItems = grep { not /^\s*$/ } split(/(\s|[*\[])/);
        #/(.*?GL\w+(?:\s*[*]){0,2})/;
        #/(.*)/;
        my $instruct = 0;
        my $haveType = 0;
        my $haveTypeQualifier = 0;
        my $inArrayDec = 0;
        my $argType = "";
        #print "argItems: #"; print join('#', @argItems); print "#\n";
        foreach $argItem (@argItems) {
            if (scalar grep {$argItem eq $_} @storageQualifiers) {
                $argType .= "$argItem ";
            } elsif (scalar grep {$argItem eq $_} @typeQualifiers) {
                $haveTypeQualifier = 1;
                $argType .= "$argItem ";
            } elsif (scalar grep {$argItem eq $_} @structTypes) {
                $argType .= "$argItem ";
                $instruct = 1;
            } elsif (scalar grep {$argItem eq $_} @cTypes) {
                $argType .= "$argItem ";
                $haveType = 1;
            } elsif ($argItem eq "*") {
                $argType .= "$argItem ";
            } elsif ($argItem eq "[") {
                $inArrayDec = 1;
                $argType .= "$argItem";
            } elsif ($argItem eq "]") {
                $inArrayDec = 0;
                $argType .= "$argItem";
            } else {
                if ($instruct) {
                    $argType .= "$argItem ";
                    $instruct = 0;
                    $haveType = 1;
                } elsif ($inArrayDec == 1) {
                    $argType .= "$argItem ";
                } elsif ($haveType == 0 && $haveTypeQualifier == 0) {
                    $argType .= "$argItem ";
                    $haveType = 1;
                }
            }
        }
        chop $argType;
        push @argList, $argType;
    }
    return @argList;
}

