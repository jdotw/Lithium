# This file is auto-generated by the Perl DateTime Suite time zone
# code generator (0.07) This code generator comes with the
# DateTime::TimeZone module distribution in the tools/ directory

#
# Generated from /tmp/huU6wrczd0/australasia.  Olson data version 2011i
#
# Do not edit this file directly.
#
package DateTime::TimeZone::Pacific::Auckland;
{
  $DateTime::TimeZone::Pacific::Auckland::VERSION = '1.40';
}

use strict;

use Class::Singleton;
use DateTime::TimeZone;
use DateTime::TimeZone::OlsonDB;

@DateTime::TimeZone::Pacific::Auckland::ISA = ( 'Class::Singleton', 'DateTime::TimeZone' );

my $spans =
[
    [
DateTime::TimeZone::NEG_INFINITY,
58943247656,
DateTime::TimeZone::NEG_INFINITY,
58943289600,
41944,
0,
'LMT'
    ],
    [
58943247656,
60805348200,
58943289056,
60805389600,
41400,
0,
'NZMT'
    ],
    [
60805348200,
60815626200,
60805393200,
60815671200,
45000,
1,
'NZST'
    ],
    [
60815626200,
60834983400,
60815667600,
60835024800,
41400,
0,
'NZMT'
    ],
    [
60834983400,
60848287200,
60835026600,
60848330400,
43200,
1,
'NZST'
    ],
    [
60848287200,
60866433000,
60848328600,
60866474400,
41400,
0,
'NZMT'
    ],
    [
60866433000,
60879736800,
60866476200,
60879780000,
43200,
1,
'NZST'
    ],
    [
60879736800,
60897882600,
60879778200,
60897924000,
41400,
0,
'NZMT'
    ],
    [
60897882600,
60911186400,
60897925800,
60911229600,
43200,
1,
'NZST'
    ],
    [
60911186400,
60929332200,
60911227800,
60929373600,
41400,
0,
'NZMT'
    ],
    [
60929332200,
60943240800,
60929375400,
60943284000,
43200,
1,
'NZST'
    ],
    [
60943240800,
60960781800,
60943282200,
60960823200,
41400,
0,
'NZMT'
    ],
    [
60960781800,
60974690400,
60960825000,
60974733600,
43200,
1,
'NZST'
    ],
    [
60974690400,
60992231400,
60974731800,
60992272800,
41400,
0,
'NZMT'
    ],
    [
60992231400,
61009768800,
60992274600,
61009812000,
43200,
1,
'NZST'
    ],
    [
61009768800,
61023076200,
61009810200,
61023117600,
41400,
0,
'NZMT'
    ],
    [
61023076200,
61041218400,
61023119400,
61041261600,
43200,
1,
'NZST'
    ],
    [
61041218400,
61054525800,
61041259800,
61054567200,
41400,
0,
'NZMT'
    ],
    [
61054525800,
61072668000,
61054569000,
61072711200,
43200,
1,
'NZST'
    ],
    [
61072668000,
61085975400,
61072709400,
61086016800,
41400,
0,
'NZMT'
    ],
    [
61085975400,
61104117600,
61086018600,
61104160800,
43200,
1,
'NZST'
    ],
    [
61104117600,
61117425000,
61104159000,
61117466400,
41400,
0,
'NZMT'
    ],
    [
61117425000,
61135567200,
61117468200,
61135610400,
43200,
1,
'NZST'
    ],
    [
61135567200,
61148874600,
61135608600,
61148916000,
41400,
0,
'NZMT'
    ],
    [
61148874600,
61167621600,
61148917800,
61167664800,
43200,
1,
'NZST'
    ],
    [
61167621600,
61180324200,
61167663000,
61180365600,
41400,
0,
'NZMT'
    ],
    [
61180324200,
61199071200,
61180367400,
61199114400,
43200,
1,
'NZST'
    ],
    [
61199071200,
61212378600,
61199112600,
61212420000,
41400,
0,
'NZMT'
    ],
    [
61212378600,
61378257600,
61212421800,
61378300800,
43200,
1,
'NZST'
    ],
    [
61378257600,
62288316000,
61378300800,
62288359200,
43200,
0,
'NZST'
    ],
    [
62288316000,
62297992800,
62288362800,
62298039600,
46800,
1,
'NZDT'
    ],
    [
62297992800,
62319160800,
62298036000,
62319204000,
43200,
0,
'NZST'
    ],
    [
62319160800,
62330652000,
62319207600,
62330698800,
46800,
1,
'NZDT'
    ],
    [
62330652000,
62351215200,
62330695200,
62351258400,
43200,
0,
'NZST'
    ],
    [
62351215200,
62362101600,
62351262000,
62362148400,
46800,
1,
'NZDT'
    ],
    [
62362101600,
62382664800,
62362144800,
62382708000,
43200,
0,
'NZST'
    ],
    [
62382664800,
62393551200,
62382711600,
62393598000,
46800,
1,
'NZDT'
    ],
    [
62393551200,
62414114400,
62393594400,
62414157600,
43200,
0,
'NZST'
    ],
    [
62414114400,
62425000800,
62414161200,
62425047600,
46800,
1,
'NZDT'
    ],
    [
62425000800,
62445564000,
62425044000,
62445607200,
43200,
0,
'NZST'
    ],
    [
62445564000,
62456450400,
62445610800,
62456497200,
46800,
1,
'NZDT'
    ],
    [
62456450400,
62477013600,
62456493600,
62477056800,
43200,
0,
'NZST'
    ],
    [
62477013600,
62487900000,
62477060400,
62487946800,
46800,
1,
'NZDT'
    ],
    [
62487900000,
62508463200,
62487943200,
62508506400,
43200,
0,
'NZST'
    ],
    [
62508463200,
62519954400,
62508510000,
62520001200,
46800,
1,
'NZDT'
    ],
    [
62519954400,
62540517600,
62519997600,
62540560800,
43200,
0,
'NZST'
    ],
    [
62540517600,
62551404000,
62540564400,
62551450800,
46800,
1,
'NZDT'
    ],
    [
62551404000,
62571967200,
62551447200,
62572010400,
43200,
0,
'NZST'
    ],
    [
62571967200,
62582853600,
62572014000,
62582900400,
46800,
1,
'NZDT'
    ],
    [
62582853600,
62603416800,
62582896800,
62603460000,
43200,
0,
'NZST'
    ],
    [
62603416800,
62614303200,
62603463600,
62614350000,
46800,
1,
'NZDT'
    ],
    [
62614303200,
62634866400,
62614346400,
62634909600,
43200,
0,
'NZST'
    ],
    [
62634866400,
62645752800,
62634913200,
62645799600,
46800,
1,
'NZDT'
    ],
    [
62645752800,
62666316000,
62645796000,
62666359200,
43200,
0,
'NZST'
    ],
    [
62666316000,
62677202400,
62666362800,
62677249200,
46800,
1,
'NZDT'
    ],
    [
62677202400,
62697765600,
62677245600,
62697808800,
43200,
0,
'NZST'
    ],
    [
62697765600,
62709256800,
62697812400,
62709303600,
46800,
1,
'NZDT'
    ],
    [
62709256800,
62729820000,
62709300000,
62729863200,
43200,
0,
'NZST'
    ],
    [
62729820000,
62740706400,
62729866800,
62740753200,
46800,
1,
'NZDT'
    ],
    [
62740706400,
62759455200,
62740749600,
62759498400,
43200,
0,
'NZST'
    ],
    [
62759455200,
62773365600,
62759502000,
62773412400,
46800,
1,
'NZDT'
    ],
    [
62773365600,
62790904800,
62773408800,
62790948000,
43200,
0,
'NZST'
    ],
    [
62790904800,
62804815200,
62790951600,
62804862000,
46800,
1,
'NZDT'
    ],
    [
62804815200,
62822354400,
62804858400,
62822397600,
43200,
0,
'NZST'
    ],
    [
62822354400,
62836264800,
62822401200,
62836311600,
46800,
1,
'NZDT'
    ],
    [
62836264800,
62853804000,
62836308000,
62853847200,
43200,
0,
'NZST'
    ],
    [
62853804000,
62868319200,
62853850800,
62868366000,
46800,
1,
'NZDT'
    ],
    [
62868319200,
62885253600,
62868362400,
62885296800,
43200,
0,
'NZST'
    ],
    [
62885253600,
62899768800,
62885300400,
62899815600,
46800,
1,
'NZDT'
    ],
    [
62899768800,
62916703200,
62899812000,
62916746400,
43200,
0,
'NZST'
    ],
    [
62916703200,
62931218400,
62916750000,
62931265200,
46800,
1,
'NZDT'
    ],
    [
62931218400,
62948152800,
62931261600,
62948196000,
43200,
0,
'NZST'
    ],
    [
62948152800,
62962668000,
62948199600,
62962714800,
46800,
1,
'NZDT'
    ],
    [
62962668000,
62980207200,
62962711200,
62980250400,
43200,
0,
'NZST'
    ],
    [
62980207200,
62994117600,
62980254000,
62994164400,
46800,
1,
'NZDT'
    ],
    [
62994117600,
63011656800,
62994160800,
63011700000,
43200,
0,
'NZST'
    ],
    [
63011656800,
63025567200,
63011703600,
63025614000,
46800,
1,
'NZDT'
    ],
    [
63025567200,
63043106400,
63025610400,
63043149600,
43200,
0,
'NZST'
    ],
    [
63043106400,
63057621600,
63043153200,
63057668400,
46800,
1,
'NZDT'
    ],
    [
63057621600,
63074556000,
63057664800,
63074599200,
43200,
0,
'NZST'
    ],
    [
63074556000,
63089071200,
63074602800,
63089118000,
46800,
1,
'NZDT'
    ],
    [
63089071200,
63106005600,
63089114400,
63106048800,
43200,
0,
'NZST'
    ],
    [
63106005600,
63120520800,
63106052400,
63120567600,
46800,
1,
'NZDT'
    ],
    [
63120520800,
63138060000,
63120564000,
63138103200,
43200,
0,
'NZST'
    ],
    [
63138060000,
63151970400,
63138106800,
63152017200,
46800,
1,
'NZDT'
    ],
    [
63151970400,
63169509600,
63152013600,
63169552800,
43200,
0,
'NZST'
    ],
    [
63169509600,
63183420000,
63169556400,
63183466800,
46800,
1,
'NZDT'
    ],
    [
63183420000,
63200959200,
63183463200,
63201002400,
43200,
0,
'NZST'
    ],
    [
63200959200,
63215474400,
63201006000,
63215521200,
46800,
1,
'NZDT'
    ],
    [
63215474400,
63232408800,
63215517600,
63232452000,
43200,
0,
'NZST'
    ],
    [
63232408800,
63246924000,
63232455600,
63246970800,
46800,
1,
'NZDT'
    ],
    [
63246924000,
63263858400,
63246967200,
63263901600,
43200,
0,
'NZST'
    ],
    [
63263858400,
63278373600,
63263905200,
63278420400,
46800,
1,
'NZDT'
    ],
    [
63278373600,
63295308000,
63278416800,
63295351200,
43200,
0,
'NZST'
    ],
    [
63295308000,
63309823200,
63295354800,
63309870000,
46800,
1,
'NZDT'
    ],
    [
63309823200,
63326757600,
63309866400,
63326800800,
43200,
0,
'NZST'
    ],
    [
63326757600,
63343087200,
63326804400,
63343134000,
46800,
1,
'NZDT'
    ],
    [
63343087200,
63358207200,
63343130400,
63358250400,
43200,
0,
'NZST'
    ],
    [
63358207200,
63374536800,
63358254000,
63374583600,
46800,
1,
'NZDT'
    ],
    [
63374536800,
63389656800,
63374580000,
63389700000,
43200,
0,
'NZST'
    ],
    [
63389656800,
63405986400,
63389703600,
63406033200,
46800,
1,
'NZDT'
    ],
    [
63405986400,
63421106400,
63406029600,
63421149600,
43200,
0,
'NZST'
    ],
    [
63421106400,
63437436000,
63421153200,
63437482800,
46800,
1,
'NZDT'
    ],
    [
63437436000,
63452556000,
63437479200,
63452599200,
43200,
0,
'NZST'
    ],
    [
63452556000,
63468885600,
63452602800,
63468932400,
46800,
1,
'NZDT'
    ],
    [
63468885600,
63484610400,
63468928800,
63484653600,
43200,
0,
'NZST'
    ],
    [
63484610400,
63500940000,
63484657200,
63500986800,
46800,
1,
'NZDT'
    ],
    [
63500940000,
63516060000,
63500983200,
63516103200,
43200,
0,
'NZST'
    ],
    [
63516060000,
63532389600,
63516106800,
63532436400,
46800,
1,
'NZDT'
    ],
    [
63532389600,
63547509600,
63532432800,
63547552800,
43200,
0,
'NZST'
    ],
    [
63547509600,
63563839200,
63547556400,
63563886000,
46800,
1,
'NZDT'
    ],
    [
63563839200,
63578959200,
63563882400,
63579002400,
43200,
0,
'NZST'
    ],
    [
63578959200,
63595288800,
63579006000,
63595335600,
46800,
1,
'NZDT'
    ],
    [
63595288800,
63610408800,
63595332000,
63610452000,
43200,
0,
'NZST'
    ],
    [
63610408800,
63626738400,
63610455600,
63626785200,
46800,
1,
'NZDT'
    ],
    [
63626738400,
63641858400,
63626781600,
63641901600,
43200,
0,
'NZST'
    ],
    [
63641858400,
63658188000,
63641905200,
63658234800,
46800,
1,
'NZDT'
    ],
    [
63658188000,
63673912800,
63658231200,
63673956000,
43200,
0,
'NZST'
    ],
    [
63673912800,
63690242400,
63673959600,
63690289200,
46800,
1,
'NZDT'
    ],
    [
63690242400,
63705362400,
63690285600,
63705405600,
43200,
0,
'NZST'
    ],
    [
63705362400,
63721692000,
63705409200,
63721738800,
46800,
1,
'NZDT'
    ],
    [
63721692000,
63736812000,
63721735200,
63736855200,
43200,
0,
'NZST'
    ],
    [
63736812000,
63753141600,
63736858800,
63753188400,
46800,
1,
'NZDT'
    ],
    [
63753141600,
63768261600,
63753184800,
63768304800,
43200,
0,
'NZST'
    ],
    [
63768261600,
63784591200,
63768308400,
63784638000,
46800,
1,
'NZDT'
    ],
    [
63784591200,
63799711200,
63784634400,
63799754400,
43200,
0,
'NZST'
    ],
];

sub olson_version { '2011i' }

sub has_dst_changes { 63 }

sub _max_year { 2021 }

sub _new_instance
{
    return shift->_init( @_, spans => $spans );
}

sub _last_offset { 43200 }

my $last_observance = bless( {
  'format' => 'NZ%sT',
  'gmtoff' => '12:00',
  'local_start_datetime' => bless( {
    'formatter' => undef,
    'local_rd_days' => 710397,
    'local_rd_secs' => 0,
    'offset_modifier' => 0,
    'rd_nanosecs' => 0,
    'tz' => bless( {
      'name' => 'floating',
      'offset' => 0
    }, 'DateTime::TimeZone::Floating' ),
    'utc_rd_days' => 710397,
    'utc_rd_secs' => 0,
    'utc_year' => 1947
  }, 'DateTime' ),
  'offset_from_std' => 0,
  'offset_from_utc' => 43200,
  'until' => [],
  'utc_start_datetime' => bless( {
    'formatter' => undef,
    'local_rd_days' => 710396,
    'local_rd_secs' => 43200,
    'offset_modifier' => 0,
    'rd_nanosecs' => 0,
    'tz' => bless( {
      'name' => 'floating',
      'offset' => 0
    }, 'DateTime::TimeZone::Floating' ),
    'utc_rd_days' => 710396,
    'utc_rd_secs' => 43200,
    'utc_year' => 1946
  }, 'DateTime' )
}, 'DateTime::TimeZone::OlsonDB::Observance' )
;
sub _last_observance { $last_observance }

my $rules = [
  bless( {
    'at' => '2:00s',
    'from' => '2007',
    'in' => 'Sep',
    'letter' => 'D',
    'name' => 'NZ',
    'offset_from_std' => 3600,
    'on' => 'lastSun',
    'save' => '1:00',
    'to' => 'max',
    'type' => undef
  }, 'DateTime::TimeZone::OlsonDB::Rule' ),
  bless( {
    'at' => '2:00s',
    'from' => '2008',
    'in' => 'Apr',
    'letter' => 'S',
    'name' => 'NZ',
    'offset_from_std' => 0,
    'on' => 'Sun>=1',
    'save' => '0',
    'to' => 'max',
    'type' => undef
  }, 'DateTime::TimeZone::OlsonDB::Rule' )
]
;
sub _rules { $rules }


1;

