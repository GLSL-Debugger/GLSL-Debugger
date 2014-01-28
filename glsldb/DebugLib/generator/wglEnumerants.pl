#!/usr/bin/perl
# File for windows VS target.
# Please fix its rules if you can.

{
    local @ARGV = qw<-mwgl>;
    do 'Enumerants.pl';
}
