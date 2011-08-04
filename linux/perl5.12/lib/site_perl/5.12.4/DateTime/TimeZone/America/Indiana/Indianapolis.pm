# This file is auto-generated by the Perl DateTime Suite time zone
# code generator (0.07) This code generator comes with the
# DateTime::TimeZone module distribution in the tools/ directory

#
# Generated from /tmp/hraLGAMw6M/northamerica.  Olson data version 2011h
#
# Do not edit this file directly.
#
package DateTime::TimeZone::America::Indiana::Indianapolis;
BEGIN {
  $DateTime::TimeZone::America::Indiana::Indianapolis::VERSION = '1.35';
}

use strict;

use Class::Singleton;
use DateTime::TimeZone;
use DateTime::TimeZone::OlsonDB;

@DateTime::TimeZone::America::Indiana::Indianapolis::ISA = ( 'Class::Singleton', 'DateTime::TimeZone' );

my $spans =
[
    [
DateTime::TimeZone::NEG_INFINITY,
59418036000,
DateTime::TimeZone::NEG_INFINITY,
59418015322,
-20678,
0,
'LMT'
    ],
    [
59418036000,
60502406400,
59418014400,
60502384800,
-21600,
0,
'CST'
    ],
    [
60502406400,
60520546800,
60502388400,
60520528800,
-18000,
1,
'CDT'
    ],
    [
60520546800,
60533856000,
60520525200,
60533834400,
-21600,
0,
'CST'
    ],
    [
60533856000,
60551996400,
60533838000,
60551978400,
-18000,
1,
'CDT'
    ],
    [
60551996400,
60557781600,
60551974800,
60557760000,
-21600,
0,
'CST'
    ],
    [
60557781600,
61235424000,
60557760000,
61235402400,
-21600,
0,
'CST'
    ],
    [
61235424000,
61243887600,
61235406000,
61243869600,
-18000,
1,
'CDT'
    ],
    [
61243887600,
61252092000,
61243866000,
61252070400,
-21600,
0,
'CST'
    ],
    [
61252092000,
61255468800,
61252070400,
61255447200,
-21600,
0,
'CST'
    ],
    [
61255468800,
61366287600,
61255450800,
61366269600,
-18000,
1,
'CWT'
    ],
    [
61366287600,
61370290800,
61366269600,
61370272800,
-18000,
1,
'CPT'
    ],
    [
61370290800,
61378322400,
61370269200,
61378300800,
-21600,
0,
'CST'
    ],
    [
61378322400,
61388438400,
61378300800,
61388416800,
-21600,
0,
'CST'
    ],
    [
61388438400,
61401740400,
61388420400,
61401722400,
-18000,
1,
'CDT'
    ],
    [
61401740400,
61419888000,
61401718800,
61419866400,
-21600,
0,
'CST'
    ],
    [
61419888000,
61433190000,
61419870000,
61433172000,
-18000,
1,
'CDT'
    ],
    [
61433190000,
61451337600,
61433168400,
61451316000,
-21600,
0,
'CST'
    ],
    [
61451337600,
61464639600,
61451319600,
61464621600,
-18000,
1,
'CDT'
    ],
    [
61464639600,
61482787200,
61464618000,
61482765600,
-21600,
0,
'CST'
    ],
    [
61482787200,
61496089200,
61482769200,
61496071200,
-18000,
1,
'CDT'
    ],
    [
61496089200,
61514841600,
61496067600,
61514820000,
-21600,
0,
'CST'
    ],
    [
61514841600,
61527538800,
61514823600,
61527520800,
-18000,
1,
'CDT'
    ],
    [
61527538800,
61546291200,
61527517200,
61546269600,
-21600,
0,
'CST'
    ],
    [
61546291200,
61559593200,
61546273200,
61559575200,
-18000,
1,
'CDT'
    ],
    [
61559593200,
61577740800,
61559571600,
61577719200,
-21600,
0,
'CST'
    ],
    [
61577740800,
61591042800,
61577722800,
61591024800,
-18000,
1,
'CDT'
    ],
    [
61591042800,
61609190400,
61591021200,
61609168800,
-21600,
0,
'CST'
    ],
    [
61609190400,
61622492400,
61609172400,
61622474400,
-18000,
1,
'CDT'
    ],
    [
61622492400,
61640640000,
61622470800,
61640618400,
-21600,
0,
'CST'
    ],
    [
61640640000,
61653942000,
61640622000,
61653924000,
-18000,
1,
'CDT'
    ],
    [
61653942000,
61672089600,
61653920400,
61672068000,
-21600,
0,
'CST'
    ],
    [
61672089600,
61748895600,
61672071600,
61748877600,
-18000,
0,
'EST'
    ],
    [
61748895600,
61767043200,
61748874000,
61767021600,
-21600,
0,
'CST'
    ],
    [
61767043200,
62104165200,
61767025200,
62104147200,
-18000,
0,
'EST'
    ],
    [
62104165200,
62114194800,
62104147200,
62114176800,
-18000,
0,
'EST'
    ],
    [
62114194800,
62129916000,
62114180400,
62129901600,
-14400,
1,
'EDT'
    ],
    [
62129916000,
62145644400,
62129898000,
62145626400,
-18000,
0,
'EST'
    ],
    [
62145644400,
62161365600,
62145630000,
62161351200,
-14400,
1,
'EDT'
    ],
    [
62161365600,
62167237200,
62161347600,
62167219200,
-18000,
0,
'EST'
    ],
    [
62167237200,
63271774800,
62167219200,
63271756800,
-18000,
0,
'EST'
    ],
    [
63271774800,
63279644400,
63271756800,
63279626400,
-18000,
0,
'EST'
    ],
    [
63279644400,
63297784800,
63279630000,
63297770400,
-14400,
1,
'EDT'
    ],
    [
63297784800,
63309279600,
63297766800,
63309261600,
-18000,
0,
'EST'
    ],
    [
63309279600,
63329839200,
63309265200,
63329824800,
-14400,
1,
'EDT'
    ],
    [
63329839200,
63340729200,
63329821200,
63340711200,
-18000,
0,
'EST'
    ],
    [
63340729200,
63361288800,
63340714800,
63361274400,
-14400,
1,
'EDT'
    ],
    [
63361288800,
63372178800,
63361270800,
63372160800,
-18000,
0,
'EST'
    ],
    [
63372178800,
63392738400,
63372164400,
63392724000,
-14400,
1,
'EDT'
    ],
    [
63392738400,
63404233200,
63392720400,
63404215200,
-18000,
0,
'EST'
    ],
    [
63404233200,
63424792800,
63404218800,
63424778400,
-14400,
1,
'EDT'
    ],
    [
63424792800,
63435682800,
63424774800,
63435664800,
-18000,
0,
'EST'
    ],
    [
63435682800,
63456242400,
63435668400,
63456228000,
-14400,
1,
'EDT'
    ],
    [
63456242400,
63467132400,
63456224400,
63467114400,
-18000,
0,
'EST'
    ],
    [
63467132400,
63487692000,
63467118000,
63487677600,
-14400,
1,
'EDT'
    ],
    [
63487692000,
63498582000,
63487674000,
63498564000,
-18000,
0,
'EST'
    ],
    [
63498582000,
63519141600,
63498567600,
63519127200,
-14400,
1,
'EDT'
    ],
    [
63519141600,
63530031600,
63519123600,
63530013600,
-18000,
0,
'EST'
    ],
    [
63530031600,
63550591200,
63530017200,
63550576800,
-14400,
1,
'EDT'
    ],
    [
63550591200,
63561481200,
63550573200,
63561463200,
-18000,
0,
'EST'
    ],
    [
63561481200,
63582040800,
63561466800,
63582026400,
-14400,
1,
'EDT'
    ],
    [
63582040800,
63593535600,
63582022800,
63593517600,
-18000,
0,
'EST'
    ],
    [
63593535600,
63614095200,
63593521200,
63614080800,
-14400,
1,
'EDT'
    ],
    [
63614095200,
63624985200,
63614077200,
63624967200,
-18000,
0,
'EST'
    ],
    [
63624985200,
63645544800,
63624970800,
63645530400,
-14400,
1,
'EDT'
    ],
    [
63645544800,
63656434800,
63645526800,
63656416800,
-18000,
0,
'EST'
    ],
    [
63656434800,
63676994400,
63656420400,
63676980000,
-14400,
1,
'EDT'
    ],
    [
63676994400,
63687884400,
63676976400,
63687866400,
-18000,
0,
'EST'
    ],
    [
63687884400,
63708444000,
63687870000,
63708429600,
-14400,
1,
'EDT'
    ],
    [
63708444000,
63719334000,
63708426000,
63719316000,
-18000,
0,
'EST'
    ],
    [
63719334000,
63739893600,
63719319600,
63739879200,
-14400,
1,
'EDT'
    ],
    [
63739893600,
63751388400,
63739875600,
63751370400,
-18000,
0,
'EST'
    ],
    [
63751388400,
63771948000,
63751374000,
63771933600,
-14400,
1,
'EDT'
    ],
    [
63771948000,
63782838000,
63771930000,
63782820000,
-18000,
0,
'EST'
    ],
    [
63782838000,
63803397600,
63782823600,
63803383200,
-14400,
1,
'EDT'
    ],
];

sub olson_version { '2011h' }

sub has_dst_changes { 33 }

sub _max_year { 2021 }

sub _new_instance
{
    return shift->_init( @_, spans => $spans );
}

sub _last_offset { -18000 }

my $last_observance = bless( {
  'format' => 'E%sT',
  'gmtoff' => '-5:00',
  'local_start_datetime' => bless( {
    'formatter' => undef,
    'local_rd_days' => 732312,
    'local_rd_secs' => 0,
    'offset_modifier' => 0,
    'rd_nanosecs' => 0,
    'tz' => bless( {
      'name' => 'floating',
      'offset' => 0
    }, 'DateTime::TimeZone::Floating' ),
    'utc_rd_days' => 732312,
    'utc_rd_secs' => 0,
    'utc_year' => 2007
  }, 'DateTime' ),
  'offset_from_std' => 0,
  'offset_from_utc' => -18000,
  'until' => [],
  'utc_start_datetime' => bless( {
    'formatter' => undef,
    'local_rd_days' => 732312,
    'local_rd_secs' => 18000,
    'offset_modifier' => 0,
    'rd_nanosecs' => 0,
    'tz' => bless( {
      'name' => 'floating',
      'offset' => 0
    }, 'DateTime::TimeZone::Floating' ),
    'utc_rd_days' => 732312,
    'utc_rd_secs' => 18000,
    'utc_year' => 2007
  }, 'DateTime' )
}, 'DateTime::TimeZone::OlsonDB::Observance' )
;
sub _last_observance { $last_observance }

my $rules = [
  bless( {
    'at' => '2:00',
    'from' => '2007',
    'in' => 'Nov',
    'letter' => 'S',
    'name' => 'US',
    'offset_from_std' => 0,
    'on' => 'Sun>=1',
    'save' => '0',
    'to' => 'max',
    'type' => undef
  }, 'DateTime::TimeZone::OlsonDB::Rule' ),
  bless( {
    'at' => '2:00',
    'from' => '2007',
    'in' => 'Mar',
    'letter' => 'D',
    'name' => 'US',
    'offset_from_std' => 3600,
    'on' => 'Sun>=8',
    'save' => '1:00',
    'to' => 'max',
    'type' => undef
  }, 'DateTime::TimeZone::OlsonDB::Rule' )
]
;
sub _rules { $rules }


1;

