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
require "functionsAllowedInBeginEnd.pl";
require "prePostExecuteList.pl";
require "justCopyPointersList.pl";

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
	"__GLXextFuncPtr" => "void *",
	"VLServer" => "void *",
	"VLPath" => "int",
	"VLNode" => "int",
	"DMbuffer" => "void *",
	"DMparams" => "struct",
	"HDC" => "void *",
	"HANDLE" => "void *",
	"UINT" => "unsigned int",
	"FLOAT" => "float",
	"INT" => "int",
	"DWORD" => "unsigned int",
	"HGLRC" => "void *",
	"LPCSTR" => "const char *",
	"BOOL" => "int",
	"PROC" => "void *",
	"HPBUFFERARB" => "void *",
	"HPBUFFEREXT" => "void *",
	"HGPUNV" => "void *",
	"INT64" => "__int64",
	"LPVOID" => "void *",
	"PGPU_DEVICE" => "void *",
	"GPU_DEVICE" => "struct",
	"LPGLYPHMETRICSFLOAT" => "void *",
	"LPLAYERPLANEDESCRIPTOR" => "void *",
	"HVIDEOOUTPUTDEVICENV" => "void *",
	"HPVIDEODEV" => "void *"
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

sub getTypeId
{
	my $arg = stripStorageQualifiers(shift);
	#print "STRIPED ARG: ##$arg##\n";
	if ($arg =~ /[*]|[\[]/) {
		return "DBG_TYPE_POINTER";
	} elsif ($arg =~ /GLbitfield/) {
		return "DBG_TYPE_BITFIELD";
	} elsif ($arg =~ /GLenum/) {
		return "DBG_TYPE_ENUM";
	} elsif ($arg =~ /GLboolean/) {
		return "DBG_TYPE_BOOLEAN";
	} elsif ($arg =~ /struct|union/) {
		return "DBG_TYPE_STRUCT";
	} elsif ($arg =~ /enum/) {
		return "DBG_TYPE_INT";
	} elsif ($typeMap{$arg}) {
		return getBasicTypeId($typeMap{$arg});
	} else {
		return getBasicTypeId($arg);
	}
}

sub printArguments
{
	my @arguments = @_;
	if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
		for (my $i = 0; $i <= $#arguments; $i++) {
			print "arg$i";
			if ($i != $#arguments) {
				print ", ";
			}
		}
	}
}

sub printArgumentReferences
{
	my @arguments = @_;
	if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
		for (my $i = 0; $i <= $#arguments; $i++) {
			print "&arg$i";
			if ($i != $#arguments) {
				print ", ";
			}
		}
	}
}

sub printPreExecute
{
	my ($indent, $fname, @arguments) = @_;
	if (scalar grep {$fname eq $_} @preExecutionList) {
		print "$indent$fname";
		print "_PREEXECUTE(";
		printArgumentReferences(@arguments);
		print ");\n";
	}
}

sub printPostExecute
{
	my ($indent, $fname, $retval, @arguments) = @_;
	if (scalar grep {$fname eq $_} @postExecutionList) {
		print "$indent$fname";
		print "_POSTEXECUTE(";
		printArgumentReferences(@arguments);
		if ($retval !~ /^void$|^$/i) {
			print ", &result";
		}
		print ", &error);\n";
	}
}

# TODO: check position of unlock statements!!!

sub createBody
{
	my $retval = shift;
	my $fname = shift;
	my $argString = shift;
	my $checkError = shift;
	my @arguments = buildArgumentList($argString);
	my $pfname = join("","PFN",uc($fname),"PROC");

	my $unlockStatement;
	if (defined $WIN32) {
		$unlockStatement = "LeaveCriticalSection(&G.lock);\n";
	} else {
		$unlockStatement = "pthread_mutex_unlock(&G.lock);\n";
	}

	###########################################################################
	# create function head
	###########################################################################
	if (defined $WIN32) {
		print "__declspec(dllexport) $retval APIENTRY Detoured$fname (";
	} else {
		print "$retval $fname (";
	}
	# add arguments to function head
	my $i = 0;
	foreach (@arguments) {
		if ($_ =~ /^void$|^$/i) {
			print "void";
		} else {
			print "$_ arg$i";
			if ($i != $#arguments) {
				print ", ";
			}
		}	
		$i++;
	}
	print ")\n{\n";
	# temp. variable that holds the return value of the function
	if ($retval !~ /^void$|^$/i) {
		print "\t\t$retval result;\n";
	}
	###########################################################################
	# first store name of called function and its arguments in the shared memory
	# segment, then call dbgFunctionCall that stops the process/thread and waits
	# for the debugger to handle the actual debugging
	print "\t\tint op, error;\n";
	if (defined $WIN32) {
		print "\t\tDbgRec *rec;\n";
		print "\t\tdbgPrint(DBGLVL_DEBUG, \"entering $fname\\n\");\n";
		print "\t\trec = getThreadRecord(GetCurrentProcessId());\n";
		if ($fname eq "glEnd") {
			print "\t\tG.errorCheckAllowed = 1;\n";
		}
		if ($fname eq "glBegin") {
			print "\t\tG.errorCheckAllowed = 0;\n";
		}
		print "\t\tif(rec->isRecursing) {\n";
		print "\t\t\tdbgPrint(DBGLVL_DEBUG, \"stopping recursion\\n\");\n";
		printPreExecute("\t\t\t", $fname, @arguments);
		if ($retval !~ /^void$|^$/i) {
			print "\t\t\tresult = ";
		} else {
			print "\t\t\t";
		}
		print "ORIG_GL($fname)(";
		printArguments(@arguments);
		print ");\n";
		print "\t\t\t/* no way to check errors in recursive calls! */\n";
		print "\t\t\terror = GL_NO_ERROR;\n";
		printPostExecute("\t\t\t", $fname, $retval, @arguments);
		if ($retval !~ /^void$|^$/i) {
			print "\t\t\treturn result;\n";
		} else {
			print "\t\t\treturn;\n";
		}
		print "\t\t}\n";
		print "\t\trec->isRecursing = 1;\n";
		print "\t\tEnterCriticalSection(&G.lock);\n";
	} else {
		print "\t\tpthread_mutex_lock(&G.lock);\n";
	}
	print "\t\tif (keepExecuting(\"$fname\")) {\n";
	print "\t\t\t$unlockStatement";
	printPreExecute("\t\t\t", $fname, @arguments);
	if ($retval !~ /^void$|^$/i) {
		print "\t\t\tresult = ";
	} else {
		print "\t\t\t";
	}
	print "ORIG_GL($fname)(";
	printArguments(@arguments);
	print ");\n";

	print "\t\t\tif (checkGLErrorInExecution()) {\n";

	if ($retval !~ /^void$|^$/i) {
		if ($checkError) {
			if (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
				print "\t\t\t\terror = ORIG_GL(glGetError)();\n";

			} else {
				print "\t\t\t\tif (G.errorCheckAllowed) {\n";
				print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
				print "\t\t\t\t} else {\n";
				print "\t\t\t\t\terror = GL_NO_ERROR;\n";
				print "\t\t\t\t}\n";
			}
		} else {
			print "\t\t\t\terror = GL_NO_ERROR;\n";
		}
	} elsif ($checkError) {
		if ($fname eq "glBegin") {
			# never check error after glBegin
			print "\t\t\t\terror = GL_NO_ERROR;\n";
		} elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
			print "\t\t\t\terror = ORIG_GL(glGetError)();\n";
		} else {
			print "\t\t\t\tif (G.errorCheckAllowed) {\n";
			print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
			print "\t\t\t\t} else {\n\t\t\t\t\terror = GL_NO_ERROR;\n\t\t\t\t}\n";
		}
	} else {
		print "\t\t\t\terror = GL_NO_ERROR;\n";
	}
	printPostExecute("\t\t\t\t", $fname, $retval, @arguments);
	print "\t\t\t\tif (error != GL_NO_ERROR) {\n";
	print "\t\t\t\t\tsetErrorCode(error);\n";
	print "\t\t\t\t\tstop();\n";
	print "\t\t\t\t} else {\n";
	if (defined $WIN32) {
		print "\t\t\t\t\trec->isRecursing = 0;\n";
	}
	if ($retval =~ /^void$|^$/i) {
		print "\t\t\t\t\treturn;\n";
	} else {
		print "\t\t\t\t\treturn result;\n";
	}
	print "\t\t\t\t}\n";
	print "\t\t\t} else {\n";
	if (scalar grep {$fname eq $_} @postExecutionList) {
		print "\t\t\t\t\terror = GL_NO_ERROR;\n";
		printPostExecute("\t\t\t\t", $fname, $retval, @arguments);
		print "\t\t\t\tif (error != GL_NO_ERROR) {\n";
		print "\t\t\t\t\tsetErrorCode(error);\n";
		print "\t\t\t\t\tstop();\n";
		print "\t\t\t\t} else {\n";
		if(defined $WIN32) {
			print "\t\t\t\t\trec->isRecursing = 0;\n";
		}
		if ($retval =~ /^void$|^$/i) {
			print "\t\t\t\t\treturn;\n";
		} else {
			print "\t\t\t\t\treturn result;\n";
		}
		print "\t\t\t\t}\n";
	} else {
		if(defined $WIN32) {
			print "\t\t\t\trec->isRecursing = 0;\n";
		}
		if ($retval =~ /^void$|^$/i) {
			print "\t\t\t\treturn;\n";
		} else {
			print "\t\t\t\treturn result;\n";
		}
	}
	print "\t\t\t}";

	print "\n\t\t}";
	
	#print "\t\tfprintf(stderr, \"ThreadID: %li\\n\", (unsigned long)pthread_self());\n";
	print "
		storeFunctionCall(\"$fname\", ";
	if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
		printf("%i, ", $#arguments + 1);
		for (my $i = 0; $i <= $#arguments; $i++) {
			print "&arg$i, ";
			print getTypeId(@arguments[$i]);
			if ($i != $#arguments) {
				print ", ";
			}
		}
	} else {
		print "0";
	}
	print ");
		stop();
		op = getDbgOperation();
		while (op != DBG_DONE) {
			switch (op) {
			case DBG_CALL_FUNCTION:\n";
	if ($retval !~ /^void$|^$/i) {
		print "\t\t\t\tresult = ";
	} else {
		print "\t\t\t\t";
	}
	print "(($pfname)getDbgFunction())(";
	if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
		for (my $i = 0; $i <= $#arguments; $i++) {
			print "arg$i";
			if ($i != $#arguments) {
				print ", ";
			}
		}
	}
	print ");";
	if ($retval !~ /^void$|^$/i) {
		print "\n\t\t\t\tstoreResult(&result, ";
		print getTypeId($retval);
		print ");"
	}
	print "
				break;
			case DBG_RECORD_CALL:\n#ifdef DBG_STREAM_HINT_";
	printf("%s", uc($fname));
	print "\n#  if DBG_STREAM_HINT_";
	printf("%s", uc($fname));
	print " != DBG_NO_RECORD
				recordFunctionCall(&G.recordedStream, \"$fname\", ";
	if ($#arguments > 1 || @arguments[0] !~ /^void$|^$/i) {
		printf("%i, ", $#arguments + 1);
		for (my $i = 0; $i <= $#arguments; $i++) {
			if (@arguments[$i] =~ /[*]$/) {
				if (scalar grep {$fname eq $_} @justCopyPointersList) {
					print "&arg$i, sizeof(void*)";
				} else {
					print "arg$i, ";
					if ($fname =~ /gl\D+([1234])\D{1,3}v[A-Z]*/ &&
						$fname !~ /glProgramNamedParameter\SvNV/) {
						print "$1*sizeof(";
						my $type = stripStorageQualifiers(@arguments[$i]);
						$type =~ s/\*//;
						$type =~ s/\s*$//;
						print "$type)";
					} else {
						print "$fname";
						print "_getArg$i";
						print "Size(";
						for (my $j = 0; $j <= $#arguments; $j++) {
							print "arg$j";
							if ($j != $#arguments) {
								print ", ";
							}
						}
						print ")"
					}
				}
			} else {
				print "&arg$i, ";
				printf("sizeof(%s)", stripStorageQualifiers(@arguments[$i]));
			}
			if ($i != $#arguments) {
				print ", ";
			}
		}
	} else {
		print "0";
	}
	print ");\n#  endif\n#  if DBG_STREAM_HINT_";
	printf("%s", uc($fname));
	print " == DBG_RECORD_AND_FINAL
				break;\n#  else
				/* FALLTHROUGH!!!! */\n#  endif\n#endif
			case DBG_CALL_ORIGFUNCTION:\n";
	printPreExecute("\t\t\t\t", $fname, @arguments);
	if ($retval !~ /^void$|^$/i) {
		print "\t\t\t\tresult = ";
	} else {
		print "\t\t\t\t";
	}
	print "ORIG_GL($fname)(";
	printArguments(@arguments);
	print ");\n";
	if ($retval !~ /^void$|^$/i) {
		if ($checkError) {
			if (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
				print "\t\t\t\terror = ORIG_GL(glGetError)();\n";
				printPostExecute("\t\t\t\t", $fname, $retval, @arguments);
				print "\t\t\t\tstoreResultOrError(error, &result, ";
				print getTypeId($retval);
				print ");";
			} else {
				print "\t\t\t\tif (G.errorCheckAllowed) {\n";
				print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
				printPostExecute("\t\t\t\t\t", $fname, $retval, @arguments);
				print "\t\t\t\t\tstoreResultOrError(error, &result,";
				print getTypeId($retval);
				print ");\n\t\t\t\t} else {";
				print "\n\t\t\t\t\terror = GL_NO_ERROR;\n";
				printPostExecute("\t\t\t\t\t", $fname, $retval, @arguments);
				print "\t\t\t\t\tstoreResult(&result, ";
				print getTypeId($retval);
				print ");\n\t\t\t\t}";
			}
		} else {
			print "\t\t\t\terror = GL_NO_ERROR;\n";
			printPostExecute("\t\t\t\t", $fname, $retval, @arguments);
			print "\t\t\t\tstoreResult(&result, ";
			print getTypeId($retval);
			print ");\n";
		}
	} elsif ($checkError) {
		if ($fname eq "glBegin") {
			# never check error after glBegin
			print "\n\t\t\t\terror = GL_NO_ERROR;\n";
			printPostExecute("\t\t\t\t", $fname, $retval, @arguments);
		} elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
			print "\n\t\t\t\terror = ORIG_GL(glGetError)();\n";
			printPostExecute("\t\t\t\t", $fname, $retval, @arguments);
		} else {
			print "\t\t\t\tif (G.errorCheckAllowed) {\n";
			print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
			print "\t\t\t\t} else {\n\t\t\t\t\terror = GL_NO_ERROR;\n\t\t\t\t}\n";
			printPostExecute("\t\t\t\t", $fname, $retval, @arguments);
		}
		print "\t\t\t\tsetErrorCode(error);";
	}
	print "\n\t\t\t\tbreak;
			case DBG_EXECUTE:
				setExecuting();
				stop();\n";
	print "\t\t\t\t$unlockStatement";
	printPreExecute("\t\t\t\t", $fname, @arguments);
	if ($retval !~ /^void$|^$/i) {
		if(defined $WIN32) {
			print "\t\t\t\trec->isRecursing = 0;\n";
		}
		print "\t\t\t\tresult = ";
	} else {
		print "\t\t\t\t";
	}
	print "ORIG_GL($fname)(";
	printArguments(@arguments);
	print ");";

	print "\n\t\t\t\tif (checkGLErrorInExecution()) {\n";

	if ($retval !~ /^void$|^$/i) {
		if ($checkError) {
			if (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
				print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";

			} else {
				print "\t\t\t\t\tif (G.errorCheckAllowed) {\n";
				print "\t\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
				print "\t\t\t\t\t} else {\n";
				print "\t\t\t\t\t\terror = GL_NO_ERROR;\n";
				print "\t\t\t\t\t}\n";
			} 
		} else {
			print "\t\t\t\terror = GL_NO_ERROR;\n";
		}
	} elsif($checkError) {
		if ($fname eq "glBegin") {
			# never check error after glBegin
			print "\t\t\t\t\terror = GL_NO_ERROR;\n";
		} elsif (not scalar grep {$fname eq $_} @allowedInBeginEnd) {
			print "\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
		} else {
			print "\t\t\t\t\tif (G.errorCheckAllowed) {\n";
			print "\t\t\t\t\t\terror = ORIG_GL(glGetError)();\n";
			print "\t\t\t\t\t} else {\n\t\t\t\t\terror = GL_NO_ERROR;\n\t\t\t\t}\n";
		}
	} else {
		print "\t\t\t\t\terror = GL_NO_ERROR;\n";
	}
	printPostExecute("\t\t\t\t\t", $fname, $retval, @arguments);
	print "\t\t\t\t\tif (error != GL_NO_ERROR) {\n";
	print "\t\t\t\t\t\tsetErrorCode(error);\n";
	print "\t\t\t\t\t} else {\n";
	if(defined $WIN32) {
		print "\t\t\t\t\t\trec->isRecursing = 0;\n";
	}
	if ($retval =~ /^void$|^$/i) {
		print "\t\t\t\t\t\treturn;\n";
	} else {
		print "\t\t\t\t\t\treturn result;\n";
	}
	print "\t\t\t\t\t}\n";
	print "\t\t\t\t} else {\n";
	
	if (scalar grep {$fname eq $_} @postExecutionList) {
		print "\t\t\t\t\terror = GL_NO_ERROR;\n";
		printPostExecute("\t\t\t\t\t", $fname, $retval, @arguments);
		print "\t\t\t\t\tif (error != GL_NO_ERROR) {\n";
		print "\t\t\t\t\t\tsetErrorCode(error);\n";
		print "\t\t\t\t\t} else {\n";
		if(defined $WIN32) {
			print "\t\t\t\t\t\trec->isRecursing = 0;\n";
		}
		if ($retval =~ /^void$|^$/i) {
			print "\t\t\t\t\t\treturn;\n";
		} else {
			print "\t\t\t\t\t\treturn result;\n";
		}
		print "\t\t\t\t\t}\n"
	} else {
		if(defined $WIN32) {
			print "\t\t\t\t\trec->isRecursing = 0;\n";
		}
		if ($retval =~ /^void$|^$/i) {
			print "\t\t\t\t\treturn;\n";
		} else {
			print "\t\t\t\t\treturn result;\n";
		}
	}
	print "\t\t\t\t}";

	print "
			default:
				executeDefaultDbgOperation(op);
			}
			stop();
			op = getDbgOperation();
		}
		setErrorCode(DBG_NO_ERROR);\n";
    print "\t\t$unlockStatement";		
	if(defined $WIN32) {
		print "\t\trec->isRecursing = 0;\n";
	}
	if ($retval !~ /^void$|^$/i) {
		print "\t\treturn result;\n"
	}
	print "}\n\n";
}

# parse GL headers 
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
			createBody($1, $2, $3, 1);
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

if (defined($WIN32)) {
	# parse WGL headers 
	foreach my $filename ("../GL/WinGDI.h", "../GL/wglext.h") {
		my $indefinition = 0;
		my $inprototypes = 0;
		$extname = "WGL_VERSION_1_0";
		open(IN, $filename) || die "Couldn’t read $filename: $!";
		while (<IN>) {
			# build type map
			if (/^\s*typedef\s+(.*?)\s*(WGL\w+)\s*;/) {
				addTypeMapping($1, $2);
			}

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
				if($fprototype =~ /^\s*(?:WINGDIAPI|extern)\s+(\S.*\S)\s+WINAPI\s+(wgl\S+)\s*\((.*)\)\s*;/ > 0) {
					if ($2 ne "wglGetProcAddress") {
					    createBody($1, $2, $3, 0);
					}
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
	createBody($1, $2, $3, 0);
} else {
	# parse GLX headers 
	foreach my $filename ("../GL/glx.h", "../GL/glxext.h") {
		my $indefinition = 0;
		my $inprototypes = 0;
		$extname = "GLX_VERSION_1_0";
		open(IN, $filename) || die "Couldn’t read $filename: $!";
		while (<IN>) {
			# build type map
			if (/^\s*typedef\s+(.*?)\s*(GLX\w+)\s*;/) {
				addTypeMapping($1, $2);
			}

			if ($indefinition == 1 && /^#define\s+$extname\s+1/) {
					$inprototypes = 1;
			}
			
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
				# No way to parse functions returning a not "typedef'ed"
				# function pointer in a simple way :-(
				if ($2 eq "glXGetProcAddressARB") {
					createBody("__GLXextFuncPtr", "glXGetProcAddressARB", "const GLubyte *", 0);
				} else {
					createBody($1, $2, $3, 0);
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

