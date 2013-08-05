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
# 	list of conditions and the following disclaimer in the documentation and/or
# 	other materials provided with the distribution.
# 
#   * Neither the name of the name of VIS, Universität Stuttgart nor the names
# 	of its contributors may be used to endorse or promote products derived from
# 	this software without specific prior written permission.
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

# edit this list for new extensions that include drawcalls
# second entry is index of parameter that gives the primitiveMode, this is so
# future-proof, I can't believe we've done that ;-)
my @debuggableDrawCalls = (
 ["glBegin", 0],
 #"glBitmap",
 ["glDrawArrays", 0],
 ["glDrawElements", 0],
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
 "glDrawElements",
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
	
	print "#include <stdlib.h>\n";
	print "#include \"debuglib.h\"\n";
	print "GLFunctionList glFunctions[] = {\n";
}


sub createFooter
{
	print "\t{NULL, NULL, NULL, 0, -1, 0, 0}\n};\n";
}

sub createListEntry
{
	my $prefix = shift;
	my $extname = shift;
	my $fname = shift;
	print "\t{\"$prefix\", \"$extname\", \"$fname\", "; 
	print (scalar grep {$fname eq $_->[0]} @debuggableDrawCalls); print ", ";
	@bla = grep {$fname eq $_->[0]} @debuggableDrawCalls;
	if (@bla) {
		print $bla[0]->[1]; print ", ";
	} else {
		print "-1, "
	}
	print (scalar grep {$fname eq $_} @shaderSwitches); print ", ";
	print (scalar grep {$fname eq $_} @frameEndMarkers); print ", ";
	print (scalar grep {$fname eq $_} @framebufferChanges); print "},\n";
}

createHeader();
foreach my $filename ("../GL/gl.h", "../GL/glext.h") {
	my $indefinition = 0;
	my $inprototypes = 0;
	$extname = "GL_VERSION_1_0";
	open(IN, $filename) || die "Couldn’t read $filename: $!";
	while (<IN>) {
		
		# create function pointer type for core hook
		if (/^\s*WINGDIAPI\s+(\S.*\S)\s+APIENTRY\s+(\S+)\s*\((.*)\)/) {
			createListEntry("GL", "GL_VERSION_1_1", $2);
		}

		# create function pointer type for extension hook
		if ($indefinition == 1) {
			if (/^#define\s+$extname\s+1/) {
				$inprototypes = 1;
			}
		}
		
		if ($inprototypes == 1) {
			if (/^\s*(?:GLAPI|extern)\s+(\S.*\S)\s*APIENTRY\s+(\S+)\s*\((.*)\)/) {
				createListEntry("GL", $extname, $2);
			}
		}
		
		if (/^#endif/ && $inprototypes == 1) {
			$inprototypes = 0;
			$indefinition = 0;
		}

		if (/^#ifndef\s+(GL_\S+)/) {
			$extname = $1;
			$indefinition = 1;
		}

	}
}

if (defined $WIN32) {
    foreach my $filename ("../GL/wglext.h", "../GL/WinGDI.h") {
	    my $indefinition = 0;
	    my $inprototypes = 0;
		$extname = "WGL_VERSION_1_0";
	    open(IN, $filename) || die "Couldn’t read $filename: $!";
	    while (<IN>) {
    		
		    if ($indefinition == 1 && /^#define\s+$extname\s+1/) {
				    $inprototypes = 1;
		    }
    		
		    if (/^\s*(?:WINGDIAPI|extern)\s+\S.*\S\s*\(.*/) {
			    my $fprototype = $_;
			    chomp $fprototype;
			    while ($fprototype !~ /.*;\s*$/) {
				    $line = <IN>;
				    chomp $line;
				    $line =~ s/\s*/ /;
				    $fprototype = $fprototype.$line;
			    }
			    if (($fprototype =~ /^\s*(?:WINGDIAPI|extern)\s+(\S.*\S)\s+WINAPI\s+(wgl\S+)\s*\((.*)\)\s*;/) > 0) {
			        createListEntry("WGL", $extname, $2);
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
		    
		    # HAZARD: This relies on the correct order of files, 
		    # i. e. WinGDI being last.
		    if (/^\/\/ OpenGL wgl prototypes/) {
			    $extname = "WGL_VERSION_1_0";
			    $indefinition = 1;
		    }		    
	    }
    }
    createListEntry("WGL", "WGL_VERSION_1_0", "SwapBuffers");
} else {
    foreach my $filename ("../GL/glx.h", "../GL/glxext.h") {
	    my $indefinition = 0;
	    my $inprototypes = 0;
		$extname = "GLX_VERSION_1_0";
	    open(IN, $filename) || die "Couldn’t read $filename: $!";
	    while (<IN>) {
    		
		    if ($indefinition == 1 && /^#define\s+$extname\s+1/) {
				    $inprototypes = 1;
		    }
    		
		    if (/^\s*(?:GLAPI|extern)\s+\S.*\S\s*\(.*/) {
			    my $fprototype = $_;
			    chomp $fprototype;
			    while ($fprototype !~ /.*;\s*$/) {
				    $line = <IN>;
				    chomp $line;
				    $line =~ s/\s*/ /;
				    $fprototype = $fprototype.$line;
			    }
			    $fprototype =~ /^\s*(?:GLAPI|extern)\s+(\S.*\S)\s*(glX\S+)\s*\((.*)\)\s*;/;
			    createListEntry("GLX", $extname, $2);
		    }

		    if (/^#endif/) {
			    if ($inprototypes == 1) {
				    $inprototypes = 0;
			    } elsif ($indefinition == 1) {	
				    $indefinition = 0;
				    $extname = "GLX_VERSION_1_0";
			    }
		    }
    		
		    if (/^#ifndef\s+(GLX_\S+)/) {
			    $extname = $1;
			    $indefinition = 1;
		    }
	    }
    }
}
createFooter();

