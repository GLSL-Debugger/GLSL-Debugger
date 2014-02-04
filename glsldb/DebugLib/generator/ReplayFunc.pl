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

require prePostExecuteList;
require genTools;
require genTypes;
our %regexps;


sub createBodyHeader
{
    print '#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */
#include "GL/gl.h"
#include "GL/glext.h"
#ifndef _WIN32
#include "GL/glx.h"
#include "GL/glxext.h"
#else /* _WIN32 */
#include "GL/wglext.h"
#include "trampolines.h"
#endif /* _WIN32 */
#include "debuglibInternal.h"
#include "streamRecording.h"
#include "replayFunction.h"

void replayFunctionCall(StoredCall *f, int final)
{
';
}

sub createBodyFooter
{
    print '    {
        fprintf(stderr, "Cannot replay %s: unknown function\n", f->fname);
    }
}
';
}

sub createFunctionHook
{
    my $line = shift;
    my $extname = shift;
    my $retval = shift;
    my $fname = shift;
    my $argString = shift;
    my $ucfname = uc($fname);
    my @arguments = buildArgumentList($argString);
    my $argOutput = arguments_types_array($fname, "f->arguments", @arguments);

    printf "#if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_REPLAY || DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL
    if (!strcmp(\"$fname\", (char*)f->fname)) {
#if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL
        if (final) {
#endif
            ORIG_GL($fname)($argOutput);
#if DBG_STREAM_HINT_$ucfname == DBG_RECORD_AND_FINAL
        }
#endif
        return;
    }
#endif
";
}


header_generated();
createBodyHeader();
parse_gl_files({ $regexps{"glapi"} => \&createFunctionHook });
createBodyFooter();
