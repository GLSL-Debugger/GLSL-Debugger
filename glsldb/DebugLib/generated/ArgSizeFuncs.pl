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


require genTypes;
require genTools;
our %regexps;

use Getopt::Std;
getopts('p');

sub createBody
{
    my ($line, $extname, $retval, $fname, $argString) = (@_);
    my $isExtFunction = $line !~ /WINGDIAPI/;
    my @arguments = buildArgumentList($argString);
    my $pfname = join("","PFN",uc($fname),"PROC");

    if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/) {
        foreach my $argument (@arguments) {
            if ($argument =~ /[*]$/) {
                if ($fname !~ /gl\D+([1234])\D{1,2}v[A-Z]*/ &&
                    $fname !~ /^gl(Gen|Get|Are)/) {
                    print "/* $extname */\n" if not $opt_p;
                    print "int $fname" . "_getArg$i" . "Size($argString)\n";
                    # If full definition is required
                    print "{\n\treturn 1;\n}\n\n" if not $opt_p;
                }
            }
        }
    }
}

my $actions = {
    $regexps{"wingdi"} => \&createBody,
    $regexps{"glapi"} => \&createBody
};


header_generated();

foreach my $filename (@ARGV){
    parse_output($filename, "GL_VERSION_1_0", "GL_", $actions, 1);
}
