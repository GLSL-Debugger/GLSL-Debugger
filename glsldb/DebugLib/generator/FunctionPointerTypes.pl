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

require genTypes;
require genTools;
our %regexps;


if ($^O =~ /Win32/) {
    $WIN32 = 1;
}

@defined_types = ();

sub print_type
{
    my $retval = shift;
    my $pfname = shift;
    printf  "\n\ttypedef $retval (APIENTRYP $pfname)(%s);",
        join(", ", map { $_ } @_);
}

sub createFPType
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;

    $retval =~ s/^\s+|\s+$//g;
    my @arguments = buildArgumentList($argString);
    my $pfname = sprintf "PFN%sPROC", uc($fname);
    print_type($retval, $pfname, @arguments);
}

sub createFPlowercaseType
{
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    return if grep { /^$fname$/i } @defined_types;

    $retval =~ s/^\s+|\s+$//g;
    my @arguments = buildArgumentList($argString);
    my $pfname = "PFN${fname}PROC";
    print_type($retval, $pfname, @arguments);
    push(@defined_types, $fname);
}


sub add_definition
{
    my $line = shift;
    my $extname = shift;
    my $fname = shift;
    push(@defined_upper, uc($fname));
}

sub create_func
{
    my $line = shift;
    my $extname = shift;
    my $fn = uc($_[1]);
    if(!grep(/^$fn$/i, @defined_upper)){
        createFPType(@_);
    }
    createFPlowercaseType(@_);
}

sub create_func_glx
{
    my $line = shift;
    my $extname = shift;
    if ($extname eq "GLX_VERSION_1_1" || $extname eq "GLX_VERSION_1_0") {
        createFPType(@_) if !grep { /^$fn$/i } @defined_upper;
    }

    # No way to parse functions returning a not "typedef'ed"
    # function pointer in a simple way :-(
    if ($_[2] eq "glXGetProcAddressARB") {
        createFPlowercaseType("__GLXextFuncPtr", "glXGetProcAddressARB",
                              "const GLubyte *");
    } else {
        createFPlowercaseType($_);
    }
}

my $gl_actions = {
    $regexps{"wingdi"} => \&create_func,
    $regexps{"glapi"} => \&create_func
};

my $add_actions;
if (defined $WIN32) {
    $add_actions = {$regexps{"winapifunc"} => \&create_func}
} else {
    $add_actions = {$regexps{"glxfunc"} => \&create_func_glx}
};


# Add PFN definitions first
parse_gl_files( { $regexps{"pfn"} => \&add_definition } );
# Then add absent definitions
parse_gl_files($gl_actions, $add_actions, defined $WIN32, \&create_func);

print "\n";

