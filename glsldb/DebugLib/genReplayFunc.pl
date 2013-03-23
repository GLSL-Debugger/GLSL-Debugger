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
require "justCopyPointersList.pl";

sub createBodyHeader
{
	print "#include <stdio.h>\n";
	print "#include <string.h>\n";
	print "#ifdef _WIN32\n";
	print "#include <windows.h>\n";
	print "#endif /* _WIN32 */\n";
	print "#include \"../GL/gl.h\"\n";
	print "#include \"../GL/glext.h\"\n";
	print "#ifndef _WIN32\n";
	print "#include \"../GL/glx.h\"\n";
	print "#include \"../GL/glxext.h\"\n";
	print "#else /* _WIN32 */\n";
	print "#include \"../GL/wglext.h\"\n";
	print "#include \"trampolines.h\"\n";
	print "#endif /* _WIN32 */\n";
	print "#include \"debuglibInternal.h\"\n";
	print "#include \"streamRecording.h\"\n";
	print "#include \"replayFunction.h\"\n\n";
	print "void replayFunctionCall(StoredCall *f, int final)\n{\n";
}

sub createBodyFooter
{
	print "\t{\n\t\tfprintf(stderr, \"Cannot replay %s: unknown function\\n\", f->fname);\n\t}\n}\n";
}

sub createFunctionHook
{
	my $retval = shift;
	my $fname = shift;
	my $argString = shift;
	my @arguments = buildArgumentList($argString);
	my $ucfname = uc($fname);
	print "#if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_REPLAY || DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL\n";
	print "\tif (!strcmp(\"$fname\", (char*)f->fname)) {\n";
	print "#if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL\n";
	print "\t\tif (final) {\n";
	print "#endif\n";
	print "\t\t\tORIG_GL($fname)(";
	if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/) {
		for (my $i = 0; $i <= $#arguments; $i++) {
			if (scalar grep {$fname eq $_} @justCopyPointersList) {
				print "*(@arguments[$i] *)f->arguments[$i]";
			} elsif (@arguments[$i] =~ /[*]$/) {
				print "(@arguments[$i])f->arguments[$i]";
			} else {
				print "*(@arguments[$i] *)f->arguments[$i]";
			}
			if ($i != $#arguments) {
				print ", ";
			}
		}
	}
	print ");\n";
	print "#if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL\n";
	print "\t\t}\n";
	print "#endif\n";
	#print "\t} else\n";
	print "\t\treturn;\n\t}\n";
	print "#endif\n";
}

createBodyHeader();

foreach my $filename ("../GL/gl.h", "../GL/glext.h") {
	my $indefinition = 0;
	my $inprototypes = 0;
	$extname = "GL_VERSION_1_0";
	open(IN, $filename) || die "Couldn’t read $filename: $!";
	while (<IN>) {
		
		if (/^\s*WINGDIAPI\s+(\S.*\S)\s+APIENTRY\s+(\S+)\s*\((.*)\)/) {
			createFunctionHook($1, $2, $3);
		}

		if ($indefinition == 1) {
			if (/^#define\s+$extname\s+1/) {
				$inprototypes = 1;
			}
		}
		
		if ($inprototypes == 1) {
			if (/^\s*(?:GLAPI|extern)\s+(\S+)\s+APIENTRY\s+(\S+)\s*\((.*)\)/) {
				createFunctionHook($1, $2, $3);
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
	close(IN);
}
createBodyFooter();

