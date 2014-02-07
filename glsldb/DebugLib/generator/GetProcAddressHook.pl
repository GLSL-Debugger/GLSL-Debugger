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


require genTools;
our %regexps;

my @functions;


if ($^O =~ /Win32/) {
    $WIN32 = 1;
}

sub createBody
{
	my ($orig, $hooked, $names);
	my $count = scalar @functions;
	my $iter = 0;
	foreach (@functions) {
		my $newline = ($iter++ % 10) ? "" : "\n";
		$orig .= $newline . " &((PVOID)Orig$_),";
		$hooked .= $newline . " Hooked$_,";
		$names .= $newline . " \"$_\",";
	}
	
	print "#define GPA_FUNCS_COUNT $count
PVOID* gpa_OrigFuncs[GPA_FUNCS_COUNT] = {$orig
};
PVOID gpa_HookedFuncs[GPA_FUNCS_COUNT] = {$hooked
};
const char* gpa_FuncsNames[GPA_FUNCS_COUNT] = {$names
};

";

    if (defined $WIN32) {
        print qq|
__declspec(dllexport) PROC APIENTRY HookedwglGetProcAddress(LPCSTR arg0) {
	int i;
	dbgPrint(DBGLVL_DEBUG, "HookedwglGetProcAddress(\\"%s\\")\\n", arg0);
	for (i = 0; i < GPA_FUNCS_COUNT; ++i) {
		if (!strcmp(gpa_FuncsNames[i], arg0)) {
			if (*gpa_OrigFuncs[i] == NULL) {
				/* *gpa_OrigFuncs[i] = (PFN${fname}PROC)OrigwglGetProcAddress(gpa_FuncsNames[i]); */
				/* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
				DbgRec *rec = getThreadRecord(GetCurrentProcessId());
				rec->isRecursing = 1;
				initExtensionTrampolines();
				rec->isRecursing = 0;
				if (*gpa_OrigFuncs[i] == NULL) {
					dbgPrint(DBGLVL_DEBUG, \"Could not get %s address\\n\", gpa_FuncsNames[i]);
				}
			}
			return (PROC) gpa_HookedFuncs[i];
		}
	}
	return NULL;
}
|;
    } else {
		my $pfname = join("","PFN",uc($fname),"PROC");
        print qq|
DBGLIBLOCAL void (*glXGetProcAddressHook(const GLubyte *arg0))(void)
{
	/* void (*result)(void) = NULL; */

	/*fprintf(stderr, \"glXGetProcAddressARB(%s)\\n\", (const char*)arg0);*/

	if (!(strcmp("glXGetProcAddressARB", (char*)arg0) &&
		  strcmp(\"glXGetProcAddress\", (char*)arg0))) {
		return (void(*)(void))glXGetProcAddressHook;
	}
	
	for (i = 0; i < GPA_FUNCS_COUNT; ++i) {
		if (!strcmp(gpa_FuncsNames[i], arg0)) 
			return (void(*)(void))(*gpa_OrigFuncs[i]);
	}
	
	{
		/*fprintf(stderr, "glXGetProcAddressARB no overload found for %s\\n", (const char*)arg0); */
		/*return ORIG_GL(glXGetProcAddressARB)(arg0);*/
		return G.origGlXGetProcAddress(arg0);
	}
	/*fprintf(stderr, \"glXGetProcAddressARB result: %p\\n\", result);*/
	/* return result; */
}
|;
    }

}

sub createFunctionHook
{
    my ($line, $extname, $retval, $fname, $argString) = @_;
    return if $fname eq "wglGetProcAddress";
	push @functions, $fname;
}

sub createXFunctionHook {
    my $fname = $_[3];
    if ($fname eq "glXGetProcAddressARB") {
        createFunctionHook($_[0], $_[1], "__GLXextFuncPtr",
                            "glXGetProcAddressARB", "const GLubyte *");
    } else {
        createFunctionHook(@_);
    }
}

header_generated();

my $gl_actions = {
        $regexps{"glapi"} => \&createFunctionHook
};

my $add_actions;
if (defined $WIN32) {
    $add_actions = {
        $regexps{"wingdi"} => \&createFunctionHook,
        $regexps{"winapifunc"} => \&createXFunctionHook
    }
} else {
    $add_actions = {$regexps{"glxfunc"} => \&createXFunctionHook}
}

parse_gl_files($gl_actions, $add_actions, defined $WIN32,
                \&createFunctionHook);

createBody();
