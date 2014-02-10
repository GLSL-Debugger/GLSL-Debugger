################################################################################
#
# Copyright (c) 2014 SirAnthony <anthony at adsorbtion.org>
# Copyright (C) 2006-2009 Institute for Visualization and Interactive Systems
# (VIS), Universit?t Stuttgart.
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
#   * Neither the name of the name of VIS, Universitдt Stuttgart nor the names
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
our %files;
our %regexps;


# TODO: possibly bullshit, need to check for WINGDIAPI/extern stuff

my @initializer = ();
my @extinitializer = ();
my @functions = ();
my %trampoline_generated = ();

sub defines {
	my $mode = shift;
	if ($mode eq "decl") {
		print "#ifndef __TRAMPOLINES_H
#define __TRAMPOLINES_H
#pragma once

/* needed for DebugFunctions to know where the functions are located */
#ifdef glsldebug_EXPORTS
#define DEBUGLIBAPI __declspec(dllexport)
#else /* glsldebug_EXPORTS */
#define DEBUGLIBAPI __declspec(dllimport)
#endif /* glsldebug_EXPORTS */

";
	} elsif ($mode eq "exp") {
		print "LIBRARY \"glsldebug\"
EXPORTS
";
	}
}

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
		pbEnd = (PBYTE)DetourCopyInstruction(NULL, NULL, (PVOID)pbSrc, (PVOID*)(&pbTarget), NULL);
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

sub footer {
	my $mode = shift;
	if ($mode eq "def") {
		my $count = scalar @functions;
		my $orig, $hooked, $names;
		my $iter = 0;
		foreach (@functions) {
			my $newline = ($iter++ % 10) ? "" : "\n";
			$orig .= $newline . " &((PVOID)Orig$_),";
			$hooked .= $newline . " Hooked$_,";
			$names .= $newline . " \"$_\",";
		}

		printf "
#define TRMP_FUNCS_COUNT $count
PVOID* trmp_OrigFuncs[TRMP_FUNCS_COUNT] = {$orig
};
PVOID trmp_HookedFuncs[TRMP_FUNCS_COUNT] = {$hooked
};
const char* trmp_FuncsNames[TRMP_FUNCS_COUNT] = {$names
};

void initTrampolines() {
%s
}

void initExtensionTrampolines() {
%s
}
", join("\n", @initializer), join("\n", @extinitializer);

		print qq|
int attachTrampolines() {
	int i;
	initTrampolines();
	initExtensionTrampolines();
	for (i = 0; i < TRMP_FUNCS_COUNT; ++i) {
		dbgPrint(DBGLVL_DEBUG, "Attaching %s 0x%x\\n", trmp_FuncsNames[i], trmp_OrigFuncs[i]);
		/* _dbg_Verify(trmp_FuncsNames[i], (PBYTE)trmp_OrigFuncs[i]); */
		if (!Mhook_SetHook(trmp_OrigFuncs[i], trmp_HookedFuncs[i])) {
			dbgPrint(DBGLVL_DEBUG, "Mhook_SetHook(%s) failed.\\n", trmp_FuncsNames[i]);
			return 0;
		}
	}
	return 1;
}

int detachTrampolines() {
	int i;
	for (i = 0; i < TRMP_FUNCS_COUNT; ++i) {
		if (!Mhook_Unhook(&((PVOID)trmp_OrigFuncs[i]))) {
			dbgPrint(DBGLVL_DEBUG, "Mhook_Unhook(%s) failed.\\n", trmp_FuncsNames[i]);
			return 0;
		}
	}
	return 1;
}

|;
	} elsif ($mode eq "decl") {
		print "void initExtensionTrampolines();
int attachTrampolines();
int detachTrampolines();
#endif /* __TRAMPOLINES_H */
";
	}

	print "\n";
}


sub createTrampoline
{
	my ($isExtension, $mode, $extname, $retval, $fname, $argString) = @_;
	return "" if $trampoline_generated{$fname} or $fname eq "wglGetProcAddress";

	my @arguments = buildArgumentList($argString);
	my $argList = join(", ", @arguments);
	my $ret = "    Orig$fname";
	$trampoline_generated{$fname} = 1;

	if ($mode eq "def") {
		$ret = "$retval (APIENTRYP Orig$fname)($argList) = NULL;
/* Forward declaration: */ __declspec(dllexport) $retval APIENTRY Hooked$fname($argList);";

		if (!$isExtension){
			push @initializer, "    Orig$fname = $fname;
	dbgPrint(DBGLVL_DEBUG, \"Orig$fname = 0x%x\\n\", $fname);";
		} else {
			push @extinitializer, "    Orig$fname = ($retval (APIENTRYP)($argList)) wglGetProcAddress(\"$fname\");";
		}
		push @functions, $fname;
	} elsif ($mode eq "decl") {
		$ret = sprintf "extern DEBUGLIBAPI $retval (APIENTRYP Orig$fname)($argList);";
	}

	return $ret;
}


my @modes = ("decl", "def", "exp");
$mode = $ARGV[0];
if (not grep(/^$mode$/, @modes)) {
	die "Argument must be one of " . join(", ", @modes) . "\n";
}


# Setup parser
sub gl_trampoline
{
	my $line = shift;
	my $isExtension = $line !~ /WINGDIAPI/;
	print createTrampoline($isExtension, $mode, @_) . "\n";
}

my $gl_actions = {
	$regexps{"glapi"} => \&gl_trampoline
};

my $win_actions = {
	$regexps{"wingdi"} => \&gl_trampoline,
	$regexps{"winapifunc"} => \&gl_trampoline,
};

my @params = ([$files{"gl"}, "GL_VERSION_1_0", "GL_", $gl_actions],
			  [$files{"wgl"}, "WGL_VERSION_1_0", "WGL_", $win_actions]);


# Begin output
header_generated($mode eq "exp" ? ";" : "//");
defines($mode);

# This windows-specific call is everywhere
gl_trampoline(0, "WGL_VERSION_1_0", "BOOL", "SwapBuffers", "HDC");
foreach my $entry (@params) {
	my $filenames = shift @$entry;
	foreach my $filename (@$filenames) {
		parse_output($filename, @$entry);
	}
}

footer($mode);
