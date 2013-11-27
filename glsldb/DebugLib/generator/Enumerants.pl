#!/usr/bin/perl
# This file is refactoring result of old 3 scripts gen*Enumerant.
# Now it requires -m parameter with mode of source file.
# Mode can be gl, glx or wgl. gl is default.

use Getopt::Std;
require genTools;
our %files;
our %regexps;

our $opt_m = "gl";
getopt('m');

sub out_struct {
    my $name = shift;
    my $elements = join("\n", map { "\t{$_, \"$_\"}," } @_);
    print "
static struct {
\tGLenum value;
\tconst char *string;
} ${name}[] = {
$elements
\t{0, NULL}
};
";
}

sub out {
    if ($opt_m eq "glx") {
        out_struct("glxEnumerantsMap", @_);
    } elsif ($opt_m eq "wgl") {
        out_struct("wglEnumerantsMap", @_);
    } else {
        my @enums = grep(!/GL_FALSE|GL_TRUE|GL_TIMEOUT_IGNORED/, @_);
        my @bits = grep(/_BIT$|_BIT_\w$|_ATTRIB_BITS/, @_);
        out_struct("glEnumerantsMap", @enums);
        # create OpenGL Bitfield map
        out_struct("glBitfieldMap", @bits);
    }
}

my @matches;
sub push_matches
{
    my ($line, $match) = (@_);
    push @matches, $match;
}

my %modes = (
    "gl" => {$regexps{"glvar"} => \&push_matches},
    "glx" => {$regexps{"glxvar"} => \&push_matches},
    "wgl" => {$regexps{"wglvar"} => \&push_matches},
);

if (not defined $modes{$opt_m}) {
    die "Mode must be one of " . join(", ", keys %modes) . "\n";
}

my $actions = $modes{$opt_m};
foreach my $filename (@{$files{$opt_m}}) {
    parse_output($filename, $actions);
}

header_generated();
out(@matches);
