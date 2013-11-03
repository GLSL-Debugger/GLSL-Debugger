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

#~ while (<>) {
    #~
    #~ if ($indefinition == 1) {
        #~ if (/^#define\s+$extname\s+1/) {
            #~ $inprototypes = 1;
        #~ }
    #~ }
    #~
    #~ if (/^#endif/ && $inprototypes == 1) {
        #~ $inprototypes = 0;
        #~ $indefinition = 0;
    #~ }
#~
    #~ if (/^#ifndef\s+(GL_\S+)/) {
        #~ $extname = $1;
        #~ $indefinition = 1;
    #~ }
#~
    #~ # save enumerant definition
    #~ if ($indefinition == 1 && $inprototypes == 0 ||
        #~ $inprototypes == 0 && $indefinition == 0) {
        #~ if (/^\s*#define\s+(GL_\w+).*/) {
            #~ push @glenumerants, $1;
        #~ }
    #~ }
#~ }

while (<>) {
    if(/^\s*#define\s+(GL_\w+)+\s+0x[0-9A-Fa-f]*/){
        my $match = $1;
        if ($match =~ /_BIT$|_BIT_\w$|_ATTRIB_BITS/) {
            # only a wild guess!! May break in the future
            push @glbits, $match;
        }elsif ($match !~ /GL_FALSE|GL_TRUE|GL_TIMEOUT_IGNORED/) {
        # ignore certain entries. Mostly bitmasks and other #defines that are no
        # GLenums.
        # only a wild guess!! May break in the future
            push @glenumerants, $match;
        }
    }
}

# create OpenGL Enumerants map
print "
static struct {
    GLenum value;
    const char *string;
} glEnumerantsMap[] = {\n";
foreach (@glenumerants) {
    print  "\t{$_, \"$_\"},\n";
}
print  "\t{0, NULL}\n";
print "};\n";

# create OpenGL Bitfield map
print "
static struct {
    GLenum value;
    const char *string;
} glBitfieldMap[] = {\n";
foreach (@glbits) {
    print  "\t{$_, \"$_\"},\n";
}
print  "\t{0, NULL}
};\n";

