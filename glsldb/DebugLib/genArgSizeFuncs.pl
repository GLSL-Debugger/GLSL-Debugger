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

require "argumentListTools.pl";

sub createBody
{
	my $retval = shift;
	my $fname = shift;
	my $argString = shift;
	my $isExtFunction = shift;
	my @arguments = buildArgumentList($argString);
	my $pfname = join("","PFN",uc($fname),"PROC");

	if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/) {
		for (my $i = 0; $i <= $#arguments; $i++) {
			if (@arguments[$i] =~ /[*]$/) {
				if ($fname !~ /gl\D+([1234])\D{1,2}v[A-Z]*/ && 
				    $fname !~ /^glGen/ &&
				    $fname !~ /^glGet/ &&
				    $fname !~ /^glAre/) {
					print "/* $extname */\n";
					print "int $fname";
					print "_getArg$i";
					print "Size($argString)\n";
					print "{\n";
					print "\treturn 1;\n";
					print "}\n\n";
				}
			}
		}
	}
}

$extname = "GL_VERSION_1_0";

while (<>) {
	
	# create core hook
	if (/^\s*WINGDIAPI\s+(\S.*\S)\s+APIENTRY\s+(\S+)\s*\((.*)\)/) {
		createBody($1, $2, $3, 0);
	}

	# create extension hook
	if ($indefinition == 1) {
		if (/^#define\s+$extname\s+1/) {
			$inprototypes = 1;
		}
	}
	
	if ($inprototypes == 1) {
		if (/^\s*(?:GLAPI|extern)\s+(\S.*\S)\s*APIENTRY\s+(\S+)\s*\((.*)\)/) {
			createBody($1, $2, $3, 1);
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

