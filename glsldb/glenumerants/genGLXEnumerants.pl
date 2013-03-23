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

while (<>) {
	
	if ($indefinition == 1 && /^#define\s+$extname\s+1/) {
			$inprototypes = 1;
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

	# save enumerant definition
	if ($indefinition == 1 && $inprototypes == 0 || 
	    $inprototypes == 0 && $indefinition == 0) {
		if (/^\s*#define\s+(GLX_\w+).*/) {
			push @glxenumerants, $1;
		}
	}
}

# create OpenGL Enumerants map
print "
static struct {
	int value;
	const char *string;
} glxEnumerantsMap[] = {\n";
foreach (@glxenumerants) {
	print  "\t{$_, \"$_\"},\n";
}
print  "\t{0, NULL}\n";
print "};\n";

