#!/usr/bin/perl
# This file is refactoring result of old 3 scripts gen*Enumerant.
# Now it requires -m parameter with mode of source file.
# Mode can be gl, glx or wgl. gl is default.

use Getopt::Std;
require genTools;
our %files;
our %regexps;
our %extnames_defines;
our @problem_defines;

our $opt_m = "gl";
getopt('m');

sub out_struct {
    my $name = shift;
    my $elements = "";
    sub generate_element {
        my $name = $_;
        my $out = "";
        my $need_escape = scalar grep { /^$name$/ } @problem_defines;
        $out .= "#ifdef $name\n" if $need_escape;
        $out .= "\t{$name, \"$name\"},";
        $out .= "\n#endif /* $name */" if $need_escape;
        return $out;
    }

    foreach my $subarray (@_){
        my ($extname, $enums) = (@$subarray);
        next if not @$enums;
        $elements .= "\n#ifdef $extname\n" if $extname;
        $elements .= join("\n", map generate_element, @$enums);
        $elements .= "\n#endif /* $extname */\n" if $extname;
    }
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
        my @enums;
        my @bits;
        foreach my $subarray (@_){
            my ($extname, $elements) = (@$subarray);
            my @e = grep(!/GL_FALSE|GL_TRUE|GL_TIMEOUT_IGNORED/, @$elements);
            my @b = grep(/_BIT$|_BIT_\w+$|_ATTRIB_BITS/, @$elements);
            push @enums, [$extname, \@e];
            push @bits, [$extname, \@b];
        }
        out_struct("glEnumerantsMap", @enums);
        # create OpenGL Bitfield map
        out_struct("glBitfieldMap", @bits);
    }
}

my @matches;
my $last_ext = "";
sub push_matches
{
    my ($line, $extname, $match) = (@_);
    $extname = $extnames_defines{$extname} if defined $extnames_defines{$extname};
    if ($last_ext ne $extname){
        push @matches, [$extname, []];
        $last_ext = $extname;
    }
    $arr = ${$matches[-1]}[-1];
    push @$arr, $match;
}

my %modes = (
    "gl" => ["GL_VERSION_1_0", "GL_", {$regexps{"glvar"} => \&push_matches}],
    "glx" => ["GLX_VERSION_1_0", "GLX_", {$regexps{"glxvar"} => \&push_matches}],
    "wgl" => ["WGL_VERSION_1_0", "WGL_", {$regexps{"wglvar"} => \&push_matches}],
);

if (not defined $modes{$opt_m}) {
    die "Mode must be one of " . join(", ", keys %modes) . "\n";
}

foreach my $filename (@{$files{$opt_m}}) {
    parse_output($filename, @{$modes{$opt_m}}, 1);
}

header_generated();
out(@matches);
