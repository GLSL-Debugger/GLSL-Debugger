#!/usr/bin/perl
# This file is refactoring result of old 3 scripts gen*Enumerant.
# Now it requires -m parameter with mode of source file.
# Mode can be gl, glx or wgl. gl is default.

use Getopt::Std;

our $opt_m = "gl";
getopt('m:');

sub out_struct {
    my $name = shift;
    my $elements = join("\n", map { "\t{$_, $_}," } @_);
    print "
static struct {
    GLenum value;
    const char *string;
} ${name}[] = {
$elements
    {0, NULL}
};\n";
}

sub out_gl {
    my @enums = grep(!/GL_FALSE|GL_TRUE|GL_TIMEOUT_IGNORED/, @_);
    my @bits = grep(/_BIT$|_BIT_\w$|_ATTRIB_BITS/, @_);
    &out_struct("glEnumerantsMap", @enums);
    # create OpenGL Bitfield map
    &out_struct("glBitfieldMap", @bits);
}

sub out_glx {
    &out_struct("glxEnumerantsMap", @_);
}

sub out_wgl {
    &out_struct("wglEnumerantsMap", @_);
}

my %modes = (
    "gl" => [qr/^\s*#define\s+(GL_\w+)\s+0x[0-9A-Fa-f]*/, \&out_gl],
    "glx" => [qr/^\s*#define\s+(GLX_\w+)\s+0x[0-9A-Fa-f]*/, \&out_glx],
    "wgl" => [qr/^\s*#define\s+((WGL|ERROR)_\w+)\s+0x[0-9A-Fa-f]*/, \&out_wgl]
);

if (not defined $modes{$opt_m}) {
    die "Mode must be 'gl', 'glx' or 'wgl'";
}

$regex = $modes{$opt_m}[0];
my @matches;
while (<>) {
    if (m/$regex/){
        push @matches, $1;
    }
}

my $func = $modes{$opt_m}[1];
$func->(@matches);
