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

require genTools;

# edit this list for new extensions that include drawcalls
# second entry is index of parameter that gives the primitiveMode, this is so
# future-proof, I can't believe we've done that ;-)
my @debuggableDrawCalls = (
 ["glBegin", 0],
 #"glBitmap",
 ["glDrawArrays", 0],
 ["glDrawArraysInstanced", 0],
 ["glDrawElements", 0],
 ["glDrawElementsInstanced", 0],
 #"glDrawPixels",
 ["glDrawRangeElements", 0],
 ["glMultiDrawArrays", 0],
 ["glMultiDrawElements", 0],
 ["glDrawArraysEXT", 0],
 ["glDrawRangeElementsEXT", 0],
 ["glMultiDrawArraysEXT", 0],
 ["glMultiDrawElementsEXT", 0],
 ["glMultiModeDrawArraysIBM", 0],
 ["glMultiModeDrawElementsIBM", 0],
 ["glDrawElementArrayATI", 0],
 ["glDrawRangeElementArrayATI", 0],
 ["glDrawMeshArraysSUN", 0],
 ["glDrawElementArrayAPPLE", 0],
 ["glDrawRangeElementArrayAPPLE", 0],
 ["glMultiDrawElementArrayAPPLE", 0],
 ["glMultiDrawRangeElementArrayAPPLE", 0],
 ["glDrawArraysInstancedEXT", 0],
 ["glDrawElementsInstancedEXT", 0]
 #"glCallList",
 #"glCallLists"
);

my @framebufferChanges = (
 "glEnd",
 "glBitmap",
 "glDrawArrays",
 "glDrawArraysInstanced",
 "glDrawElements",
 "glDrawElementsInstanced",
 "glDrawPixels",
 "glDrawRangeElements",
 "glMultiDrawArrays",
 "glMultiDrawElements",
 "glDrawArraysEXT",
 "glDrawRangeElementsEXT",
 "glMultiDrawArraysEXT",
 "glMultiDrawElementsEXT",
 "glMultiModeDrawArraysIBM",
 "glMultiModeDrawElementsIBM",
 "glDrawElementArrayATI",
 "glDrawRangeElementArrayATI",
 "glDrawMeshArraysSUN",
 "glDrawElementArrayAPPLE",
 "glDrawRangeElementArrayAPPLE",
 "glMultiDrawElementArrayAPPLE",
 "glMultiDrawRangeElementArrayAPPLE",
 "glDrawArraysInstancedEXT",
 "glDrawElementsInstancedEXT",
 "glCallList",
 "glCallLists",
 "glClear",
 "glCopyPixels",
 "glBlitFramebufferEXT",
 "glXSwapBuffers",
 "SwapBuffers"
);

my @shaderSwitches = (
    "glUseProgram",
    "glUseProgramObjectARB",
    "glLinkProgram",
    "glLinkProgramObjectARB"
);

my @frameEndMarkers = (
    "glXSwapBuffers",
    "SwapBuffers"
);

if ($^O =~ /Win32/) {
    $WIN32 = 1;
}

sub createHeader
{
    #print "#include <stdlib.h>\n";
    #print "struct {\n";
    #print "\tconst char *prefix;\n";
    #print "\tconst char *extname;\n";
    #print "\tconst char *fname;\n";
    #print "\tint isDrawDebuggableCall;\n";
    #print "\tint primitiveModeIndex;\n";
    #print "\tint isShaderSwitch;\n";
    #print "\tint isFrameEnd;\n";
    #print "} glFunctions[] = {\n";

    print '#include <stdlib.h>
#include "debuglib.h"
GLFunctionList glFunctions[] = {
';
}


sub createFooter
{
    print "  {NULL, NULL, NULL, 0, -1, 0, 0}
};
";
}

sub createListEntry
{
    my $prefix = shift;
    my $extname = shift;
    my $fname = shift;
    my @abla = grep {$fname eq $_->[0]} @debuggableDrawCalls;
    my $bla = @abla ? $abla[0]->[1] : -1;

    printf "  {\"$prefix\", \"$extname\", \"$fname\", %s, $bla, %s, %s, %s},
", (scalar grep {$fname eq $_->[0]} @debuggableDrawCalls),
   (scalar grep {$fname eq $_} @shaderSwitches),
   (scalar grep {$fname eq $_} @frameEndMarkers),
   (scalar grep {$fname eq $_} @framebufferChanges);
}


sub gl_entry
{
    my ($isExtension, $extname, $retval, $funcname) = @_;
    createListEntry("GL", $extname, $funcname);
}

sub wgl_entry
{
    my ($isExtension, $extname, $retval, $funcname) = @_;
    createListEntry("WGL", $extname, $funcname);
}

sub glx_entry
{
    my ($isExtension, $extname, $retval, $funcname) = @_;
    createListEntry("GLX", $extname, $funcname);
}


my $gl_actions = {    
    $regexps{"glapi"} => \&gl_entry,
};

my $add_actions;
if (defined $WIN32) {
    $add_actions = {
        $regexps{"wingdi"} => \&wgl_entry,
        $regexps{"winapifunc"} => \&wgl_entry
    }
} else {
    $add_actions = { $regexps{"glxfunc"} => \&glx_entry }
}

header_generated();
createHeader();
parse_gl_files($gl_actions, $add_actions, defined $WIN32, \&wgl_entry);
createFooter();

