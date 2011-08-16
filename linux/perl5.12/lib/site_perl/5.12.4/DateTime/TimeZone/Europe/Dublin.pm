# This file is auto-generated by the Perl DateTime Suite time zone
# code generator (0.07) This code generator comes with the
# DateTime::TimeZone module distribution in the tools/ directory

#
# Generated from /tmp/hraLGAMw6M/europe.  Olson data version 2011h
#
# Do not edit this file directly.
#
package DateTime::TimeZone::Europe::Dublin;
BEGIN {
  $DateTime::TimeZone::Europe::Dublin::VERSION = '1.35';
}

use strict;

use Class::Singleton;
use DateTime::TimeZone;
use DateTime::TimeZone::OlsonDB;

@DateTime::TimeZone::Europe::Dublin::ISA = ( 'Class::Singleton', 'DateTime::TimeZone' );

my $spans =
[
    [
DateTime::TimeZone::NEG_INFINITY,
59314033500,
DateTime::TimeZone::NEG_INFINITY,
59314032000,
-1500,
0,
'LMT'
    ],
    [
59314033500,
60443720721,
59314031979,
60443719200,
-1521,
0,
'DMT'
    ],
    [
60443720721,
60455211921,
60443722800,
60455214000,
2079,
1,
'IST'
    ],
    [
60455211921,
60471540000,
60455211921,
60471540000,
0,
0,
'GMT'
    ],
    [
60471540000,
60485536800,
60471543600,
60485540400,
3600,
1,
'BST'
    ],
    [
60485536800,
60501780000,
60485536800,
60501780000,
0,
0,
'GMT'
    ],
    [
60501780000,
60518196000,
60501783600,
60518199600,
3600,
1,
'BST'
    ],
    [
60518196000,
60533834400,
60518196000,
60533834400,
0,
0,
'GMT'
    ],
    [
60533834400,
60549645600,
60533838000,
60549649200,
3600,
1,
'BST'
    ],
    [
60549645600,
60565284000,
60549645600,
60565284000,
0,
0,
'GMT'
    ],
    [
60565284000,
60583514400,
60565287600,
60583518000,
3600,
1,
'BST'
    ],
    [
60583514400,
60597338400,
60583514400,
60597338400,
0,
0,
'GMT'
    ],
    [
60597338400,
60613149600,
60597342000,
60613153200,
3600,
1,
'BST'
    ],
    [
60613149600,
60618672000,
60613149600,
60618672000,
0,
0,
'GMT'
    ],
    [
60618672000,
60628183200,
60618672000,
60628183200,
0,
0,
'GMT'
    ],
    [
60628183200,
60645117600,
60628186800,
60645121200,
3600,
1,
'IST'
    ],
    [
60645117600,
60662052000,
60645117600,
60662052000,
0,
0,
'GMT'
    ],
    [
60662052000,
60674752800,
60662055600,
60674756400,
3600,
1,
'IST'
    ],
    [
60674752800,
60692896800,
60674752800,
60692896800,
0,
0,
'GMT'
    ],
    [
60692896800,
60706807200,
60692900400,
60706810800,
3600,
1,
'IST'
    ],
    [
60706807200,
60724951200,
60706807200,
60724951200,
0,
0,
'GMT'
    ],
    [
60724951200,
60739466400,
60724954800,
60739470000,
3600,
1,
'IST'
    ],
    [
60739466400,
60756400800,
60739466400,
60756400800,
0,
0,
'GMT'
    ],
    [
60756400800,
60770916000,
60756404400,
60770919600,
3600,
1,
'IST'
    ],
    [
60770916000,
60787245600,
60770916000,
60787245600,
0,
0,
'GMT'
    ],
    [
60787245600,
60802365600,
60787249200,
60802369200,
3600,
1,
'IST'
    ],
    [
60802365600,
60819904800,
60802365600,
60819904800,
0,
0,
'GMT'
    ],
    [
60819904800,
60834420000,
60819908400,
60834423600,
3600,
1,
'IST'
    ],
    [
60834420000,
60851354400,
60834420000,
60851354400,
0,
0,
'GMT'
    ],
    [
60851354400,
60865869600,
60851358000,
60865873200,
3600,
1,
'IST'
    ],
    [
60865869600,
60882199200,
60865869600,
60882199200,
0,
0,
'GMT'
    ],
    [
60882199200,
60897319200,
60882202800,
60897322800,
3600,
1,
'IST'
    ],
    [
60897319200,
60914253600,
60897319200,
60914253600,
0,
0,
'GMT'
    ],
    [
60914253600,
60928768800,
60914257200,
60928772400,
3600,
1,
'IST'
    ],
    [
60928768800,
60945703200,
60928768800,
60945703200,
0,
0,
'GMT'
    ],
    [
60945703200,
60960218400,
60945706800,
60960222000,
3600,
1,
'IST'
    ],
    [
60960218400,
60976548000,
60960218400,
60976548000,
0,
0,
'GMT'
    ],
    [
60976548000,
60992272800,
60976551600,
60992276400,
3600,
1,
'IST'
    ],
    [
60992272800,
61009207200,
60992272800,
61009207200,
0,
0,
'GMT'
    ],
    [
61009207200,
61023722400,
61009210800,
61023726000,
3600,
1,
'IST'
    ],
    [
61023722400,
61040052000,
61023722400,
61040052000,
0,
0,
'GMT'
    ],
    [
61040052000,
61055172000,
61040055600,
61055175600,
3600,
1,
'IST'
    ],
    [
61055172000,
61072106400,
61055172000,
61072106400,
0,
0,
'GMT'
    ],
    [
61072106400,
61086621600,
61072110000,
61086625200,
3600,
1,
'IST'
    ],
    [
61086621600,
61103556000,
61086621600,
61103556000,
0,
0,
'GMT'
    ],
    [
61103556000,
61118071200,
61103559600,
61118074800,
3600,
1,
'IST'
    ],
    [
61118071200,
61134400800,
61118071200,
61134400800,
0,
0,
'GMT'
    ],
    [
61134400800,
61149520800,
61134404400,
61149524400,
3600,
1,
'IST'
    ],
    [
61149520800,
61166455200,
61149520800,
61166455200,
0,
0,
'GMT'
    ],
    [
61166455200,
61185204000,
61166458800,
61185207600,
3600,
1,
'IST'
    ],
    [
61185204000,
61193671200,
61185204000,
61193671200,
0,
0,
'GMT'
    ],
    [
61193671200,
61402323600,
61193674800,
61402327200,
3600,
1,
'IST'
    ],
    [
61402323600,
61416237600,
61402323600,
61416237600,
0,
0,
'GMT'
    ],
    [
61416237600,
61436192400,
61416241200,
61436196000,
3600,
1,
'IST'
    ],
    [
61436192400,
61450711200,
61436192400,
61450711200,
0,
0,
'GMT'
    ],
    [
61450711200,
61467645600,
61450714800,
61467649200,
3600,
1,
'IST'
    ],
    [
61467645600,
61480951200,
61467645600,
61480951200,
0,
0,
'GMT'
    ],
    [
61480951200,
61499095200,
61480954800,
61499098800,
3600,
1,
'IST'
    ],
    [
61499095200,
61513610400,
61499095200,
61513610400,
0,
0,
'GMT'
    ],
    [
61513610400,
61529940000,
61513614000,
61529943600,
3600,
1,
'IST'
    ],
    [
61529940000,
61545060000,
61529940000,
61545060000,
0,
0,
'GMT'
    ],
    [
61545060000,
61561389600,
61545063600,
61561393200,
3600,
1,
'IST'
    ],
    [
61561389600,
61577114400,
61561389600,
61577114400,
0,
0,
'GMT'
    ],
    [
61577114400,
61593444000,
61577118000,
61593447600,
3600,
1,
'IST'
    ],
    [
61593444000,
61608564000,
61593444000,
61608564000,
0,
0,
'GMT'
    ],
    [
61608564000,
61623079200,
61608567600,
61623082800,
3600,
1,
'IST'
    ],
    [
61623079200,
61639408800,
61623079200,
61639408800,
0,
0,
'GMT'
    ],
    [
61639408800,
61654528800,
61639412400,
61654532400,
3600,
1,
'IST'
    ],
    [
61654528800,
61671463200,
61654528800,
61671463200,
0,
0,
'GMT'
    ],
    [
61671463200,
61685978400,
61671466800,
61685982000,
3600,
1,
'IST'
    ],
    [
61685978400,
61703517600,
61685978400,
61703517600,
0,
0,
'GMT'
    ],
    [
61703517600,
61718032800,
61703521200,
61718036400,
3600,
1,
'IST'
    ],
    [
61718032800,
61734362400,
61718032800,
61734362400,
0,
0,
'GMT'
    ],
    [
61734362400,
61749482400,
61734366000,
61749486000,
3600,
1,
'IST'
    ],
    [
61749482400,
61766416800,
61749482400,
61766416800,
0,
0,
'GMT'
    ],
    [
61766416800,
61780932000,
61766420400,
61780935600,
3600,
1,
'IST'
    ],
    [
61780932000,
61797866400,
61780932000,
61797866400,
0,
0,
'GMT'
    ],
    [
61797866400,
61812381600,
61797870000,
61812385200,
3600,
1,
'IST'
    ],
    [
61812381600,
61828711200,
61812381600,
61828711200,
0,
0,
'GMT'
    ],
    [
61828711200,
61843831200,
61828714800,
61843834800,
3600,
1,
'IST'
    ],
    [
61843831200,
61858951200,
61843831200,
61858951200,
0,
0,
'GMT'
    ],
    [
61858951200,
61877700000,
61858954800,
61877703600,
3600,
1,
'IST'
    ],
    [
61877700000,
61890400800,
61877700000,
61890400800,
0,
0,
'GMT'
    ],
    [
61890400800,
61909149600,
61890404400,
61909153200,
3600,
1,
'IST'
    ],
    [
61909149600,
61922455200,
61909149600,
61922455200,
0,
0,
'GMT'
    ],
    [
61922455200,
61940599200,
61922458800,
61940602800,
3600,
1,
'IST'
    ],
    [
61940599200,
61953300000,
61940599200,
61953300000,
0,
0,
'GMT'
    ],
    [
61953300000,
61972048800,
61953303600,
61972052400,
3600,
1,
'IST'
    ],
    [
61972048800,
61984749600,
61972048800,
61984749600,
0,
0,
'GMT'
    ],
    [
61984749600,
62003498400,
61984753200,
62003502000,
3600,
1,
'IST'
    ],
    [
62003498400,
62016199200,
62003498400,
62016199200,
0,
0,
'GMT'
    ],
    [
62016199200,
62034948000,
62016202800,
62034951600,
3600,
1,
'IST'
    ],
    [
62034948000,
62047648800,
62034948000,
62047648800,
0,
0,
'GMT'
    ],
    [
62047648800,
62067002400,
62047652400,
62067006000,
3600,
1,
'IST'
    ],
    [
62067002400,
62076679200,
62067002400,
62076679200,
0,
0,
'GMT'
    ],
    [
62076679200,
62098441200,
62076682800,
62098444800,
3600,
1,
'IST'
    ],
    [
62098441200,
62193405600,
62098444800,
62193409200,
3600,
0,
'IST'
    ],
    [
62193405600,
62205501600,
62193405600,
62205501600,
0,
0,
'GMT'
    ],
    [
62205501600,
62224855200,
62205505200,
62224858800,
3600,
1,
'IST'
    ],
    [
62224855200,
62236951200,
62224855200,
62236951200,
0,
0,
'GMT'
    ],
    [
62236951200,
62256304800,
62236954800,
62256308400,
3600,
1,
'IST'
    ],
    [
62256304800,
62268400800,
62256304800,
62268400800,
0,
0,
'GMT'
    ],
    [
62268400800,
62287754400,
62268404400,
62287758000,
3600,
1,
'IST'
    ],
    [
62287754400,
62299850400,
62287754400,
62299850400,
0,
0,
'GMT'
    ],
    [
62299850400,
62319204000,
62299854000,
62319207600,
3600,
1,
'IST'
    ],
    [
62319204000,
62331904800,
62319204000,
62331904800,
0,
0,
'GMT'
    ],
    [
62331904800,
62350653600,
62331908400,
62350657200,
3600,
1,
'IST'
    ],
    [
62350653600,
62363354400,
62350653600,
62363354400,
0,
0,
'GMT'
    ],
    [
62363354400,
62382103200,
62363358000,
62382106800,
3600,
1,
'IST'
    ],
    [
62382103200,
62394804000,
62382103200,
62394804000,
0,
0,
'GMT'
    ],
    [
62394804000,
62414157600,
62394807600,
62414161200,
3600,
1,
'IST'
    ],
    [
62414157600,
62426253600,
62414157600,
62426253600,
0,
0,
'GMT'
    ],
    [
62426253600,
62445607200,
62426257200,
62445610800,
3600,
1,
'IST'
    ],
    [
62445607200,
62457703200,
62445607200,
62457703200,
0,
0,
'GMT'
    ],
    [
62457703200,
62477056800,
62457706800,
62477060400,
3600,
1,
'IST'
    ],
    [
62477056800,
62490358800,
62477056800,
62490358800,
0,
0,
'GMT'
    ],
    [
62490358800,
62508502800,
62490362400,
62508506400,
3600,
1,
'IST'
    ],
    [
62508502800,
62521808400,
62508502800,
62521808400,
0,
0,
'GMT'
    ],
    [
62521808400,
62539952400,
62521812000,
62539956000,
3600,
1,
'IST'
    ],
    [
62539952400,
62553258000,
62539952400,
62553258000,
0,
0,
'GMT'
    ],
    [
62553258000,
62571402000,
62553261600,
62571405600,
3600,
1,
'IST'
    ],
    [
62571402000,
62584707600,
62571402000,
62584707600,
0,
0,
'GMT'
    ],
    [
62584707600,
62603456400,
62584711200,
62603460000,
3600,
1,
'IST'
    ],
    [
62603456400,
62616762000,
62603456400,
62616762000,
0,
0,
'GMT'
    ],
    [
62616762000,
62634906000,
62616765600,
62634909600,
3600,
1,
'IST'
    ],
    [
62634906000,
62648211600,
62634906000,
62648211600,
0,
0,
'GMT'
    ],
    [
62648211600,
62666355600,
62648215200,
62666359200,
3600,
1,
'IST'
    ],
    [
62666355600,
62679661200,
62666355600,
62679661200,
0,
0,
'GMT'
    ],
    [
62679661200,
62697805200,
62679664800,
62697808800,
3600,
1,
'IST'
    ],
    [
62697805200,
62711110800,
62697805200,
62711110800,
0,
0,
'GMT'
    ],
    [
62711110800,
62729254800,
62711114400,
62729258400,
3600,
1,
'IST'
    ],
    [
62729254800,
62742560400,
62729254800,
62742560400,
0,
0,
'GMT'
    ],
    [
62742560400,
62761309200,
62742564000,
62761312800,
3600,
1,
'IST'
    ],
    [
62761309200,
62774010000,
62761309200,
62774010000,
0,
0,
'GMT'
    ],
    [
62774010000,
62792758800,
62774013600,
62792762400,
3600,
1,
'IST'
    ],
    [
62792758800,
62806064400,
62792758800,
62806064400,
0,
0,
'GMT'
    ],
    [
62806064400,
62824208400,
62806068000,
62824212000,
3600,
1,
'IST'
    ],
    [
62824208400,
62837514000,
62824208400,
62837514000,
0,
0,
'GMT'
    ],
    [
62837514000,
62855658000,
62837517600,
62855661600,
3600,
1,
'IST'
    ],
    [
62855658000,
62868963600,
62855658000,
62868963600,
0,
0,
'GMT'
    ],
    [
62868963600,
62887107600,
62868967200,
62887111200,
3600,
1,
'IST'
    ],
    [
62887107600,
62900413200,
62887107600,
62900413200,
0,
0,
'GMT'
    ],
    [
62900413200,
62918557200,
62900416800,
62918560800,
3600,
1,
'IST'
    ],
    [
62918557200,
62931862800,
62918557200,
62931862800,
0,
0,
'GMT'
    ],
    [
62931862800,
62950006800,
62931866400,
62950010400,
3600,
1,
'IST'
    ],
    [
62950006800,
62956137600,
62950006800,
62956137600,
0,
0,
'GMT'
    ],
    [
62956137600,
62963917200,
62956137600,
62963917200,
0,
0,
'GMT'
    ],
    [
62963917200,
62982061200,
62963920800,
62982064800,
3600,
1,
'IST'
    ],
    [
62982061200,
62995366800,
62982061200,
62995366800,
0,
0,
'GMT'
    ],
    [
62995366800,
63013510800,
62995370400,
63013514400,
3600,
1,
'IST'
    ],
    [
63013510800,
63026816400,
63013510800,
63026816400,
0,
0,
'GMT'
    ],
    [
63026816400,
63044960400,
63026820000,
63044964000,
3600,
1,
'IST'
    ],
    [
63044960400,
63058266000,
63044960400,
63058266000,
0,
0,
'GMT'
    ],
    [
63058266000,
63077014800,
63058269600,
63077018400,
3600,
1,
'IST'
    ],
    [
63077014800,
63089715600,
63077014800,
63089715600,
0,
0,
'GMT'
    ],
    [
63089715600,
63108464400,
63089719200,
63108468000,
3600,
1,
'IST'
    ],
    [
63108464400,
63121165200,
63108464400,
63121165200,
0,
0,
'GMT'
    ],
    [
63121165200,
63139914000,
63121168800,
63139917600,
3600,
1,
'IST'
    ],
    [
63139914000,
63153219600,
63139914000,
63153219600,
0,
0,
'GMT'
    ],
    [
63153219600,
63171363600,
63153223200,
63171367200,
3600,
1,
'IST'
    ],
    [
63171363600,
63184669200,
63171363600,
63184669200,
0,
0,
'GMT'
    ],
    [
63184669200,
63202813200,
63184672800,
63202816800,
3600,
1,
'IST'
    ],
    [
63202813200,
63216118800,
63202813200,
63216118800,
0,
0,
'GMT'
    ],
    [
63216118800,
63234867600,
63216122400,
63234871200,
3600,
1,
'IST'
    ],
    [
63234867600,
63247568400,
63234867600,
63247568400,
0,
0,
'GMT'
    ],
    [
63247568400,
63266317200,
63247572000,
63266320800,
3600,
1,
'IST'
    ],
    [
63266317200,
63279018000,
63266317200,
63279018000,
0,
0,
'GMT'
    ],
    [
63279018000,
63297766800,
63279021600,
63297770400,
3600,
1,
'IST'
    ],
    [
63297766800,
63310467600,
63297766800,
63310467600,
0,
0,
'GMT'
    ],
    [
63310467600,
63329216400,
63310471200,
63329220000,
3600,
1,
'IST'
    ],
    [
63329216400,
63342522000,
63329216400,
63342522000,
0,
0,
'GMT'
    ],
    [
63342522000,
63360666000,
63342525600,
63360669600,
3600,
1,
'IST'
    ],
    [
63360666000,
63373971600,
63360666000,
63373971600,
0,
0,
'GMT'
    ],
    [
63373971600,
63392115600,
63373975200,
63392119200,
3600,
1,
'IST'
    ],
    [
63392115600,
63405421200,
63392115600,
63405421200,
0,
0,
'GMT'
    ],
    [
63405421200,
63424170000,
63405424800,
63424173600,
3600,
1,
'IST'
    ],
    [
63424170000,
63436870800,
63424170000,
63436870800,
0,
0,
'GMT'
    ],
    [
63436870800,
63455619600,
63436874400,
63455623200,
3600,
1,
'IST'
    ],
    [
63455619600,
63468320400,
63455619600,
63468320400,
0,
0,
'GMT'
    ],
    [
63468320400,
63487069200,
63468324000,
63487072800,
3600,
1,
'IST'
    ],
    [
63487069200,
63500374800,
63487069200,
63500374800,
0,
0,
'GMT'
    ],
    [
63500374800,
63518518800,
63500378400,
63518522400,
3600,
1,
'IST'
    ],
    [
63518518800,
63531824400,
63518518800,
63531824400,
0,
0,
'GMT'
    ],
    [
63531824400,
63549968400,
63531828000,
63549972000,
3600,
1,
'IST'
    ],
    [
63549968400,
63563274000,
63549968400,
63563274000,
0,
0,
'GMT'
    ],
    [
63563274000,
63581418000,
63563277600,
63581421600,
3600,
1,
'IST'
    ],
    [
63581418000,
63594723600,
63581418000,
63594723600,
0,
0,
'GMT'
    ],
    [
63594723600,
63613472400,
63594727200,
63613476000,
3600,
1,
'IST'
    ],
    [
63613472400,
63626173200,
63613472400,
63626173200,
0,
0,
'GMT'
    ],
    [
63626173200,
63644922000,
63626176800,
63644925600,
3600,
1,
'IST'
    ],
    [
63644922000,
63657622800,
63644922000,
63657622800,
0,
0,
'GMT'
    ],
    [
63657622800,
63676371600,
63657626400,
63676375200,
3600,
1,
'IST'
    ],
    [
63676371600,
63689677200,
63676371600,
63689677200,
0,
0,
'GMT'
    ],
    [
63689677200,
63707821200,
63689680800,
63707824800,
3600,
1,
'IST'
    ],
    [
63707821200,
63721126800,
63707821200,
63721126800,
0,
0,
'GMT'
    ],
    [
63721126800,
63739270800,
63721130400,
63739274400,
3600,
1,
'IST'
    ],
    [
63739270800,
63752576400,
63739270800,
63752576400,
0,
0,
'GMT'
    ],
    [
63752576400,
63771325200,
63752580000,
63771328800,
3600,
1,
'IST'
    ],
    [
63771325200,
63784026000,
63771325200,
63784026000,
0,
0,
'GMT'
    ],
    [
63784026000,
63802774800,
63784029600,
63802778400,
3600,
1,
'IST'
    ],
];

sub olson_version { '2011h' }

sub has_dst_changes { 98 }

sub _max_year { 2021 }

sub _new_instance
{
    return shift->_init( @_, spans => $spans );
}

sub _last_offset { 0 }

my $last_observance = bless( {
  'format' => 'GMT/IST',
  'gmtoff' => '0:00',
  'local_start_datetime' => bless( {
    'formatter' => undef,
    'local_rd_days' => 728659,
    'local_rd_secs' => 0,
    'offset_modifier' => 0,
    'rd_nanosecs' => 0,
    'tz' => bless( {
      'name' => 'floating',
      'offset' => 0
    }, 'DateTime::TimeZone::Floating' ),
    'utc_rd_days' => 728659,
    'utc_rd_secs' => 0,
    'utc_year' => 1997
  }, 'DateTime' ),
  'offset_from_std' => 0,
  'offset_from_utc' => 0,
  'until' => [],
  'utc_start_datetime' => bless( {
    'formatter' => undef,
    'local_rd_days' => 728659,
    'local_rd_secs' => 0,
    'offset_modifier' => 0,
    'rd_nanosecs' => 0,
    'tz' => bless( {
      'name' => 'floating',
      'offset' => 0
    }, 'DateTime::TimeZone::Floating' ),
    'utc_rd_days' => 728659,
    'utc_rd_secs' => 0,
    'utc_year' => 1997
  }, 'DateTime' )
}, 'DateTime::TimeZone::OlsonDB::Observance' )
;
sub _last_observance { $last_observance }

my $rules = [
  bless( {
    'at' => '1:00u',
    'from' => '1996',
    'in' => 'Oct',
    'letter' => '',
    'name' => 'EU',
    'offset_from_std' => 0,
    'on' => 'lastSun',
    'save' => '0',
    'to' => 'max',
    'type' => undef
  }, 'DateTime::TimeZone::OlsonDB::Rule' ),
  bless( {
    'at' => '1:00u',
    'from' => '1981',
    'in' => 'Mar',
    'letter' => 'S',
    'name' => 'EU',
    'offset_from_std' => 3600,
    'on' => 'lastSun',
    'save' => '1:00',
    'to' => 'max',
    'type' => undef
  }, 'DateTime::TimeZone::OlsonDB::Rule' )
]
;
sub _rules { $rules }


1;

