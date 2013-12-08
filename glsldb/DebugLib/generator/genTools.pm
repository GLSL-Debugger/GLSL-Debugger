################################################################################
#
# Copyright (c) 2013 SirAnthony <anthony at adsorbtion.org>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
################################################################################

use Time::localtime;
require genSettings;
our %extname_matches;
our %files;
our @skip_defines;


our %regexps = (
    "glapi" => qr/^\s*(?:GLAPI\b)\s+(.*?)(?:GL)?APIENTRY\s+(.*?)\s*\((.*?)\)/,
    "wingdi" => qr/^\s*(?:WINGDIAPI\b)\s+(.*?)(?:GL)?APIENTRY\s+(.*?)\s*\((.*?)\)/,
    "winapifunc" => qr/^\s*(?:WINGDIAPI\b|extern\b)\s+(\S.*\S)\s+WINAPI\s+(wgl\S+)\s*\((.*)\)\s*;/,
    "glxfunc" => qr/^\s*(?:GLAPI\b|extern\b)\s+(\S.*\S)\s*(glX\S+)\s*\((.*)\)\s*;/,
    "typegl" => qr/^\s*typedef\s+(.*?)\s*(GL\w+)\s*;/,
    "typewgl" => qr/^\s*typedef\s+(.*?)\s*(WGL\w+)\s*;/,
    "typeglx" => qr/^\s*typedef\s+(.*?)\s*(GLX\w+)\s*;/,
    "pfn" => qr/^\s*typedef.*\((?:GL)?APIENTRY\S*\s+PFN(\S+)PROC\)/,
    "glvar" => qr/^\s*#define\s+(GL_\w+)\s+0x[0-9A-Fa-f]*/,
    "glxvar" => qr/^\s*#define\s+(GLX_\w+)\s+0x[0-9A-Fa-f]*/,
    "wglvar" => qr/^\s*#define\s+((WGL|ERROR)_\w+)\s+0x[0-9A-Fa-f]*/,
);

my $func_match = qr/^\s*(?:GLAPI\b|WINGDIAPI\b|extern\b)\s+\S.*\S\s*\([^)]*?$/;

# I wanted to make it clear
# Well, shit...
sub parse_output {
    my ($filename, $bextname, $api, $actions) = (@_);
    my ($ifdir, $indef) = (0, 1);
    my $extname = $bextname;
    my @definitions = ($extname);
    my $proto = $extname;
    my $api_re = qr/^#ifndef\s+($api\S+)/;
    my $api_defined = qr/^#ifdef\s+(\S+)/;
    my $skip = 0;

    my $ifh;
    my $is_stdin = 0;
    if ($filename) {
        open $ifh, "<", $filename or die $!;
    } else {
        $ifh = *STDIN;
        $is_stdin++;
    }

    while (<$ifh>) {
        chomp;

        # Skip comments
        next if /^\/\//;

        # Multiline function
        if (/$func_match/) {
            my $fprototype = $_;
            chomp $fprototype;
            while ($fprototype !~ /.*;\s*$/) {
                my $line = <$ifh>;
                chomp $line;
                $line =~ s/\s*/ /;
                $fprototype .= $line;
            }
            $_ = $fprototype;
        }

        # Prototype name in #define or in comment
        my $reg_match = $extname_matches{$_};
        my $extreg = qr/^#define\s+$extname\s+1/;
        if ($reg_match || ($indef == $ifdir && /$extreg/)) {
            if ($reg_match){
                push @definitions, $extname;
                $extname = $reg_match;
            }
            $proto = $extname;
        }

        # Run each supplied regexp here
        if ($indef == $ifdir) {
            while (my ($regexp, $func) = each(%$actions) ) {
                if (my @matches = /$regexp/){
                    $func->($_, $proto, @matches);
                }
            }
        }

        if (/^#endif/ and $ifdir) {
            if ($skip > 0){
                $skip--;
                next;
            }

            if ($ifdir == $indef){
                $indef--;
                $extname = pop @definitions;
                $proto = $bextname;
            }
            $ifdir--;
        }

        if (/^#if/) {
            if (/$api_defined/ and grep { /^$1$/ } @skip_defines){
                $skip++;
                next;
            }

            $ifdir++;
            if (/$api_re/) {
                push @definitions, $extname;
                $extname = $1;
                $indef = $ifdir;
            }
        }
    }
    close $ifh unless $is_stdin;
}


sub header_generated {
    my $t = localtime;
    my $year = $t->year + 1900;
    printf "////////////////////////////////////////////////////////
//
//   THIS FILE IS GENERATED AUTOMATICALLY %02d.%02d.%04d %02d:%02d:%02d
//
// Copyright (c) %04d Perl generator
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the \"Software\"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
////////////////////////////////////////////////////////

", $t->mday, $t->mon + 1, $year, $t->hour, $t->min, $t->sec, $year;
}


sub parse_gl_files {
    my $gl_actions = shift;
    my $add_actions = shift;
    my $WIN32 = shift;
    my $win32func = shift;
    my @params = ([$files{"gl"}, "GL_VERSION_1_0", "GL_", $gl_actions]);

    if (ref($add_actions) eq "HASH") {
        if ($WIN32) {
            push @params, [$files{"wgl"}, "WGL_VERSION_1_0",
                            "WGL_", $add_actions];

            # Additional function from original file
            $win32func->(0, "WGL_VERSION_1_0", "BOOL", "SwapBuffers",
                            "HDC") if $win32func;
        } else {
            push @params, [$files{"glx"}, "GLX_VERSION_1_0",
                            "GLX_", $add_actions];
        }
    }

    foreach my $entry (@params) {
        my $filenames = shift @$entry;
        foreach my $filename (@$filenames) {
            parse_output($filename, @$entry);
        }
    }
}

