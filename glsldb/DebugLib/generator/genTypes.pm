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


my @storageQualifiers = (
    "register",
    "restricted",
    "volatile",
    "const",
    "CONST"
);

my %isStorageQualifier = map {$_ => 1} @storageQualifiers;

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

# use only basic C types and size qualifiers, struct, or * (i.e pointer)
our %typeMap = (
    # GL
    "ptrdiff_t" => "long int", # FIXME: 32/64bit issue
    # GLX
    "Pixmap" => "unsigned long int", # FIXME: 32/64bit issue
    "Colormap" => "unsigned long int", # FIXME: 32/64bit issue
    "Window" => "unsigned long int", # FIXME: 32/64bit issue
    "int32_t" => "int", # FIXME: 32/64bit issue
    "int64_t" => "long", # FIXME: 32/64bit issue
    "Bool" => "int",
    "Font" => "unsigned long int", # FIXME: 32/64bit issue
    "XID" => "unsigned long int", # FIXME: 32/64bit issue
    "Status" => "int",
    "__GLXextFuncPtr" => "void *",
    "VLServer" => "void *",
    "VLPath" => "int",
    "VLNode" => "int",
    "DMbuffer" => "void *",
    "DMparams" => "struct",
    "HDC" => "void *",
    "HANDLE" => "void *",
    "UINT" => "unsigned int",
    "FLOAT" => "float",
    "INT" => "int",
    "DWORD" => "unsigned int",
    "HGLRC" => "void *",
    "LPCSTR" => "const char *",
    "BOOL" => "int",
    "PROC" => "void *",
    "HPBUFFERARB" => "void *",
    "HPBUFFEREXT" => "void *",
    "HGPUNV" => "void *",
    "INT64" => "__int64",
    "LPVOID" => "void *",
    "PGPU_DEVICE" => "void *",
    "GPU_DEVICE" => "struct",
    "LPGLYPHMETRICSFLOAT" => "void *",
    "LPLAYERPLANEDESCRIPTOR" => "void *",
    "HVIDEOOUTPUTDEVICENV" => "void *",
    "HPVIDEODEV" => "void *",
    "GLhandleARB" => "unsigned int",
);

sub addTypeMapping
{
    my $line = shift;
    my $extname = shift;
    my $oldtype = shift;
    my $newtype = shift;
    $oldtype =~ s/\s+/ /g;
    $newtype =~ s/\s+/ /g;
    while (my $prevType = $typeMap{$oldtype}) {
        $oldtype = $prevType;
    }
    #print "addTypeMapping: $newtype -> $oldtype\n";
    $typeMap{$newtype} = $oldtype;
}


sub stripStorageQualifiers
{
    my $arg = shift;
    my @argItems = grep { not /^\s*$/ } split(/(\s|[*\[\]])/, $arg);
    my $stripedArg = join(" ", grep {
            not $isStorageQualifier{$_}} @argItems);
    $stripedArg=~ s/^\s+|\s+$//g;
    return $stripedArg;
}

sub getBasicTypeId
{
    my $arg = shift;
    my $id = "DBG_TYPE";
    if ($arg =~ /[*]|[\[]/) {
        $id .= "_POINTER";
    } elsif ($arg =~ /struct|union/) {
        $id .= "_STRUCT";
    } elsif ($arg =~ /float/) {
        $id .= "_FLOAT";
    } elsif ($arg =~ /double/) {
        my $N = $arg =~ s/(long)/$1/g;
        for ($i = 0; $i < $N; $i++) {
            $id .= "_LONG";
        }
        $id .= "_DOUBLE";
    } elsif ($arg =~ /char/) {
        if ($arg =~ /unsigned/) {
            $id .= "_UNSIGNED";
        }
        $id .= "_CHAR";
    } elsif ($arg =~ /short/) {
        if ($arg =~ /unsigned/) {
            $id .= "_UNSIGNED";
        }
        $id .= "_SHORT_INT";
    } elsif ($arg =~ /int|long|unsigned|signed/) {
        if ($arg =~ /unsigned/) {
            $id .= "_UNSIGNED";
        }
        my $N = $arg =~ s/(long)/$1/g;
        for ($i = 0; $i < $N; $i++) {
            $id .= "_LONG";
        }
        $id .= "_INT";
    } else {
        die "getBasicTypeId: Cannot determine argument type of $arg\n";
    }
    return $id;
}

sub getTypeId
{
    my $arg = stripStorageQualifiers(shift);
    #print "STRIPED ARG: ##$arg##\n";
    if ($arg =~ /[*]|[\[]/) {
        return "DBG_TYPE_POINTER";
    } elsif ($arg =~ /GLbitfield/) {
        return "DBG_TYPE_BITFIELD";
    } elsif ($arg =~ /GLenum/) {
        return "DBG_TYPE_ENUM";
    } elsif ($arg =~ /GLboolean/) {
        return "DBG_TYPE_BOOLEAN";
    } elsif ($arg =~ /struct|union/) {
        return "DBG_TYPE_STRUCT";
    } elsif ($arg =~ /enum/) {
        return "DBG_TYPE_INT";
    } elsif ($typeMap{$arg}) {
        return getBasicTypeId($typeMap{$arg});
    } else {
        return getBasicTypeId($arg);
    }
}


sub getDummyValue
{
    my $tdarg = shift;
    my $stdarg = stripStorageQualifiers($tdarg);
    my $arg;
    if ($typeMap{$stdarg}) {
        $arg =  $typeMap{$stdarg}
    } else {
        $arg = $tdarg;
    }
    if ($arg =~ /[*]|[\[]/) {
        return "($arg)(void *)dirtyHack";
    } elsif ($arg =~ /struct|union/) {
        return "*($arg *)(void *)dirtyHack";
    } elsif ($arg =~ /float/) {
        return "1.0f";
    } elsif ($arg =~ /double/) {
        return "1.0";
    } elsif ($arg =~ /enum|char|short|int|long|unsigned|signed/) {
        return "1";
    } elsif ($arg =~ /void/) {
        return "";
    } else {
        die "getBasicTypeId: Cannot determine argument type of \"$arg\"\n";
    }
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
