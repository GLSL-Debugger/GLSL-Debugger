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

if ($^O =~ /Win32/) {
	$WIN32 = 1;
}

# use only basic C types and size qualifiers, struct, or * (i.e pointer)
my %typeMap = (
	# GL
	"ptrdiff_t" => "long int", # FIXME: 32/64bit issue
	# GLX
	"Pixmap" => "unsigned long int", # FIXME: 32/64bit issue
	"Colormap" => "unsigned long int", # FIXME: 32/64bit issue
	"Window" => "unsigned long int", # FIXME: 32/64bit issue
	"int32_t" => "int", # FIXME: 32/64bit issue
	"int64_t" => "long", # FIXME: 32/64bit issue
	"Bool" => "int",
	"Font" => "unsigned long int", # FIXME: 32/64bit issue
	"XID" => "unsigned long int", # FIXME: 32/64bit issue
	"Status" => "int",
	"VLServer" => "void *",
	"VLPath" => "int",
	"VLNode" => "int",
	"DMbuffer" => "void *",
	"DMparams" => "struct {int a;}",
	"__GLXextFuncPtr" => "void *"
);

sub addTypeMapping
{
	my $oldtype = shift;
	my $newtype = shift;
	$oldtype =~ s/\s+/ /g;
	$newtype =~ s/\s+/ /g;
	while (my $prevType = $typeMap{$oldtype}) {
		$oldtype = $prevType;
	}
	#print "addTypeMapping: $newtype -> $oldtype\n";
	$typeMap{$newtype} = $oldtype;
}

sub getDummyValue
{
	my $tdarg = shift;
	my $stdarg = stripStorageQualifiers($tdarg);
	my $arg;
	if ($typeMap{$stdarg}) {
		$arg =  $typeMap{$stdarg}
	} else {
		$arg = $tdarg;
	}
	if ($arg =~ /[*]|[\[]/) {
		return "($arg)(void *)dirtyHack";
	} elsif ($arg =~ /struct|union/) {
		return "*($arg *)(void *)dirtyHack";
	} elsif ($arg =~ /float/) {
		return "1.0f";
	} elsif ($arg =~ /double/) {
		return "1.0";
	} elsif ($arg =~ /enum|char|short|int|long|unsigned|signed/) {
		return "1";
	} elsif ($arg =~ /void/) {
		return "";
	} else {
		die "getBasicTypeId: Cannot determine argument type of \"$arg\"\n";	
	}
}


sub createBody
{
	my $retval = shift;
	my $fname = shift;
	my $argString = shift;
	my $isExtension = shift;
	my @arguments = buildArgumentList($argString);
	my $pfname = join("","PFN",uc($fname),"PROC");

	print "\t{\n";
	if (defined $WIN32) {
		if ($isExtension) {
			print "\t$pfname func = ($pfname)wglGetProcAddress((const GLubyte *)\"$fname\");\n";
		} else {
			print "\t$pfname func = $fname;\n";
		}
	} else {
		print "\t$pfname func = ($pfname)glXGetProcAddressARB((const GLubyte *)\"$fname\");\n";
	}
	print "\t\tif (func) {\n";
    print "#ifdef _WIN32\n";
    print "\t\t\t/* This is an evil hack to catch an access violation in Nvidia's\n";
    print "\t\t\t * Windows driver. */\n";
    print "\t\t\t__try {\n";
	print "\t\t\t\tglBegin(GL_POINTS);\n";
	print "#else /* _WIN32 */\n";
	print "\t\t\tif (!sigsetjmp(check_env, 1)) {\n";
	print "\t\t\t\tglBegin(GL_POINTS);\n";
	print "\t\t\t\tcurrentFname = \"$fname\";\n";
	print "\t\t\t\tsignal(SIGSEGV, catch_segfault);\n";
    print "#endif /* _WIN32 */\n";
	print "\t\t\t\tfunc(";
	# add arguments to function head
	my $i = 0;
	foreach (@arguments) {
		print getDummyValue($_);
		if ($i != $#arguments) {
			print ", ";
		}
		$i++;
	}
	print ");\n";
	print "#ifndef _WIN32\n";
	print "\t\t\t\tcurrentFname = \"WTF\";\n";
	print "\t\t\t\tsignal(SIGSEGV, SIG_DFL);\n";
	print "#endif /* !_WIN32 */\n";
	print "\t\t\t\tglEnd();\n";
	print "\t\t\t\tif (glGetError() != GL_INVALID_OPERATION) {\n";
	print "\t\t\t\t\tprintf(\"$fname,\\n\");\n";
	print "\t\t\t\t}\n";
    print "#ifdef _WIN32\n";
    print "\t\t\t} __except(dirtyFilter(GetExceptionCode(), GetExceptionInformation())) {\n";
    print "\t\t\t\tprintf(\"# ACCESS VIOLATION WHEN CALLING \\\"$fname\\\"\\n\");\n";
    print "\t\t\t}\n";
	print "#else /* _WIN32 */\n";
	print "\t\t\t}\n";
    print "#endif /* _WIN32 */\n";
	print "\t\t}\n";
	print "\t}\n";
}

print "#ifdef _WIN32\n";
print "#include <windows.h>\n";
print "#include <excpt.h>\n";
print "#else /* _WIN32 */\n";
print "#include <signal.h>\n";
print "#include <sys/types.h>\n";
print "#include <setjmp.h>\n";
print "#endif /* _WIN32 */\n";
print "#include <stdio.h>\n";
print "#include <stdlib.h>\n";
print "#include <string.h>\n";
print "#include \"../GL/gl.h\"\n";
print "#include \"../GL/glext.h\"\n";
print "#include <GL/glut.h>\n";
print "#include \"debuglibInternal.h\"\n";

print "\n\n";
print "#ifdef _WIN32\n";
print "int dirtyFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {\n";
print "\tif (code == EXCEPTION_ACCESS_VIOLATION) {\n";
print "\t\treturn EXCEPTION_EXECUTE_HANDLER;\n";
print "\t} else {\n";
print "\t\treturn EXCEPTION_CONTINUE_SEARCH;\n";
print "\t}\n";
print "}\n";
print "#else /* _WIN32 */\n";
print "const char *currentFname = NULL;\n";
print "static jmp_buf check_env;\n";
print "void catch_segfault(int sig_num) {\n";
print "\tprintf(\"# ACCESS VIOLATION WHEN CALLING \\\"%s\\\"\\n\", currentFname);\n";
print "\tfflush(stdout);\n";
print "\tsiglongjmp(check_env, sig_num);\n";
print "}\n\n";
#SIGILL,SIGBUS
print "#endif /* _WIN32 */\n\n";

print "void testFunc(void) {\n";
print "\tint dirtyHack[4096];\n\n";
print "\tmemset(dirtyHack, 0, 4096);\n\n";

foreach my $filename ("../GL/gl.h", "../GL/glext.h") {
	my $indefinition = 0;
	my $inprototypes = 0;
	$extname = "GL_VERSION_1_0";
	open(IN, $filename) || die "Couldn’t read $filename: $!";
	while (<IN>) {
		
		# build type map
		if (/^\s*typedef\s+(.*?)\s*(GL\w+)\s*;/) {
			addTypeMapping($1, $2);
		}

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
	close(IN);
}
print "\t\texit(0);\n";
print "}\n";

print "int main(int argc, char *argv[]) {\n";
print "\tglutInit(&argc, argv);\n";
print "\tglutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH);\n";
print "\tglutInitWindowSize(128, 128);\n";
print "\tglutCreateWindow(argv[0]);\n";
print "\tglutDisplayFunc(testFunc);\n";
print "\tglutMainLoop();\n";
print "\treturn 0;\n";
print "}\n";

