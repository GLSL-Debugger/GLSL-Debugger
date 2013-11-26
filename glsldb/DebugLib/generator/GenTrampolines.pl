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

require "argumentListTools.pl";

# TODO: possibly bullshit, need to check for WINGDIAPI/extern stuff

my @initializer = ();
my @extinitializer = ();
my @attach =();
my @detach = ();

sub createUtils {
print qq|
static VOID _dbg_Dump(PBYTE pbBytes, LONG nBytes, PBYTE pbTarget)
{
    LONG n, m;
    for (n = 0; n < nBytes; n += 16) {
        dbgPrintNoPrefix(DBGLVL_DEBUG, "    %p: ", pbBytes + n);
        for (m = n; m < n + 16; m++) {
            if (m >= nBytes) {
                dbgPrintNoPrefix(DBGLVL_DEBUG, "  ");
            }
            else {
                dbgPrintNoPrefix(DBGLVL_DEBUG, "%02x", pbBytes[m]);
            }
            if (m % 4 == 3) {
                dbgPrintNoPrefix(DBGLVL_DEBUG, " ");
            }
        }
        if (n == 0 && pbTarget != DETOUR_INSTRUCTION_TARGET_NONE) {
            dbgPrintNoPrefix(DBGLVL_DEBUG, " [%p]", pbTarget);
        }
        dbgPrintNoPrefix(DBGLVL_DEBUG, "\\n");
    }
}

static VOID _dbg_Decode(PCSTR pszDesc, PBYTE pbCode, PBYTE pbOther, PBYTE pbPointer, LONG nInst)
{
    PBYTE pbSrc;
    PBYTE pbEnd;
    PVOID pbTarget;
    LONG n;

    if (pbCode != pbPointer) {
        dbgPrint(DBGLVL_DEBUG, "  %s = %p [%p]\\n", pszDesc, pbCode, pbPointer);
    }
    else {
        dbgPrint(DBGLVL_DEBUG, "  %s = %p\\n", pszDesc, pbCode);
    }

    if (pbCode == pbOther) {
        dbgPrint(DBGLVL_DEBUG, "    ... unchanged ...\\n");
        return;
    }

    pbSrc = pbCode;
    for (n = 0; n < nInst; n++) {
        pbEnd = (PBYTE)DetourCopyInstruction(NULL, pbSrc, &pbTarget);
        _dbg_Dump(pbSrc, (int)(pbEnd - pbSrc), (PBYTE)pbTarget);
        pbSrc = pbEnd;
    }
}


VOID WINAPI _dbg_Verify(PCHAR pszFunc, PVOID pvPointer)
{
    PVOID pvCode = DetourCodeFromPointer(pvPointer, NULL);

    _dbg_Decode(pszFunc, (PBYTE)pvCode, NULL, (PBYTE)pvPointer, 3);
}
|;
}

sub createExtensionTrampolineDefinition
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my @arguments = buildArgumentList($argString);
    my $argList = "";
    for (my $i = 0; $i <= $#arguments; $i++) {
        $argList .= "@arguments[$i]";
        if ($i != $#arguments) {
            $argList .= ", ";
        }
    }

    print "$retval (APIENTRYP Orig$fname)($argList) = NULL;\n";
    print "/* Forward declaration: */ __declspec(dllexport) $retval APIENTRY Detoured$fname($argList);\n";

    push @extinitializer, "\tOrig$fname = ($retval (APIENTRYP)($argList)) OrigwglGetProcAddress(\"$fname\");";
}

sub createTrampolineDefinition
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my @arguments = buildArgumentList($argString);
    my $argList = "";
    for (my $i = 0; $i <= $#arguments; $i++) {
        $argList .= "@arguments[$i]";
        if ($i != $#arguments) {
            $argList .= ", ";
        }
    }
    print "$retval (APIENTRYP Orig$fname)($argList";
    #print ") = $fname;\n";
    print ") = NULL;\n";
    print "/* Forward declaration: */ __declspec(dllexport) $retval APIENTRY Detoured$fname($argList);\n";

    push @initializer, "\tOrig$fname = $fname;\n\tdbgPrint(DBGLVL_DEBUG, \"Orig$fname = 0x%x\\n\", $fname);\n";

    push @attach, "\tdbgPrint(DBGLVL_DEBUG, \"Attaching $fname 0x%x\\n\", (Orig$fname));
    /* _dbg_Verify(\"$fname\", (PBYTE)Orig$fname); */
    retval = DetourAttach(&((PVOID)Orig$fname), Detoured$fname);
    if (retval != NO_ERROR) {
        dbgPrint(DBGLVL_DEBUG, \"DetourAttach($fname) failed: %u\\n\", retval);
        return 0;
    }";
    push @detach, "\tretval = DetourDetach(&((PVOID)Orig$fname), Detoured$fname);
    if (retval != NO_ERROR) {
        dbgPrint(DBGLVL_DEBUG, \"DetourDetach($fname) failed: %u\\n\", retval);
        return 0;
    }";
}

sub createTrampolineDeclaration
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my @arguments = buildArgumentList($argString);
    print "extern DEBUGLIBAPI $retval (APIENTRYP Orig$fname)(";
    for (my $i = 0; $i <= $#arguments; $i++) {
        print "@arguments[$i]";
        if ($i != $#arguments) {
            print ", ";
        }
    }
    print ");\n";
}

if ($#ARGV == 0) {
    $mode = $ARGV[0];
} else {
    die "argument must be decl, def or exp";
}

if ($mode eq "exp") {
    print ";\n; THIS IS A GENERATED FILE!\n;\n\n";
} else {
    print "/*\n * THIS IS A GENERATED FILE!\n */\n\n";
}


if ($mode eq "decl") {
    print "#ifndef __TRAMPOLINES_H\n";
    print "#define __TRAMPOLINES_H\n";
    print "#pragma once\n\n";
    print "/* needed for DebugFunctions to know where the functions are located */\n";
    print "#ifdef DEBUGLIB_EXPORTS\n";
    print "#define DEBUGLIBAPI __declspec(dllexport)\n";
    print "#else /* DEBUGLIB_EXPORTS */\n";
    print "#define DEBUGLIBAPI __declspec(dllimport)\n";
    print "#endif /* DEBUGLIB_EXPORTS */\n\n";
} elsif ($mode eq "exp") {
    print "LIBRARY \"DebugLib\"\n";
    print "EXPORTS\n";
}


$filename = "../GL/gl.h";
my $indefinition = 0;
my $inprototypes = 0;
$extname = "GL_VERSION_1_0";
open(IN, $filename) || die "Couldn’t read $filename: $!";
while (<IN>) {

    # create function pointer type for core hook
    if (/^\s*WINGDIAPI\s+(\S.*\S)\s+(?:GL)?APIENTRY\s+(\S+)\s*\((.*)\)/) {
        if ($mode eq "decl") {
            createTrampolineDeclaration ($1, $2, $3);
        } elsif ($mode eq "def") {
            createTrampolineDefinition ($1, $2, $3);
        } elsif ($mode eq "exp") {
            print "\tOrig$2\n";
        }
    }

    #~ # create function pointer type for extension hook
    #~ if ($indefinition == 1) {
        #~ if (/^#define\s+$extname\s+1/) {
            #~ $inprototypes = 1;
        #~ }
    #~ }
#~
    #~ if ($inprototypes == 1) {
        if (/^\s*(?:GLAPI|extern)\s+(\S.*\S)\s*(?:GL)?APIENTRY\s+(\S+)\s*\((.*)\)/) {
            if ($mode eq "decl") {
                createTrampolineDeclaration ($1, $2, $3);
            } elsif ($mode eq "def") {
                createTrampolineDefinition ($1, $2, $3);
            }  elsif ($mode eq "exp") {
                print "\tOrig$2\n";
            }
        }
    #~ }
#~
    #~ if (/^#endif/ && $inprototypes == 1) {
        #~ $inprototypes = 0;
        #~ $indefinition = 0;
    #~ }
#~
    #~ if (/^#ifndef\s+(GL_\S+)/) {
        #~ $extname = $1;
        #~ $indefinition = 1;
    #~ }
}
close(IN);

$filename = "../GL/WinGDI.h";
my $indefinition = 0;
my $inprototypes = 0;
open(IN, $filename) || die "Couldn’t read $filename: $!";
while (<IN>) {
    if (/^\s*(?:WINGDIAPI|extern)\s+\S.*\S\s*\(.*/) {
        my $fprototype = $_;
        chomp $fprototype;
        while ($fprototype !~ /.*;\s*$/) {
            $line = <IN>;
            chomp $line;
            $line =~ s/\s*/ /;
            $fprototype = $fprototype.$line;
        }
        if ($fprototype =~ /^\s*(?:WINGDIAPI|extern)\s+(\S.*\S)\s+WINAPI\s+(wgl\S+)\s*\((.*)\)\s*;/ > 0) {
            if ($mode eq "decl") {
                createTrampolineDeclaration ($1, $2, $3);
            } elsif ($mode eq "def") {
                createTrampolineDefinition ($1, $2, $3);
            }  elsif ($mode eq "exp") {
                print "\tOrig$2\n";
            }
        }
    }

    if (/^#endif/) {
        if ($inprototypes == 1) {
            $inprototypes = 0;
        } elsif ($indefinition == 1) {
            $indefinition = 0;
            $extname = "WGL_VERSION_1_0";
        }
    }

    if (/^#ifndef\s+(WGL_\S+)/) {
        $extname = $1;
        $indefinition = 1;
    }
}
close(IN);

"BOOL SwapBuffers HDC" =~ /(\S+)\s(\S+)\s(\S+)/;
if ($mode eq "decl") {
    createTrampolineDeclaration ($1, $2, $3);
} elsif ($mode eq "def") {
    createTrampolineDefinition ($1, $2, $3);
}  elsif ($mode eq "exp") {
    print "\tOrig$2\n";
}

$filename = "../GL/glext.h";
my $indefinition = 0;
my $inprototypes = 0;
open(IN, $filename) || die "Couldn’t read $filename: $!";
while (<IN>) {

    # create function pointer type for core hook
    if (/^\s*WINGDIAPI\s+(\S.*\S)\s+(?:GL)?APIENTRY\s+(\S+)\s*\((.*)\)/) {
        if ($mode eq "decl") {
            createTrampolineDeclaration ($1, $2, $3);
        } elsif ($mode eq "def") {
            createExtensionTrampolineDefinition ($1, $2, $3);
        } elsif ($mode eq "exp") {
            print "\tOrig$2\n";
        }
    }

    #~ # create function pointer type for extension hook
    #~ if ($indefinition == 1) {
        #~ if (/^#define\s+$extname\s+1/) {
            #~ $inprototypes = 1;
        #~ }
    #~ }
#~
    #~ if ($inprototypes == 1) {
        if (/^\s*(?:GLAPI\b)(.*?)(?:GL)?APIENTRY\s+(.*?)\s*\((.*?)\)/) {
            if ($mode eq "decl") {
                createTrampolineDeclaration ($1, $2, $3);
            } elsif ($mode eq "def") {
                createExtensionTrampolineDefinition ($1, $2, $3);
            }  elsif ($mode eq "exp") {
                print "\tOrig$2\n";
            }
        }
    #~ }
#~
    #~ if (/^#endif/ && $inprototypes == 1) {
        #~ $inprototypes = 0;
        #~ $indefinition = 0;
    #~ }
#~
    #~ if (/^#ifndef\s+(GL_\S+)/) {
        #~ $extname = $1;
        #~ $indefinition = 1;
    #~ }
}
close(IN);

$filename = "../GL/wglext.h";
my $indefinition = 0;
my $inprototypes = 0;
$extname = "WGL_VERSION_1_0";
open(IN, $filename) || die "Couldn’t read $filename: $!";
while (<IN>) {
    if (/^\s*(?:WINGDIAPI|extern)\s+\S.*\S\s*\(.*/) {
        my $fprototype = $_;
        chomp $fprototype;
        while ($fprototype !~ /.*;\s*$/) {
            $line = <IN>;
            chomp $line;
            $line =~ s/\s*/ /;
            $fprototype = $fprototype.$line;
        }
        if ($fprototype =~ /^\s*(?:WINGDIAPI|extern)\s+(\S.*\S)\s+WINAPI\s+(wgl\S+)\s*\((.*)\)\s*;/ > 0) {
            if ($mode eq "decl") {
                createTrampolineDeclaration ($1, $2, $3);
            } elsif ($mode eq "def") {
                createExtensionTrampolineDefinition ($1, $2, $3);
            } elsif ($mode eq "exp") {
                print "\tOrig$2\n";
            }
        }
    }

    if (/^#endif/) {
        if ($inprototypes == 1) {
            $inprototypes = 0;
        } elsif ($indefinition == 1) {
            $indefinition = 0;
            $extname = "WGL_VERSION_1_0";
        }
    }

    if (/^#ifndef\s+(WGL_\S+)/) {
        $extname = $1;
        $indefinition = 1;
    }
}
close(IN);

if ($mode eq "def") {
    print "\n";
    print "void initTrampolines() {\n";
    print join "\n", @initializer;
    print "\n}\n";

    print "\n";
    print "void initExtensionTrampolines() {\n";
    print join "\n", @extinitializer;
    print "\n}\n";

    print "\n";
    createUtils();
    print "\n";
    print qq|int attachTrampolines() {
    LONG retval = 0;
    initTrampolines();
    if ((retval = DetourTransactionBegin()) != NO_ERROR) {
        dbgPrint(DBGLVL_ERROR, "DetourTransactionBegin failed: %u\\n", retval);
    }
    if ((retval = DetourUpdateThread(GetCurrentThread())) != NO_ERROR) {
        dbgPrint(DBGLVL_ERROR, "DetourUpdateThread failed: %u\\n", retval);
    }\n|;
    print join "\n", @attach;
    print qq|\n\tif ((retval = DetourTransactionCommit()) != NO_ERROR) {
        dbgPrint(DBGLVL_ERROR, "DetourTransactionCommit failed: %u\\n", retval);
    }|;
    print "\n\treturn 1;\n}\n";

    print "\n";
    print qq|int detachTrampolines() {
    LONG retval = 0;
    if ((retval = DetourTransactionBegin()) != NO_ERROR) {
        dbgPrint(DBGLVL_ERROR, "DetourTransactionBegin failed: %u\\n", retval);
    }
    if ((retval = DetourUpdateThread(GetCurrentThread())) != NO_ERROR) {
        dbgPrint(DBGLVL_ERROR, "DetourUpdateThread failed: %u\\n", retval);
    }\n|;
    print join "\n", @detach;
    print qq|\n\tif ((retval = DetourTransactionCommit()) != NO_ERROR) {
        dbgPrint(DBGLVL_ERROR, "DetourTransactionCommit failed: %u\\n", retval);
    }|;
    print "\n\treturn 1;\n}\n";
}

if ($mode eq "decl") {
    print "void initExtensionTrampolines();\n";
    print "int attachTrampolines();\n";
    print "int detachTrampolines();\n";
    print "#endif /* __TRAMPOLINES_H */\n";
}
if ($mode ne "exp") {
    print "\n";
}

