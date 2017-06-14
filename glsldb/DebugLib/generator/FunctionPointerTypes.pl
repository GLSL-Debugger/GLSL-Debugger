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

use FindBin;
use lib "$FindBin::Bin";

require genTypes;
require genTools;
our %regexps;
our %files;


if ($^O =~ /Win32/) {
    $WIN32 = 1;
}

my %defined_types = ();
my %defined_upper = ();


# This extensions was enabled in one file (gl.h, for example) but
# another file contains #ifndef for it, which ignored by preprocessor
# but not this parser
my @skipped_extnames = (
    "GL_VERSION_1_1",
    "GL_VERSION_1_2",
    "GL_VERSION_1_3",
    "GL_ARB_imaging",
);


sub print_type
{
    my $retval = shift;
    my $pfname = shift;
    printf  "\ntypedef $retval (APIENTRYP $pfname)(%s);",
        join(", ", map { $_ } @_);
}

sub createFPType
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my $ufname = uc($fname);
    return if $defined_upper{$ufname};

    $retval =~ s/^\s+|\s+$//g;
    my @arguments = buildArgumentList($argString);
    print_type($retval, "PFN${ufname}PROC", @arguments);
    $defined_upper{$ufname} = 1;
}

sub createFPlowercaseType
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    return if $defined_types{$fname};

    $retval =~ s/^\s+|\s+$//g;
    my @arguments = buildArgumentList($argString);
    print_type($retval, "PFN${fname}PROC", @arguments);
    $defined_types{$fname} = 1;
}


sub add_definition
{
    my $isExtension = shift;
    my $extname = shift;
    my $fname = shift;
    $defined_upper{uc($fname)} = 1;
}

sub add_definition_check
{
    my $line = shift;
    my $extname = shift;
    my $fname = shift;
    if (not grep { /^$extname$/ } @skipped_extnames) {
        $defined_upper{uc($fname)} = 1;
    }
}

sub create_func
{
    my $isExtension = shift;
    my $extname = shift;
    createFPType(@_);
    createFPlowercaseType(@_);
}

sub create_func_glx
{
    my $isExtension = shift;
    my $extname = shift;
    if ($extname eq "GLX_VERSION_1_1" || $extname eq "GLX_VERSION_1_0") {
        createFPType(@_);
    }

    # No way to parse functions returning a not "typedef'ed"
    # function pointer in a simple way :-(
    if ($_[2] eq "glXGetProcAddressARB") {
        createFPlowercaseType("__GLXextFuncPtr", "glXGetProcAddressARB",
                              "const GLubyte *");
    } else {
        createFPlowercaseType(@_);
    }
}

my $gl_actions = {    
    $regexps{"glapi"} => \&create_func
};

my $add_actions;
if (defined $WIN32) {
    $add_actions = {
        $regexps{"winapifunc"} => \&create_func,
        $regexps{"wingdi"} => \&create_func,
    }
} else {
    $add_actions = {$regexps{"glxfunc"} => \&create_func_glx}
};

header_generated();
# Add PFN definitions first
my $defines = {$regexps{"pfn"} => \&add_definition};
my $defines_check = {$regexps{"pfn"} => \&add_definition_check};
my @params = ([$files{"gl"}[0], "GL_VERSION_1_0", "GL_", $defines],
              [$files{"gl"}[1], "GL_VERSION_1_0", "GL_", $defines_check]);
foreach my $entry (@params) {
    parse_output(@$entry, 1);
}


# Then add absent definitions
parse_gl_files($gl_actions, $add_actions, defined $WIN32, \&create_func);

print "\n";

