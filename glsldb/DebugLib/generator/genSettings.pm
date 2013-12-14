################################################################################
#
# Copyright (c) 2013 SirAnthony <anthony at adsorbtion.org>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
################################################################################

#
# This file contains some settings to use in the parser
#

# The files to parse by category
our %files = (
    "gl" => ["../../GL/gl.h", "../../GL/glext.h"],
    "glx" => ["../../GL/glx.h", "../../GL/glxext.h"],
    "wgl" => ["../../GL/WinGDI.h", "../../GL/wglext.h"]
);


# Some extensions switches defined in comments, due to lack of #ifndef
# for old gl versions. It must be checked when gl*.h updates.
our %extname_matches = (
    # gl.h
    " * Vertex Arrays  (1.1)" => "GL_VERSION_1_1",
    " * Lighting" => "GL_VERSION_1_0",
    "/* 1.1 functions */" => "GL_VERSION_1_1",
    " * OpenGL 1.2" => "GL_VERSION_1_2",
    " * GL_ARB_imaging" => "GL_ARB_imaging",
    " * OpenGL 1.3" => "GL_VERSION_1_3",
    # glx.h
    " * GLX 1.1 and later:" => "GLX_VERSION_1_1",
    "/* GLX 1.1 and later */" => "GLX_VERSION_1_1",
    "/* GLX 1.2 and later */" => "GLX_VERSION_1_2",
    " * GLX 1.3 and later:" => "GLX_VERSION_1_3",
    "/* GLX 1.3 and later */" => "GLX_VERSION_1_3",
    " * GLX 1.4 and later:" => "GLX_VERSION_1_4",
    "** Events." => "GLX_VERSION_1_0",
);


# defines #ifdef for which must be ignored
# This defines does not prevent the parsing of block they surround
our @skip_defines = (
    "GL_GLEXT_PROTOTYPES"
);


# Hash of equivalents for #ifdef in generated files
our %extnames_defines = (
    "GL_VERSION_1_0" => 0,
    "GL_VERSION_1_1" => 0,
    "GL_VERSION_1_2" => 0,
    "GL_VERSION_1_3" => 0,
    "GLX_VERSION_1_0" => 0
);

# This defines caused some problems and must be #ifdef'ed
# TODO: find what caused it
our @problem_defines = (
    "GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT",
    "GL_MIN_PROGRAM_TEXEL_OFFSET_EXT",
    "GL_MAX_PROGRAM_TEXEL_OFFSET_EXT",
    "GL_GLYPH_HAS_KERNING_BIT_NV",
    "GL_FONT_X_MIN_BOUNDS_BIT_NV",
    "GL_FONT_Y_MIN_BOUNDS_BIT_NV",
    "GL_FONT_X_MAX_BOUNDS_BIT_NV",
    "GL_FONT_Y_MAX_BOUNDS_BIT_NV",
    "GL_FONT_UNITS_PER_EM_BIT_NV",
    "GL_FONT_ASCENDER_BIT_NV",
    "GL_FONT_DESCENDER_BIT_NV",
    "GL_FONT_HEIGHT_BIT_NV",
    "GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV",
    "GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV",
    "GL_FONT_UNDERLINE_POSITION_BIT_NV",
    "GL_FONT_UNDERLINE_THICKNESS_BIT_NV",
    "GL_FONT_HAS_KERNING_BIT_NV",
);
