# This file is auto-generated by the Perl DateTime Suite time zone
# code generator (0.07) This code generator comes with the
# DateTime::TimeZone module distribution in the tools/ directory

#
# Generated from /tmp/hraLGAMw6M/northamerica.  Olson data version 2011h
#
# Do not edit this file directly.
#
package DateTime::TimeZone::America::St_Johns;
BEGIN {
  $DateTime::TimeZone::America::St_Johns::VERSION = '1.35';
}

use strict;

use Class::Singleton;
use DateTime::TimeZone;
use DateTime::TimeZone::OlsonDB;

@DateTime::TimeZone::America::St_Johns::ISA = ( 'Class::Singleton', 'DateTime::TimeZone' );

my $spans =
[
    [
DateTime::TimeZone::NEG_INFINITY,
59421785452,
DateTime::TimeZone::NEG_INFINITY,
59421772800,
-12652,
0,
'LMT'
    ],
    [
59421785452,
60471552652,
59421772800,
60471540000,
-12652,
0,
'NST'
    ],
    [
60471552652,
60485545852,
60471543600,
60485536800,
-9052,
1,
'NDT'
    ],
    [
60485545852,
60494700652,
60485533200,
60494688000,
-12652,
0,
'NST'
    ],
    [
60494700652,
60503607052,
60494688000,
60503594400,
-12652,
0,
'NST'
    ],
    [
60503607052,
60520883452,
60503598000,
60520874400,
-9052,
1,
'NDT'
    ],
    [
60520883452,
60526236652,
60520870800,
60526224000,
-12652,
0,
'NST'
    ],
    [
60526236652,
60537033052,
60526224000,
60537020400,
-12652,
0,
'NST'
    ],
    [
60537033052,
60545583052,
60537024000,
60545574000,
-9052,
1,
'NDT'
    ],
    [
60545583052,
60568396252,
60545570400,
60568383600,
-12652,
0,
'NST'
    ],
    [
60568396252,
60584117452,
60568387200,
60584108400,
-9052,
1,
'NDT'
    ],
    [
60584117452,
60599845852,
60584104800,
60599833200,
-12652,
0,
'NST'
    ],
    [
60599845852,
60615567052,
60599836800,
60615558000,
-9052,
1,
'NDT'
    ],
    [
60615567052,
60631900252,
60615554400,
60631887600,
-12652,
0,
'NST'
    ],
    [
60631900252,
60647016652,
60631891200,
60647007600,
-9052,
1,
'NDT'
    ],
    [
60647016652,
60663349852,
60647004000,
60663337200,
-12652,
0,
'NST'
    ],
    [
60663349852,
60678466252,
60663340800,
60678457200,
-9052,
1,
'NDT'
    ],
    [
60678466252,
60694799452,
60678453600,
60694786800,
-12652,
0,
'NST'
    ],
    [
60694799452,
60709915852,
60694790400,
60709906800,
-9052,
1,
'NDT'
    ],
    [
60709915852,
60726249052,
60709903200,
60726236400,
-12652,
0,
'NST'
    ],
    [
60726249052,
60741365452,
60726240000,
60741356400,
-9052,
1,
'NDT'
    ],
    [
60741365452,
60757698652,
60741352800,
60757686000,
-12652,
0,
'NST'
    ],
    [
60757698652,
60773419852,
60757689600,
60773410800,
-9052,
1,
'NDT'
    ],
    [
60773419852,
60789148252,
60773407200,
60789135600,
-12652,
0,
'NST'
    ],
    [
60789148252,
60804869452,
60789139200,
60804860400,
-9052,
1,
'NDT'
    ],
    [
60804869452,
60821202652,
60804856800,
60821190000,
-12652,
0,
'NST'
    ],
    [
60821202652,
60836319052,
60821193600,
60836310000,
-9052,
1,
'NDT'
    ],
    [
60836319052,
60852652252,
60836306400,
60852639600,
-12652,
0,
'NST'
    ],
    [
60852652252,
60867768652,
60852643200,
60867759600,
-9052,
1,
'NDT'
    ],
    [
60867768652,
60884101852,
60867756000,
60884089200,
-12652,
0,
'NST'
    ],
    [
60884101852,
60899218252,
60884092800,
60899209200,
-9052,
1,
'NDT'
    ],
    [
60899218252,
60915551452,
60899205600,
60915538800,
-12652,
0,
'NST'
    ],
    [
60915551452,
60930667852,
60915542400,
60930658800,
-9052,
1,
'NDT'
    ],
    [
60930667852,
60947001052,
60930655200,
60946988400,
-12652,
0,
'NST'
    ],
    [
60947001052,
60962722252,
60946992000,
60962713200,
-9052,
1,
'NDT'
    ],
    [
60962722252,
60979055452,
60962709600,
60979042800,
-12652,
0,
'NST'
    ],
    [
60979055452,
60994171852,
60979046400,
60994162800,
-9052,
1,
'NDT'
    ],
    [
60994171852,
61010505052,
60994159200,
61010492400,
-12652,
0,
'NST'
    ],
    [
61010505052,
61025621452,
61010496000,
61025612400,
-9052,
1,
'NDT'
    ],
    [
61025621452,
61038761452,
61025608800,
61038748800,
-12652,
0,
'NST'
    ],
    [
61038761452,
61041954600,
61038748852,
61041942000,
-12600,
0,
'NST'
    ],
    [
61041954600,
61057071000,
61041945600,
61057062000,
-9000,
1,
'NDT'
    ],
    [
61057071000,
61074012600,
61057058400,
61074000000,
-12600,
0,
'NST'
    ],
    [
61074012600,
61086709800,
61074003600,
61086700800,
-9000,
1,
'NDT'
    ],
    [
61086709800,
61105462200,
61086697200,
61105449600,
-12600,
0,
'NST'
    ],
    [
61105462200,
61118159400,
61105453200,
61118150400,
-9000,
1,
'NDT'
    ],
    [
61118159400,
61136911800,
61118146800,
61136899200,
-12600,
0,
'NST'
    ],
    [
61136911800,
61149609000,
61136902800,
61149600000,
-9000,
1,
'NDT'
    ],
    [
61149609000,
61168966200,
61149596400,
61168953600,
-12600,
0,
'NST'
    ],
    [
61168966200,
61181058600,
61168957200,
61181049600,
-9000,
1,
'NDT'
    ],
    [
61181058600,
61200415800,
61181046000,
61200403200,
-12600,
0,
'NST'
    ],
    [
61200415800,
61213113000,
61200406800,
61213104000,
-9000,
1,
'NDT'
    ],
    [
61213113000,
61231865400,
61213100400,
61231852800,
-12600,
0,
'NST'
    ],
    [
61231865400,
61244562600,
61231856400,
61244553600,
-9000,
1,
'NDT'
    ],
    [
61244562600,
61263315000,
61244550000,
61263302400,
-12600,
0,
'NST'
    ],
    [
61263315000,
61366287600,
61263306000,
61366278600,
-9000,
1,
'NWT'
    ],
    [
61366287600,
61370281800,
61366278600,
61370272800,
-9000,
1,
'NPT'
    ],
    [
61370281800,
61378313400,
61370269200,
61378300800,
-12600,
0,
'NST'
    ],
    [
61378313400,
61389639000,
61378300800,
61389626400,
-12600,
0,
'NST'
    ],
    [
61389639000,
61402336200,
61389630000,
61402327200,
-9000,
1,
'NDT'
    ],
    [
61402336200,
61421088600,
61402323600,
61421076000,
-12600,
0,
'NST'
    ],
    [
61421088600,
61433785800,
61421079600,
61433776800,
-9000,
1,
'NDT'
    ],
    [
61433785800,
61452538200,
61433773200,
61452525600,
-12600,
0,
'NST'
    ],
    [
61452538200,
61465235400,
61452529200,
61465226400,
-9000,
1,
'NDT'
    ],
    [
61465235400,
61483987800,
61465222800,
61483975200,
-12600,
0,
'NST'
    ],
    [
61483987800,
61496685000,
61483978800,
61496676000,
-9000,
1,
'NDT'
    ],
    [
61496685000,
61516042200,
61496672400,
61516029600,
-12600,
0,
'NST'
    ],
    [
61516042200,
61528739400,
61516033200,
61528730400,
-9000,
1,
'NDT'
    ],
    [
61528739400,
61546282200,
61528726800,
61546269600,
-12600,
0,
'NST'
    ],
    [
61546282200,
61559584200,
61546273200,
61559575200,
-9000,
1,
'NDT'
    ],
    [
61559584200,
61577731800,
61559571600,
61577719200,
-12600,
0,
'NST'
    ],
    [
61577731800,
61591033800,
61577722800,
61591024800,
-9000,
1,
'NDT'
    ],
    [
61591033800,
61609181400,
61591021200,
61609168800,
-12600,
0,
'NST'
    ],
    [
61609181400,
61622483400,
61609172400,
61622474400,
-9000,
1,
'NDT'
    ],
    [
61622483400,
61640631000,
61622470800,
61640618400,
-12600,
0,
'NST'
    ],
    [
61640631000,
61653933000,
61640622000,
61653924000,
-9000,
1,
'NDT'
    ],
    [
61653933000,
61672080600,
61653920400,
61672068000,
-12600,
0,
'NST'
    ],
    [
61672080600,
61685382600,
61672071600,
61685373600,
-9000,
1,
'NDT'
    ],
    [
61685382600,
61704135000,
61685370000,
61704122400,
-12600,
0,
'NST'
    ],
    [
61704135000,
61717437000,
61704126000,
61717428000,
-9000,
1,
'NDT'
    ],
    [
61717437000,
61735584600,
61717424400,
61735572000,
-12600,
0,
'NST'
    ],
    [
61735584600,
61748886600,
61735575600,
61748877600,
-9000,
1,
'NDT'
    ],
    [
61748886600,
61767034200,
61748874000,
61767021600,
-12600,
0,
'NST'
    ],
    [
61767034200,
61780336200,
61767025200,
61780327200,
-9000,
1,
'NDT'
    ],
    [
61780336200,
61798483800,
61780323600,
61798471200,
-12600,
0,
'NST'
    ],
    [
61798483800,
61811785800,
61798474800,
61811776800,
-9000,
1,
'NDT'
    ],
    [
61811785800,
61829933400,
61811773200,
61829920800,
-12600,
0,
'NST'
    ],
    [
61829933400,
61846259400,
61829924400,
61846250400,
-9000,
1,
'NDT'
    ],
    [
61846259400,
61861987800,
61846246800,
61861975200,
-12600,
0,
'NST'
    ],
    [
61861987800,
61877709000,
61861978800,
61877700000,
-9000,
1,
'NDT'
    ],
    [
61877709000,
61893437400,
61877696400,
61893424800,
-12600,
0,
'NST'
    ],
    [
61893437400,
61909158600,
61893428400,
61909149600,
-9000,
1,
'NDT'
    ],
    [
61909158600,
61924887000,
61909146000,
61924874400,
-12600,
0,
'NST'
    ],
    [
61924887000,
61940608200,
61924878000,
61940599200,
-9000,
1,
'NDT'
    ],
    [
61940608200,
61956336600,
61940595600,
61956324000,
-12600,
0,
'NST'
    ],
    [
61956336600,
61972057800,
61956327600,
61972048800,
-9000,
1,
'NDT'
    ],
    [
61972057800,
61987786200,
61972045200,
61987773600,
-12600,
0,
'NST'
    ],
    [
61987786200,
62004112200,
61987777200,
62004103200,
-9000,
1,
'NDT'
    ],
    [
62004112200,
62019235800,
62004099600,
62019223200,
-12600,
0,
'NST'
    ],
    [
62019235800,
62035561800,
62019226800,
62035552800,
-9000,
1,
'NDT'
    ],
    [
62035561800,
62051290200,
62035549200,
62051277600,
-12600,
0,
'NST'
    ],
    [
62051290200,
62067011400,
62051281200,
62067002400,
-9000,
1,
'NDT'
    ],
    [
62067011400,
62082739800,
62066998800,
62082727200,
-12600,
0,
'NST'
    ],
    [
62082739800,
62098461000,
62082730800,
62098452000,
-9000,
1,
'NDT'
    ],
    [
62098461000,
62114189400,
62098448400,
62114176800,
-12600,
0,
'NST'
    ],
    [
62114189400,
62129910600,
62114180400,
62129901600,
-9000,
1,
'NDT'
    ],
    [
62129910600,
62145639000,
62129898000,
62145626400,
-12600,
0,
'NST'
    ],
    [
62145639000,
62161360200,
62145630000,
62161351200,
-9000,
1,
'NDT'
    ],
    [
62161360200,
62177088600,
62161347600,
62177076000,
-12600,
0,
'NST'
    ],
    [
62177088600,
62193414600,
62177079600,
62193405600,
-9000,
1,
'NDT'
    ],
    [
62193414600,
62209143000,
62193402000,
62209130400,
-12600,
0,
'NST'
    ],
    [
62209143000,
62224864200,
62209134000,
62224855200,
-9000,
1,
'NDT'
    ],
    [
62224864200,
62240592600,
62224851600,
62240580000,
-12600,
0,
'NST'
    ],
    [
62240592600,
62256313800,
62240583600,
62256304800,
-9000,
1,
'NDT'
    ],
    [
62256313800,
62272042200,
62256301200,
62272029600,
-12600,
0,
'NST'
    ],
    [
62272042200,
62287763400,
62272033200,
62287754400,
-9000,
1,
'NDT'
    ],
    [
62287763400,
62303491800,
62287750800,
62303479200,
-12600,
0,
'NST'
    ],
    [
62303491800,
62319213000,
62303482800,
62319204000,
-9000,
1,
'NDT'
    ],
    [
62319213000,
62334941400,
62319200400,
62334928800,
-12600,
0,
'NST'
    ],
    [
62334941400,
62351267400,
62334932400,
62351258400,
-9000,
1,
'NDT'
    ],
    [
62351267400,
62366391000,
62351254800,
62366378400,
-12600,
0,
'NST'
    ],
    [
62366391000,
62382717000,
62366382000,
62382708000,
-9000,
1,
'NDT'
    ],
    [
62382717000,
62398445400,
62382704400,
62398432800,
-12600,
0,
'NST'
    ],
    [
62398445400,
62414166600,
62398436400,
62414157600,
-9000,
1,
'NDT'
    ],
    [
62414166600,
62429895000,
62414154000,
62429882400,
-12600,
0,
'NST'
    ],
    [
62429895000,
62445616200,
62429886000,
62445607200,
-9000,
1,
'NDT'
    ],
    [
62445616200,
62461344600,
62445603600,
62461332000,
-12600,
0,
'NST'
    ],
    [
62461344600,
62477065800,
62461335600,
62477056800,
-9000,
1,
'NDT'
    ],
    [
62477065800,
62492794200,
62477053200,
62492781600,
-12600,
0,
'NST'
    ],
    [
62492794200,
62508515400,
62492785200,
62508506400,
-9000,
1,
'NDT'
    ],
    [
62508515400,
62524243800,
62508502800,
62524231200,
-12600,
0,
'NST'
    ],
    [
62524243800,
62540569800,
62524234800,
62540560800,
-9000,
1,
'NDT'
    ],
    [
62540569800,
62555693400,
62540557200,
62555680800,
-12600,
0,
'NST'
    ],
    [
62555693400,
62572019400,
62555684400,
62572010400,
-9000,
1,
'NDT'
    ],
    [
62572019400,
62587747800,
62572006800,
62587735200,
-12600,
0,
'NST'
    ],
    [
62587747800,
62603469000,
62587738800,
62603460000,
-9000,
1,
'NDT'
    ],
    [
62603469000,
62619197400,
62603456400,
62619184800,
-12600,
0,
'NST'
    ],
    [
62619197400,
62634918600,
62619188400,
62634909600,
-9000,
1,
'NDT'
    ],
    [
62634918600,
62650647000,
62634906000,
62650634400,
-12600,
0,
'NST'
    ],
    [
62650647000,
62666368200,
62650638000,
62666359200,
-9000,
1,
'NDT'
    ],
    [
62666368200,
62680275060,
62666355600,
62680262460,
-12600,
0,
'NST'
    ],
    [
62680275060,
62697810660,
62680266060,
62697801660,
-9000,
1,
'NDT'
    ],
    [
62697810660,
62711724660,
62697798060,
62711712060,
-12600,
0,
'NST'
    ],
    [
62711724660,
62729861460,
62711719260,
62729856060,
-5400,
1,
'NDDT'
    ],
    [
62729861460,
62743174260,
62729848860,
62743161660,
-12600,
0,
'NST'
    ],
    [
62743174260,
62761314660,
62743165260,
62761305660,
-9000,
1,
'NDT'
    ],
    [
62761314660,
62774623860,
62761302060,
62774611260,
-12600,
0,
'NST'
    ],
    [
62774623860,
62792764260,
62774614860,
62792755260,
-9000,
1,
'NDT'
    ],
    [
62792764260,
62806678260,
62792751660,
62806665660,
-12600,
0,
'NST'
    ],
    [
62806678260,
62824213860,
62806669260,
62824204860,
-9000,
1,
'NDT'
    ],
    [
62824213860,
62838127860,
62824201260,
62838115260,
-12600,
0,
'NST'
    ],
    [
62838127860,
62855663460,
62838118860,
62855654460,
-9000,
1,
'NDT'
    ],
    [
62855663460,
62869577460,
62855650860,
62869564860,
-12600,
0,
'NST'
    ],
    [
62869577460,
62887717860,
62869568460,
62887708860,
-9000,
1,
'NDT'
    ],
    [
62887717860,
62901027060,
62887705260,
62901014460,
-12600,
0,
'NST'
    ],
    [
62901027060,
62919167460,
62901018060,
62919158460,
-9000,
1,
'NDT'
    ],
    [
62919167460,
62932476660,
62919154860,
62932464060,
-12600,
0,
'NST'
    ],
    [
62932476660,
62950617060,
62932467660,
62950608060,
-9000,
1,
'NDT'
    ],
    [
62950617060,
62964531060,
62950604460,
62964518460,
-12600,
0,
'NST'
    ],
    [
62964531060,
62982066660,
62964522060,
62982057660,
-9000,
1,
'NDT'
    ],
    [
62982066660,
62995980660,
62982054060,
62995968060,
-12600,
0,
'NST'
    ],
    [
62995980660,
63013516260,
62995971660,
63013507260,
-9000,
1,
'NDT'
    ],
    [
63013516260,
63027430260,
63013503660,
63027417660,
-12600,
0,
'NST'
    ],
    [
63027430260,
63044965860,
63027421260,
63044956860,
-9000,
1,
'NDT'
    ],
    [
63044965860,
63058879860,
63044953260,
63058867260,
-12600,
0,
'NST'
    ],
    [
63058879860,
63077020260,
63058870860,
63077011260,
-9000,
1,
'NDT'
    ],
    [
63077020260,
63090329460,
63077007660,
63090316860,
-12600,
0,
'NST'
    ],
    [
63090329460,
63108469860,
63090320460,
63108460860,
-9000,
1,
'NDT'
    ],
    [
63108469860,
63121779060,
63108457260,
63121766460,
-12600,
0,
'NST'
    ],
    [
63121779060,
63139919460,
63121770060,
63139910460,
-9000,
1,
'NDT'
    ],
    [
63139919460,
63153833460,
63139906860,
63153820860,
-12600,
0,
'NST'
    ],
    [
63153833460,
63171369060,
63153824460,
63171360060,
-9000,
1,
'NDT'
    ],
    [
63171369060,
63185283060,
63171356460,
63185270460,
-12600,
0,
'NST'
    ],
    [
63185283060,
63202818660,
63185274060,
63202809660,
-9000,
1,
'NDT'
    ],
    [
63202818660,
63216732660,
63202806060,
63216720060,
-12600,
0,
'NST'
    ],
    [
63216732660,
63234873060,
63216723660,
63234864060,
-9000,
1,
'NDT'
    ],
    [
63234873060,
63248182260,
63234860460,
63248169660,
-12600,
0,
'NST'
    ],
    [
63248182260,
63266322660,
63248173260,
63266313660,
-9000,
1,
'NDT'
    ],
    [
63266322660,
63279631860,
63266310060,
63279619260,
-12600,
0,
'NST'
    ],
    [
63279631860,
63297772260,
63279622860,
63297763260,
-9000,
1,
'NDT'
    ],
    [
63297772260,
63309267060,
63297759660,
63309254460,
-12600,
0,
'NST'
    ],
    [
63309267060,
63329826660,
63309258060,
63329817660,
-9000,
1,
'NDT'
    ],
    [
63329826660,
63340716660,
63329814060,
63340704060,
-12600,
0,
'NST'
    ],
    [
63340716660,
63361276260,
63340707660,
63361267260,
-9000,
1,
'NDT'
    ],
    [
63361276260,
63372166260,
63361263660,
63372153660,
-12600,
0,
'NST'
    ],
    [
63372166260,
63392725860,
63372157260,
63392716860,
-9000,
1,
'NDT'
    ],
    [
63392725860,
63404220660,
63392713260,
63404208060,
-12600,
0,
'NST'
    ],
    [
63404220660,
63424780260,
63404211660,
63424771260,
-9000,
1,
'NDT'
    ],
    [
63424780260,
63435670260,
63424767660,
63435657660,
-12600,
0,
'NST'
    ],
    [
63435670260,
63456229860,
63435661260,
63456220860,
-9000,
1,
'NDT'
    ],
    [
63456229860,
63467119860,
63456217260,
63467107260,
-12600,
0,
'NST'
    ],
    [
63467119860,
63487679460,
63467110860,
63487670460,
-9000,
1,
'NDT'
    ],
    [
63487679460,
63498569460,
63487666860,
63498556860,
-12600,
0,
'NST'
    ],
    [
63498569460,
63519129060,
63498560460,
63519120060,
-9000,
1,
'NDT'
    ],
    [
63519129060,
63530019060,
63519116460,
63530006460,
-12600,
0,
'NST'
    ],
    [
63530019060,
63550578660,
63530010060,
63550569660,
-9000,
1,
'NDT'
    ],
    [
63550578660,
63561468660,
63550566060,
63561456060,
-12600,
0,
'NST'
    ],
    [
63561468660,
63582028260,
63561459660,
63582019260,
-9000,
1,
'NDT'
    ],
    [
63582028260,
63593523060,
63582015660,
63593510460,
-12600,
0,
'NST'
    ],
    [
63593523060,
63614082660,
63593514060,
63614073660,
-9000,
1,
'NDT'
    ],
    [
63614082660,
63624972660,
63614070060,
63624960060,
-12600,
0,
'NST'
    ],
    [
63624972660,
63645532260,
63624963660,
63645523260,
-9000,
1,
'NDT'
    ],
    [
63645532260,
63656422260,
63645519660,
63656409660,
-12600,
0,
'NST'
    ],
    [
63656422260,
63676981860,
63656413260,
63676972860,
-9000,
1,
'NDT'
    ],
    [
63676981860,
63687871860,
63676969260,
63687859260,
-12600,
0,
'NST'
    ],
    [
63687871860,
63708431460,
63687862860,
63708422460,
-9000,
1,
'NDT'
    ],
    [
63708431460,
63719321460,
63708418860,
63719308860,
-12600,
0,
'NST'
    ],
    [
63719321460,
63739881060,
63719312460,
63739872060,
-9000,
1,
'NDT'
    ],
    [
63739881060,
63751375860,
63739868460,
63751363260,
-12600,
0,
'NST'
    ],
    [
63751375860,
63771935460,
63751366860,
63771926460,
-9000,
1,
'NDT'
    ],
    [
63771935460,
63782825460,
63771922860,
63782812860,
-12600,
0,
'NST'
    ],
    [
63782825460,
63803385060,
63782816460,
63803376060,
-9000,
1,
'NDT'
    ],
];

sub olson_version { '2011h' }

sub has_dst_changes { 104 }

sub _max_year { 2021 }

sub _new_instance
{
    return shift->_init( @_, spans => $spans );
}

sub _last_offset { -12600 }

my $last_observance = bless( {
  'format' => 'N%sT',
  'gmtoff' => '-3:30',
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
  'offset_from_utc' => -12600,
  'until' => [],
  'utc_start_datetime' => bless( {
    'formatter' => undef,
    'local_rd_days' => 710397,
    'local_rd_secs' => 12600,
    'offset_modifier' => 0,
    'rd_nanosecs' => 0,
    'tz' => bless( {
      'name' => 'floating',
      'offset' => 0
    }, 'DateTime::TimeZone::Floating' ),
    'utc_rd_days' => 710397,
    'utc_rd_secs' => 12600,
    'utc_year' => 1947
  }, 'DateTime' )
}, 'DateTime::TimeZone::OlsonDB::Observance' )
;
sub _last_observance { $last_observance }

my $rules = [
  bless( {
    'at' => '0:01',
    'from' => '2007',
    'in' => 'Mar',
    'letter' => 'D',
    'name' => 'StJohns',
    'offset_from_std' => 3600,
    'on' => 'Sun>=8',
    'save' => '1:00',
    'to' => 'max',
    'type' => undef
  }, 'DateTime::TimeZone::OlsonDB::Rule' ),
  bless( {
    'at' => '0:01',
    'from' => '2007',
    'in' => 'Nov',
    'letter' => 'S',
    'name' => 'StJohns',
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

