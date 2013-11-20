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


require genTools;
our %regexps;

if ($^O =~ /Win32/) {
    $WIN32 = 1;
}

sub createBodyHeader
{
    if (defined $WIN32) {
        print "
__declspec(dllexport) PROC APIENTRY DetouredwglGetProcAddress(LPCSTR arg0) {
    dbgPrint(DBGLVL_DEBUG, \"DetouredwglGetProcAddress(\\\"%s\\\")\\n\", arg0);
        ";

    } else {
        print "
    DBGLIBLOCAL void (*glXGetProcAddressHook(const GLubyte *n))(void)
    {
        void (*result)(void) = NULL;

        /*fprintf(stderr, \"glXGetProcAddressARB(%s)\\n\", (const char*)n);*/

        if (!(strcmp(\"glXGetProcAddressARB\", (char*)n) &&
              strcmp(\"glXGetProcAddress\", (char*)n))) {
            return (void(*)(void))glXGetProcAddressHook;
        }

        ";
    }
}

sub createBodyFooter
{
    if (defined $WIN32) {
        print "\n\treturn NULL;\n}";
    } else {
        print "
        {
            /*fprintf(stderr, \"glXGetProcAddressARB no overload found for %s\\n\", (const char*)n);*/
            /*return ORIG_GL(glXGetProcAddressARB)(n);*/
            return G.origGlXGetProcAddress(n);
        }
        /*fprintf(stderr, \"glXGetProcAddressARB result: %p\\n\", result);*/
        return result;
    }\n";
    }
}

sub createFunctionHook
{
    my $line = shift;
    my $extname = shift;
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;

    if ($fname =~ /MESA$/) {
        return;
    }

    if (defined $WIN32) {
        print "
    if (strcmp(\"$fname\", arg0) == 0) {
        if (Orig$fname == NULL) {
            /* Orig$fname = (PFN${fname}PROC)OrigwglGetProcAddress(\"$fname\"); */
            /* HAZARD BUG OMGWTF This is plain wrong. Use GetCurrentThreadId() */
            DbgRec *rec = getThreadRecord(GetCurrentProcessId());
            rec->isRecursing = 1;
            initExtensionTrampolines();
            rec->isRecursing = 0;
            if (Orig$fname == NULL) {
                dbgPrint(DBGLVL_DEBUG, \"Could not get $fname address\\n\");
            }
        }
        return (PROC) Detoured$fname;
    }
        ";
    } else {
        my $pfname = join("","PFN",uc($fname),"PROC");

        print "
        if (!strcmp(\"$fname\", (char*)n)) {
                return (void(*)(void))$fname;
        }";
    }
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
createBodyHeader();

my $gl_actions = {
        $regexps{"wingdi"} => \&createFunctionHook,
        $regexps{"glapi"} => \&createFunctionHook
}

my $add_actions;
if (defined $WIN32) {
    $add_actions = {$regexps{"winapifunc"} => \&createXFunctionHook}
} else {
    $add_actions = {$regexps{"glxfunc"} => \&createXFunctionHook}
}

parse_gl_files($gl_actions, $add_actions, defined $WIN32,
                \&createFunctionHook);

createBodyFooter();
