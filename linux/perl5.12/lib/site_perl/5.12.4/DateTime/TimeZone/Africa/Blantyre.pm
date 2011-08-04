# This file is auto-generated by the Perl DateTime Suite time zone
# code generator (0.07) This code generator comes with the
# DateTime::TimeZone module distribution in the tools/ directory

#
# Generated from /tmp/hraLGAMw6M/africa.  Olson data version 2011h
#
# Do not edit this file directly.
#
package DateTime::TimeZone::Africa::Blantyre;
BEGIN {
  $DateTime::TimeZone::Africa::Blantyre::VERSION = '1.35';
}

use strict;

use Class::Singleton;
use DateTime::TimeZone;
use DateTime::TimeZone::OlsonDB;

@DateTime::TimeZone::Africa::Blantyre::ISA = ( 'Class::Singleton', 'DateTime::TimeZone' );

my $spans =
[
    [
DateTime::TimeZone::NEG_INFINITY,
60026391600,
DateTime::TimeZone::NEG_INFINITY,
60026400000,
8400,
0,
'LMT'
    ],
    [
60026391600,
DateTime::TimeZone::INFINITY,
60026398800,
DateTime::TimeZone::INFINITY,
7200,
0,
'CAT'
    ],
];

sub olson_version { '2011h' }

sub has_dst_changes { 0 }

sub _max_year { 2021 }

sub _new_instance
{
    return shift->_init( @_, spans => $spans );
}



1;

