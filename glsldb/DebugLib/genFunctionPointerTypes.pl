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

if ($^O =~ /Win32/) {
    $WIN32 = 1;
}

@defined_types = ();

sub createFPType
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;

    my @arguments = buildArgumentList($argString);
    my $pfname = join("","PFN",uc($fname),"PROC");
    print  "
    typedef $retval (APIENTRYP $pfname)(";
    for (my $i = 0; $i <= $#arguments; $i++) {
        print "@arguments[$i]";
        if ($i != $#arguments) {
            print ", ";
        }
    }
    print ");";
}

sub createFPlowercaseType
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my @arguments = buildArgumentList($argString);
    if(grep(/^$fname$/i, @defined_types)){
        return;
    }

    my $pfname = join("","PFN",$fname,"PROC");
    print  "
    typedef $retval (APIENTRYP $pfname)(";
    for (my $i = 0; $i <= $#arguments; $i++) {
        print "@arguments[$i]";
        if ($i != $#arguments) {
            print ", ";
        }
    }
    print ");";

    push(@defined_types, $fname);
}

foreach my $filename ("../GL/gl.h", "../GL/glext.h") {
    my $indefinition = 0;
    my $inprototypes = 0;
    $extname = "GL_VERSION_1_0";
    open(IN, $filename) || die "Couldn’t read $filename: $!";
    @defined_upper = ();
    while (<IN>) {
        if(/^\s*typedef.*\((?:GL)?APIENTRY\S*\s+PFN(\S+)PROC\)/){
            push(@defined_upper, uc($1));
        }
    }

    seek(IN, 0, SEEK_SET);
    while (<IN>) {

        # create function pointer type for core hook
        if (/^\s*WINGDIAPI\s+(\S.*\S)\s+(?:GL)?APIENTRY\s+(\S+)\s*\((.*)\)/) {
            createFPType($1, $2, $3);
            createFPlowercaseType($1, $2, $3);
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
                my $fn = uc($2);
                if(!grep(/^$fn$/i, @defined_upper)){
                    createFPType($1, $2, $3);
                }
                createFPlowercaseType($1, $2, $3);
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
}

if (defined $WIN32) {
    foreach my $filename ("../GL/WinGDI.h", "../GL/wglext.h") {
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
                    createFPType($1, $2, $3);
                    createFPlowercaseType($1, $2, $3);
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
    }
    "BOOL SwapBuffers HDC" =~ /(\S+)\s(\S+)\s(\S+)/;
    createFPType($1, $2, $3);
    createFPlowercaseType($1, $2, $3);
} else {
    foreach my $filename ("../GL/glx.h", "../GL/glxext.h") {
        my $indefinition = 0;
        my $inprototypes = 0;
        $extname = "GLX_VERSION_1_0";
        open(IN, $filename) || die "Couldn’t read $filename: $!";
        while (<IN>) {
            if (/^\s*(?:GLAPI|extern)\s+\S.*\S\s*\(.*/) {
                # ignore some obscure extensions
                if ($extname eq "GLX_SGIX_dm_buffer" ||
                    $extname eq "GLX_SGIX_video_source") {
                    next;
                }
                my $fprototype = $_;
                chomp $fprototype;
                while ($fprototype !~ /.*;\s*$/) {
                    $line = <IN>;
                    chomp $line;
                    $line =~ s/\s*/ /;
                    $fprototype = $fprototype.$line;
                }
                $fprototype =~ /^\s*(?:GLAPI|extern)\s+(\S.*\S)\s*(glX\S+)\s*\((.*)\)\s*;/;
                if ($extname eq "GLX_VERSION_1_1" || $extname eq "GLX_VERSION_1_0") {
                    createFPType($1, $2, $3);
                }
                # No way to parse functions returning a not "typedef'ed"
                # function pointer in a simple way :-(
                if ($2 eq "glXGetProcAddressARB") {
                    createFPlowercaseType("__GLXextFuncPtr", "glXGetProcAddressARB", "const GLubyte *");
                } else {
                    createFPlowercaseType($1, $2, $3);
                }
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
        close(IN);
    }
}

print "\n";

