
/*
 Copyright (C) 2003 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "mersennetwister.hpp"
#include <ql/RandomNumbers/mt19937uniformrng.hpp>
#include <ql/dataformatters.hpp>

using namespace QuantLib;
using namespace QuantLib::RandomNumbers;

void MersenneTwisterTest::runTest() {

    // the following numbers are provided by MT authors in order
    // to check any actual implementation of MT
    static const unsigned long referenceLongValues[] = {
        1067595299UL,  955945823UL,  477289528UL, 4107218783UL, 4228976476UL,
        3344332714UL, 3355579695UL,  227628506UL,  810200273UL, 2591290167UL,
        2560260675UL, 3242736208UL,  646746669UL, 1479517882UL, 4245472273UL,
        1143372638UL, 3863670494UL, 3221021970UL, 1773610557UL, 1138697238UL,
        1421897700UL, 1269916527UL, 2859934041UL, 1764463362UL, 3874892047UL,
        3965319921UL,   72549643UL, 2383988930UL, 2600218693UL, 3237492380UL,
        2792901476UL,  725331109UL,  605841842UL,  271258942UL,  715137098UL,
        3297999536UL, 1322965544UL, 4229579109UL, 1395091102UL, 3735697720UL,
        2101727825UL, 3730287744UL, 2950434330UL, 1661921839UL, 2895579582UL,
        2370511479UL, 1004092106UL, 2247096681UL, 2111242379UL, 3237345263UL,
        4082424759UL,  219785033UL, 2454039889UL, 3709582971UL,  835606218UL,
        2411949883UL, 2735205030UL,  756421180UL, 2175209704UL, 1873865952UL,
        2762534237UL, 4161807854UL, 3351099340UL,  181129879UL, 3269891896UL,
         776029799UL, 2218161979UL, 3001745796UL, 1866825872UL, 2133627728UL,
          34862734UL, 1191934573UL, 3102311354UL, 2916517763UL, 1012402762UL,
        2184831317UL, 4257399449UL, 2899497138UL, 3818095062UL, 3030756734UL,
        1282161629UL,  420003642UL, 2326421477UL, 2741455717UL, 1278020671UL,
        3744179621UL,  271777016UL, 2626330018UL, 2560563991UL, 3055977700UL,
        4233527566UL, 1228397661UL, 3595579322UL, 1077915006UL, 2395931898UL,
        1851927286UL, 3013683506UL, 1999971931UL, 3006888962UL, 1049781534UL,
        1488758959UL, 3491776230UL,  104418065UL, 2448267297UL, 3075614115UL,
        3872332600UL,  891912190UL, 3936547759UL, 2269180963UL, 2633455084UL,
        1047636807UL, 2604612377UL, 2709305729UL, 1952216715UL,  207593580UL,
        2849898034UL,  670771757UL, 2210471108UL,  467711165UL,  263046873UL,
        3569667915UL, 1042291111UL, 3863517079UL, 1464270005UL, 2758321352UL,
        3790799816UL, 2301278724UL, 3106281430UL,    7974801UL, 2792461636UL,
         555991332UL,  621766759UL, 1322453093UL,  853629228UL,  686962251UL,
        1455120532UL,  957753161UL, 1802033300UL, 1021534190UL, 3486047311UL,
        1902128914UL, 3701138056UL, 4176424663UL, 1795608698UL,  560858864UL,
        3737752754UL, 3141170998UL, 1553553385UL, 3367807274UL,  711546358UL,
        2475125503UL,  262969859UL,  251416325UL, 2980076994UL, 1806565895UL,
         969527843UL, 3529327173UL, 2736343040UL, 2987196734UL, 1649016367UL,
        2206175811UL, 3048174801UL, 3662503553UL, 3138851612UL, 2660143804UL,
        1663017612UL, 1816683231UL,  411916003UL, 3887461314UL, 2347044079UL,
        1015311755UL, 1203592432UL, 2170947766UL, 2569420716UL,  813872093UL,
        1105387678UL, 1431142475UL,  220570551UL, 4243632715UL, 4179591855UL,
        2607469131UL, 3090613241UL,  282341803UL, 1734241730UL, 1391822177UL,
        1001254810UL,  827927915UL, 1886687171UL, 3935097347UL, 2631788714UL,
        3905163266UL,  110554195UL, 2447955646UL, 3717202975UL, 3304793075UL,
        3739614479UL, 3059127468UL,  953919171UL, 2590123714UL, 1132511021UL,
        3795593679UL, 2788030429UL,  982155079UL, 3472349556UL,  859942552UL,
        2681007391UL, 2299624053UL,  647443547UL,  233600422UL,  608168955UL,
        3689327453UL, 1849778220UL, 1608438222UL, 3968158357UL, 2692977776UL,
        2851872572UL,  246750393UL, 3582818628UL, 3329652309UL, 4036366910UL,
        1012970930UL,  950780808UL, 3959768744UL, 2538550045UL,  191422718UL,
        2658142375UL, 3276369011UL, 2927737484UL, 1234200027UL, 1920815603UL,
        3536074689UL, 1535612501UL, 2184142071UL, 3276955054UL,  428488088UL,
        2378411984UL, 4059769550UL, 3913744741UL, 2732139246UL,   64369859UL,
        3755670074UL,  842839565UL, 2819894466UL, 2414718973UL, 1010060670UL,
        1839715346UL, 2410311136UL,  152774329UL, 3485009480UL, 4102101512UL,
        2852724304UL,  879944024UL, 1785007662UL, 2748284463UL, 1354768064UL,
        3267784736UL, 2269127717UL, 3001240761UL, 3179796763UL,  895723219UL,
         865924942UL, 4291570937UL,   89355264UL, 1471026971UL, 4114180745UL,
        3201939751UL, 2867476999UL, 2460866060UL, 3603874571UL, 2238880432UL,
        3308416168UL, 2072246611UL, 2755653839UL, 3773737248UL, 1709066580UL,
        4282731467UL, 2746170170UL, 2832568330UL,  433439009UL, 3175778732UL,
          26248366UL, 2551382801UL,  183214346UL, 3893339516UL, 1928168445UL,
        1337157619UL, 3429096554UL, 3275170900UL, 1782047316UL, 4264403756UL,
        1876594403UL, 4289659572UL, 3223834894UL, 1728705513UL, 4068244734UL,
        2867840287UL, 1147798696UL,  302879820UL, 1730407747UL, 1923824407UL,
        1180597908UL, 1569786639UL,  198796327UL,  560793173UL, 2107345620UL,
        2705990316UL, 3448772106UL, 3678374155UL,  758635715UL,  884524671UL,
         486356516UL, 1774865603UL, 3881226226UL, 2635213607UL, 1181121587UL,
        1508809820UL, 3178988241UL, 1594193633UL, 1235154121UL,  326117244UL,
        2304031425UL,  937054774UL, 2687415945UL, 3192389340UL, 2003740439UL,
        1823766188UL, 2759543402UL,   10067710UL, 1533252662UL, 4132494984UL,
          82378136UL,  420615890UL, 3467563163UL,  541562091UL, 3535949864UL,
        2277319197UL, 3330822853UL, 3215654174UL, 4113831979UL, 4204996991UL,
        2162248333UL, 3255093522UL, 2219088909UL, 2978279037UL,  255818579UL,
        2859348628UL, 3097280311UL, 2569721123UL, 1861951120UL, 2907080079UL,
        2719467166UL,  998319094UL, 2521935127UL, 2404125338UL,  259456032UL,
        2086860995UL, 1839848496UL, 1893547357UL, 2527997525UL, 1489393124UL,
        2860855349UL,   76448234UL, 2264934035UL,  744914583UL, 2586791259UL,
        1385380501UL,   66529922UL, 1819103258UL, 1899300332UL, 2098173828UL,
        1793831094UL,  276463159UL,  360132945UL, 4178212058UL,  595015228UL,
         177071838UL, 2800080290UL, 1573557746UL, 1548998935UL,  378454223UL,
        1460534296UL, 1116274283UL, 3112385063UL, 3709761796UL,  827999348UL,
        3580042847UL, 1913901014UL,  614021289UL, 4278528023UL, 1905177404UL,
          45407939UL, 3298183234UL, 1184848810UL, 3644926330UL, 3923635459UL,
        1627046213UL, 3677876759UL,  969772772UL, 1160524753UL, 1522441192UL,
         452369933UL, 1527502551UL,  832490847UL, 1003299676UL, 1071381111UL,
        2891255476UL,  973747308UL, 4086897108UL, 1847554542UL, 3895651598UL,
        2227820339UL, 1621250941UL, 2881344691UL, 3583565821UL, 3510404498UL,
         849362119UL,  862871471UL,  797858058UL, 2867774932UL, 2821282612UL,
        3272403146UL, 3997979905UL,  209178708UL, 1805135652UL,    6783381UL,
        2823361423UL,  792580494UL, 4263749770UL,  776439581UL, 3798193823UL,
        2853444094UL, 2729507474UL, 1071873341UL, 1329010206UL, 1289336450UL,
        3327680758UL, 2011491779UL,   80157208UL,  922428856UL, 1158943220UL,
        1667230961UL, 2461022820UL, 2608845159UL,  387516115UL, 3345351910UL,
        1495629111UL, 4098154157UL, 3156649613UL, 3525698599UL, 4134908037UL,
         446713264UL, 2137537399UL, 3617403512UL,  813966752UL, 1157943946UL,
        3734692965UL, 1680301658UL, 3180398473UL, 3509854711UL, 2228114612UL,
        1008102291UL,  486805123UL,  863791847UL, 3189125290UL, 1050308116UL,
        3777341526UL, 4291726501UL,  844061465UL, 1347461791UL, 2826481581UL,
         745465012UL, 2055805750UL, 4260209475UL, 2386693097UL, 2980646741UL,
         447229436UL, 2077782664UL, 1232942813UL, 4023002732UL, 1399011509UL,
        3140569849UL, 2579909222UL, 3794857471UL,  900758066UL, 2887199683UL,
        1720257997UL, 3367494931UL, 2668921229UL,  955539029UL, 3818726432UL,
        1105704962UL, 3889207255UL, 2277369307UL, 2746484505UL, 1761846513UL,
        2413916784UL, 2685127085UL, 4240257943UL, 1166726899UL, 4215215715UL,
        3082092067UL, 3960461946UL, 1663304043UL, 2087473241UL, 4162589986UL,
        2507310778UL, 1579665506UL,  767234210UL,  970676017UL,  492207530UL,
        1441679602UL, 1314785090UL, 3262202570UL, 3417091742UL, 1561989210UL,
        3011406780UL, 1146609202UL, 3262321040UL, 1374872171UL, 1634688712UL,
        1280458888UL, 2230023982UL,  419323804UL, 3262899800UL,   39783310UL,
        1641619040UL, 1700368658UL, 2207946628UL, 2571300939UL, 2424079766UL,
         780290914UL, 2715195096UL, 3390957695UL,  163151474UL, 2309534542UL,
        1860018424UL,  555755123UL,  280320104UL, 1604831083UL, 2713022383UL,
        1728987441UL, 3639955502UL,  623065489UL, 3828630947UL, 4275479050UL,
        3516347383UL, 2343951195UL, 2430677756UL,  635534992UL, 3868699749UL,
         808442435UL, 3070644069UL, 4282166003UL, 2093181383UL, 2023555632UL,
        1568662086UL, 3422372620UL, 4134522350UL, 3016979543UL, 3259320234UL,
        2888030729UL, 3185253876UL, 4258779643UL, 1267304371UL, 1022517473UL,
         815943045UL,  929020012UL, 2995251018UL, 3371283296UL, 3608029049UL,
        2018485115UL,  122123397UL, 2810669150UL, 1411365618UL, 1238391329UL,
        1186786476UL, 3155969091UL, 2242941310UL, 1765554882UL,  279121160UL,
        4279838515UL, 1641578514UL, 3796324015UL,   13351065UL,  103516986UL,
        1609694427UL,  551411743UL, 2493771609UL, 1316337047UL, 3932650856UL,
        4189700203UL,  463397996UL, 2937735066UL, 1855616529UL, 2626847990UL,
          55091862UL, 3823351211UL,  753448970UL, 4045045500UL, 1274127772UL,
        1124182256UL,   92039808UL, 2126345552UL,  425973257UL,  386287896UL,
        2589870191UL, 1987762798UL, 4084826973UL, 2172456685UL, 3366583455UL,
        3602966653UL, 2378803535UL, 2901764433UL, 3716929006UL, 3710159000UL,
        2653449155UL, 3469742630UL, 3096444476UL, 3932564653UL, 2595257433UL,
         318974657UL, 3146202484UL,  853571438UL,  144400272UL, 3768408841UL,
         782634401UL, 2161109003UL,  570039522UL, 1886241521UL,   14249488UL,
        2230804228UL, 1604941699UL, 3928713335UL, 3921942509UL, 2155806892UL,
         134366254UL,  430507376UL, 1924011722UL,  276713377UL,  196481886UL,
        3614810992UL, 1610021185UL, 1785757066UL,  851346168UL, 3761148643UL,
        2918835642UL, 3364422385UL, 3012284466UL, 3735958851UL, 2643153892UL,
        3778608231UL, 1164289832UL,  205853021UL, 2876112231UL, 3503398282UL,
        3078397001UL, 3472037921UL, 1748894853UL, 2740861475UL,  316056182UL,
        1660426908UL,  168885906UL,  956005527UL, 3984354789UL,  566521563UL,
        1001109523UL, 1216710575UL, 2952284757UL, 3834433081UL, 3842608301UL,
        2467352408UL, 3974441264UL, 3256601745UL, 1409353924UL, 1329904859UL,
        2307560293UL, 3125217879UL, 3622920184UL, 3832785684UL, 3882365951UL,
        2308537115UL, 2659155028UL, 1450441945UL, 3532257603UL, 3186324194UL,
        1225603425UL, 1124246549UL,  175808705UL, 3009142319UL, 2796710159UL,
        3651990107UL,  160762750UL, 1902254979UL, 1698648476UL, 1134980669UL,
         497144426UL, 3302689335UL, 4057485630UL, 3603530763UL, 4087252587UL,
         427812652UL,  286876201UL,  823134128UL, 1627554964UL, 3745564327UL,
        2589226092UL, 4202024494UL,   62878473UL, 3275585894UL, 3987124064UL,
        2791777159UL, 1916869511UL, 2585861905UL, 1375038919UL, 1403421920UL,
          60249114UL, 3811870450UL, 3021498009UL, 2612993202UL,  528933105UL,
        2757361321UL, 3341402964UL, 2621861700UL,  273128190UL, 4015252178UL,
        3094781002UL, 1621621288UL, 2337611177UL, 1796718448UL, 1258965619UL,
        4241913140UL, 2138560392UL, 3022190223UL, 4174180924UL,  450094611UL,
        3274724580UL,  617150026UL, 2704660665UL, 1469700689UL, 1341616587UL,
         356715071UL, 1188789960UL, 2278869135UL, 1766569160UL, 2795896635UL,
          57824704UL, 2893496380UL, 1235723989UL, 1630694347UL, 3927960522UL,
         428891364UL, 1814070806UL, 2287999787UL, 4125941184UL, 3968103889UL,
        3548724050UL, 1025597707UL, 1404281500UL, 2002212197UL,   92429143UL,
        2313943944UL, 2403086080UL, 3006180634UL, 3561981764UL, 1671860914UL,
        1768520622UL, 1803542985UL,  844848113UL, 3006139921UL, 1410888995UL,
        1157749833UL, 2125704913UL, 1789979528UL, 1799263423UL,  741157179UL,
        2405862309UL,  767040434UL, 2655241390UL, 3663420179UL, 2172009096UL,
        2511931187UL, 1680542666UL,  231857466UL, 1154981000UL,  157168255UL,
        1454112128UL, 3505872099UL, 1929775046UL, 2309422350UL, 2143329496UL,
        2960716902UL,  407610648UL, 2938108129UL, 2581749599UL,  538837155UL,
        2342628867UL,  430543915UL,  740188568UL, 1937713272UL, 3315215132UL,
        2085587024UL, 4030765687UL,  766054429UL, 3517641839UL,  689721775UL,
        1294158986UL, 1753287754UL, 4202601348UL, 1974852792UL,   33459103UL,
        3568087535UL, 3144677435UL, 1686130825UL, 4134943013UL, 3005738435UL,
        3599293386UL,  426570142UL,  754104406UL, 3660892564UL, 1964545167UL,
         829466833UL,  821587464UL, 1746693036UL, 1006492428UL, 1595312919UL,
        1256599985UL, 1024482560UL, 1897312280UL, 2902903201UL,  691790057UL,
        1037515867UL, 3176831208UL, 1968401055UL, 2173506824UL, 1089055278UL,
        1748401123UL, 2941380082UL,  968412354UL, 1818753861UL, 2973200866UL,
        3875951774UL, 1119354008UL, 3988604139UL, 1647155589UL, 2232450826UL,
        3486058011UL, 3655784043UL, 3759258462UL,  847163678UL, 1082052057UL,
         989516446UL, 2871541755UL, 3196311070UL, 3929963078UL,  658187585UL,
        3664944641UL, 2175149170UL, 2203709147UL, 2756014689UL, 2456473919UL,
        3890267390UL, 1293787864UL, 2830347984UL, 3059280931UL, 4158802520UL,
        1561677400UL, 2586570938UL,  783570352UL, 1355506163UL,   31495586UL,
        3789437343UL, 3340549429UL, 2092501630UL,  896419368UL,  671715824UL,
        3530450081UL, 3603554138UL, 1055991716UL, 3442308219UL, 1499434728UL,
        3130288473UL, 3639507000UL,   17769680UL, 2259741420UL,  487032199UL,
        4227143402UL, 3693771256UL, 1880482820UL, 3924810796UL,  381462353UL,
        4017855991UL, 2452034943UL, 2736680833UL, 2209866385UL, 2128986379UL,
         437874044UL,  595759426UL,  641721026UL, 1636065708UL, 3899136933UL,
         629879088UL, 3591174506UL,  351984326UL, 2638783544UL, 2348444281UL,
        2341604660UL, 2123933692UL,  143443325UL, 1525942256UL,  364660499UL,
         599149312UL,  939093251UL, 1523003209UL,  106601097UL,  376589484UL,
        1346282236UL, 1297387043UL,  764598052UL, 3741218111UL,  933457002UL,
        1886424424UL, 3219631016UL,  525405256UL, 3014235619UL,  323149677UL,
        2038881721UL, 4100129043UL, 2851715101UL, 2984028078UL, 1888574695UL,
        2014194741UL, 3515193880UL, 4180573530UL, 3461824363UL, 2641995497UL,
        3179230245UL, 2902294983UL, 2217320456UL, 4040852155UL, 1784656905UL,
        3311906931UL,   87498458UL, 2752971818UL, 2635474297UL, 2831215366UL,
        3682231106UL, 2920043893UL, 3772929704UL, 2816374944UL,  309949752UL,
        2383758854UL,  154870719UL,  385111597UL, 1191604312UL, 1840700563UL,
         872191186UL, 2925548701UL, 1310412747UL, 2102066999UL, 1504727249UL,
        3574298750UL, 1191230036UL, 3330575266UL, 3180292097UL, 3539347721UL,
         681369118UL, 3305125752UL, 3648233597UL,  950049240UL, 4173257693UL,
        1760124957UL,  512151405UL,  681175196UL,  580563018UL, 1169662867UL,
        4015033554UL, 2687781101UL,  699691603UL, 2673494188UL, 1137221356UL,
         123599888UL,  472658308UL, 1053598179UL, 1012713758UL, 3481064843UL,
        3759461013UL, 3981457956UL, 3830587662UL, 1877191791UL, 3650996736UL,
         988064871UL, 3515461600UL, 4089077232UL, 2225147448UL, 1249609188UL,
        2643151863UL, 3896204135UL, 2416995901UL, 1397735321UL, 3460025646UL
    };

    static const double referenceValues[] = {
        0.76275443, 0.99000644, 0.98670464, 0.10143112, 0.27933125,
        0.69867227, 0.94218740, 0.03427201, 0.78842173, 0.28180608,
        0.92179002, 0.20785655, 0.54534773, 0.69644020, 0.38107718,
        0.23978165, 0.65286910, 0.07514568, 0.22765211, 0.94872929,
        0.74557914, 0.62664415, 0.54708246, 0.90959343, 0.42043116,
        0.86334511, 0.19189126, 0.14718544, 0.70259889, 0.63426346,
        0.77408121, 0.04531601, 0.04605807, 0.88595519, 0.69398270,
        0.05377184, 0.61711170, 0.05565708, 0.10133577, 0.41500776,
        0.91810699, 0.22320679, 0.23353705, 0.92871862, 0.98897234,
        0.19786706, 0.80558809, 0.06961067, 0.55840445, 0.90479405,
        0.63288060, 0.95009721, 0.54948447, 0.20645042, 0.45000959,
        0.87050869, 0.70806991, 0.19406895, 0.79286390, 0.49332866,
        0.78483914, 0.75145146, 0.12341941, 0.42030252, 0.16728160,
        0.59906494, 0.37575460, 0.97815160, 0.39815952, 0.43595080,
        0.04952478, 0.33917805, 0.76509902, 0.61034321, 0.90654701,
        0.92915732, 0.85365931, 0.18812377, 0.65913428, 0.28814566,
        0.59476081, 0.27835931, 0.60722542, 0.68310435, 0.69387186,
        0.03699800, 0.65897714, 0.17527003, 0.02889304, 0.86777366,
        0.12352068, 0.91439461, 0.32022990, 0.44445731, 0.34903686,
        0.74639273, 0.65918367, 0.92492794, 0.31872642, 0.77749724,
        0.85413832, 0.76385624, 0.32744211, 0.91326300, 0.27458185,
        0.22190155, 0.19865383, 0.31227402, 0.85321225, 0.84243342,
        0.78544200, 0.71854080, 0.92503892, 0.82703064, 0.88306297,
        0.47284073, 0.70059042, 0.48003761, 0.38671694, 0.60465770,
        0.41747204, 0.47163243, 0.72750808, 0.65830223, 0.10955369,
        0.64215401, 0.23456345, 0.95944940, 0.72822249, 0.40888451,
        0.69980355, 0.26677428, 0.57333635, 0.39791582, 0.85377858,
        0.76962816, 0.72004885, 0.90903087, 0.51376506, 0.37732665,
        0.12691640, 0.71249738, 0.81217908, 0.37037313, 0.32772374,
        0.14238259, 0.05614811, 0.74363008, 0.39773267, 0.94859135,
        0.31452454, 0.11730313, 0.62962618, 0.33334237, 0.45547255,
        0.10089665, 0.56550662, 0.60539371, 0.16027624, 0.13245301,
        0.60959939, 0.04671662, 0.99356286, 0.57660859, 0.40269560,
        0.45274629, 0.06699735, 0.85064246, 0.87742744, 0.54508392,
        0.87242982, 0.29321385, 0.67660627, 0.68230715, 0.79052073,
        0.48592054, 0.25186266, 0.93769755, 0.28565487, 0.47219067,
        0.99054882, 0.13155240, 0.47110470, 0.98556600, 0.84397623,
        0.12875246, 0.90953202, 0.49129015, 0.23792727, 0.79481194,
        0.44337770, 0.96564297, 0.67749118, 0.55684872, 0.27286897,
        0.79538393, 0.61965356, 0.22487929, 0.02226018, 0.49248200,
        0.42247006, 0.91797788, 0.99250134, 0.23449967, 0.52531508,
        0.10246337, 0.78685622, 0.34310922, 0.89892996, 0.40454552,
        0.68608407, 0.30752487, 0.83601319, 0.54956031, 0.63777550,
        0.82199797, 0.24890696, 0.48801123, 0.48661910, 0.51223987, 
        0.32969635, 0.31075073, 0.21393155, 0.73453207, 0.15565705,
        0.58584522, 0.28976728, 0.97621478, 0.61498701, 0.23891470,
        0.28518540, 0.46809591, 0.18371914, 0.37597910, 0.13492176,
        0.66849449, 0.82811466, 0.56240330, 0.37548956, 0.27562998, 
        0.27521910, 0.74096121, 0.77176757, 0.13748143, 0.99747138, 
        0.92504502, 0.09175241, 0.21389176, 0.21766512, 0.31183245, 
        0.23271221, 0.21207367, 0.57903312, 0.77523344, 0.13242613, 
        0.31037988, 0.01204835, 0.71652949, 0.84487594, 0.14982178, 
        0.57423142, 0.45677888, 0.48420169, 0.53465428, 0.52667473, 
        0.46880526, 0.49849733, 0.05670710, 0.79022476, 0.03872047, 
        0.21697212, 0.20443086, 0.28949326, 0.81678186, 0.87629474, 
        0.92297064, 0.27373097, 0.84625273, 0.51505586, 0.00582792, 
        0.33295971, 0.91848412, 0.92537226, 0.91760033, 0.07541125, 
        0.71745848, 0.61158698, 0.00941650, 0.03135554, 0.71527471, 
        0.24821915, 0.63636652, 0.86159918, 0.26450229, 0.60160194, 
        0.35557725, 0.24477500, 0.07186456, 0.51757096, 0.62120362, 
        0.97981062, 0.69954667, 0.21065616, 0.13382753, 0.27693186, 
        0.59644095, 0.71500764, 0.04110751, 0.95730081, 0.91600724, 
        0.47704678, 0.26183479, 0.34706971, 0.07545431, 0.29398385, 
        0.93236070, 0.60486023, 0.48015011, 0.08870451, 0.45548581, 
        0.91872718, 0.38142712, 0.10668643, 0.01397541, 0.04520355, 
        0.93822273, 0.18011940, 0.57577277, 0.91427606, 0.30911399, 
        0.95853475, 0.23611214, 0.69619891, 0.69601980, 0.76765372, 
        0.58515930, 0.49479057, 0.11288752, 0.97187699, 0.32095365, 
        0.57563608, 0.40760618, 0.78703383, 0.43261152, 0.90877651, 
        0.84686346, 0.10599030, 0.72872803, 0.19315490, 0.66152912, 
        0.10210518, 0.06257876, 0.47950688, 0.47062066, 0.72701157, 
        0.48915116, 0.66110261, 0.60170685, 0.24516994, 0.12726050, 
        0.03451185, 0.90864994, 0.83494878, 0.94800035, 0.91035206, 
        0.14480751, 0.88458997, 0.53498312, 0.15963215, 0.55378627, 
        0.35171349, 0.28719791, 0.09097957, 0.00667896, 0.32309622, 
        0.87561479, 0.42534520, 0.91748977, 0.73908457, 0.41793223, 
        0.99279792, 0.87908370, 0.28458072, 0.59132853, 0.98672190, 
        0.28547393, 0.09452165, 0.89910674, 0.53681109, 0.37931425, 
        0.62683489, 0.56609740, 0.24801549, 0.52948179, 0.98328855, 
        0.66403523, 0.55523786, 0.75886666, 0.84784685, 0.86829981, 
        0.71448906, 0.84670080, 0.43922919, 0.20771016, 0.64157936, 
        0.25664246, 0.73055695, 0.86395782, 0.65852932, 0.99061803, 
        0.40280575, 0.39146298, 0.07291005, 0.97200603, 0.20555729, 
        0.59616495, 0.08138254, 0.45796388, 0.33681125, 0.33989127, 
        0.18717090, 0.53545811, 0.60550838, 0.86520709, 0.34290701, 
        0.72743276, 0.73023855, 0.34195926, 0.65019733, 0.02765254, 
        0.72575740, 0.32709576, 0.03420866, 0.26061893, 0.56997511, 
        0.28439072, 0.84422744, 0.77637570, 0.55982168, 0.06720327, 
        0.58449067, 0.71657369, 0.15819609, 0.58042821, 0.07947911, 
        0.40193792, 0.11376012, 0.88762938, 0.67532159, 0.71223735, 
        0.27829114, 0.04806073, 0.21144026, 0.58830274, 0.04140071, 
        0.43215628, 0.12952729, 0.94668759, 0.87391019, 0.98382450, 
        0.27750768, 0.90849647, 0.90962737, 0.59269720, 0.96102026, 
        0.49544979, 0.32007095, 0.62585546, 0.03119821, 0.85953001, 
        0.22017528, 0.05834068, 0.80731217, 0.53799961, 0.74166948, 
        0.77426600, 0.43938444, 0.54862081, 0.58575513, 0.15886492, 
        0.73214332, 0.11649057, 0.77463977, 0.85788827, 0.17061997, 
        0.66838056, 0.96076133, 0.07949296, 0.68521946, 0.89986254, 
        0.05667410, 0.12741385, 0.83470977, 0.63969104, 0.46612929, 
        0.10200126, 0.01194925, 0.10476340, 0.90285217, 0.31221221, 
        0.32980614, 0.46041971, 0.52024973, 0.05425470, 0.28330912, 
        0.60426543, 0.00598243, 0.97244013, 0.21135841, 0.78561597, 
        0.78428734, 0.63422849, 0.32909934, 0.44771136, 0.27380750, 
        0.14966697, 0.18156268, 0.65686758, 0.28726350, 0.97074787, 
        0.63676171, 0.96649494, 0.24526295, 0.08297372, 0.54257548, 
        0.03166785, 0.33735355, 0.15946671, 0.02102971, 0.46228045, 
        0.11892296, 0.33408336, 0.29875681, 0.29847692, 0.73767569, 
        0.02080745, 0.62980060, 0.08082293, 0.22993106, 0.25031439, 
        0.87787525, 0.45150053, 0.13673441, 0.63407612, 0.97907688, 
        0.52241942, 0.50580158, 0.06273902, 0.05270283, 0.77031811, 
        0.05113352, 0.24393329, 0.75036441, 0.37436336, 0.22877652, 
        0.59975358, 0.85707591, 0.88691457, 0.85547165, 0.36641027, 
        0.58720133, 0.45462835, 0.09243817, 0.32981586, 0.07820411, 
        0.25421519, 0.36004706, 0.60092307, 0.46192412, 0.36758683, 
        0.98424170, 0.08019934, 0.68594024, 0.45826386, 0.29962317, 
        0.79365413, 0.89231296, 0.49478547, 0.87645944, 0.23590734, 
        0.28106737, 0.75026285, 0.08136314, 0.79582424, 0.76010628, 
        0.82792971, 0.27947652, 0.72482861, 0.82191216, 0.46171689, 
        0.79189752, 0.96043686, 0.51609668, 0.88995725, 0.28998963, 
        0.55191845, 0.03934737, 0.83033700, 0.49553013, 0.98009549, 
        0.19017594, 0.98347750, 0.33452066, 0.87144372, 0.72106301, 
        0.71272114, 0.71465963, 0.88361677, 0.85571283, 0.73782329, 
        0.20920458, 0.34855153, 0.46766817, 0.02780062, 0.74898344, 
        0.03680650, 0.44866557, 0.77426312, 0.91025891, 0.25195236, 
        0.87319953, 0.63265037, 0.25552148, 0.27422476, 0.95217406, 
        0.39281839, 0.66441573, 0.09158900, 0.94515992, 0.07800798, 
        0.02507888, 0.39901462, 0.17382573, 0.12141278, 0.85502334, 
        0.19902911, 0.02160210, 0.44460522, 0.14688742, 0.68020336, 
        0.71323733, 0.60922473, 0.95400380, 0.99611159, 0.90897777, 
        0.41073520, 0.66206647, 0.32064685, 0.62805003, 0.50677209, 
        0.52690101, 0.87473387, 0.73918362, 0.39826974, 0.43683919, 
        0.80459118, 0.32422684, 0.01958019, 0.95319576, 0.98326137, 
        0.83931735, 0.69060863, 0.33671416, 0.68062550, 0.65152380, 
        0.33392969, 0.03451730, 0.95227244, 0.68200635, 0.85074171, 
        0.64721009, 0.51234433, 0.73402047, 0.00969637, 0.93835057, 
        0.80803854, 0.31485260, 0.20089527, 0.01323282, 0.59933780, 
        0.31584602, 0.20209563, 0.33754800, 0.68604181, 0.24443049, 
        0.19952227, 0.78162632, 0.10336988, 0.11360736, 0.23536740, 
        0.23262256, 0.67803776, 0.48749791, 0.74658435, 0.92156640, 
        0.56706407, 0.36683221, 0.99157136, 0.23421374, 0.45183767, 
        0.91609720, 0.85573315, 0.37706276, 0.77042618, 0.30891908, 
        0.40709595, 0.06944866, 0.61342849, 0.88817388, 0.58734506, 
        0.98711323, 0.14744128, 0.63242656, 0.87704136, 0.68347125, 
        0.84446569, 0.43265239, 0.25146321, 0.04130111, 0.34259839, 
        0.92697368, 0.40878778, 0.56990338, 0.76204273, 0.19820348, 
        0.66314909, 0.02482844, 0.06669207, 0.50205581, 0.26084093, 
        0.65139159, 0.41650223, 0.09733904, 0.56344203, 0.62651696, 
        0.67332139, 0.58037374, 0.47258086, 0.21010758, 0.05713135, 
        0.89390629, 0.10781246, 0.32037450, 0.07628388, 0.34227964, 
        0.42190597, 0.58201860, 0.77363549, 0.49595133, 0.86031236, 
        0.83906769, 0.81098161, 0.26694195, 0.14215941, 0.88210306, 
        0.53634237, 0.12090720, 0.82480459, 0.75930318, 0.31847147, 
        0.92768077, 0.01037616, 0.56201727, 0.88107122, 0.35925856, 
        0.85860762, 0.61109408, 0.70408301, 0.58434977, 0.92192494, 
        0.62667915, 0.75988365, 0.06858761, 0.36156496, 0.58057195, 
        0.13636150, 0.57719713, 0.59340255, 0.63530602, 0.22976282, 
        0.71915530, 0.41162531, 0.63979565, 0.09931342, 0.79344045, 
        0.10893790, 0.84450224, 0.23122236, 0.99485593, 0.73637397, 
        0.17276368, 0.13357764, 0.74965804, 0.64991737, 0.61990341, 
        0.41523170, 0.05878239, 0.05687301, 0.05497131, 0.42868366, 
        0.42571090, 0.25810502, 0.89642955, 0.30439758, 0.39310223, 
        0.11357431, 0.04288255, 0.23397550, 0.11200634, 0.85621396, 
        0.89733974, 0.37508865, 0.42077265, 0.68597384, 0.72781399, 
        0.19296476, 0.61699087, 0.31667128, 0.67756410, 0.00177323, 
        0.05725176, 0.79474693, 0.18885238, 0.06724856, 0.68193156, 
        0.42202167, 0.22082041, 0.28554673, 0.64995708, 0.87851940, 
        0.29124547, 0.61009521, 0.87374537, 0.05743712, 0.69902994, 
        0.81925115, 0.45653873, 0.37236821, 0.31118709, 0.52734307, 
        0.39672836, 0.38185294, 0.30163915, 0.17374510, 0.04913278, 
        0.90404879, 0.25742801, 0.58266467, 0.97663209, 0.79823377, 
        0.36437958, 0.15206043, 0.26529938, 0.22690047, 0.05839021, 
        0.84721160, 0.18622435, 0.37809403, 0.55706977, 0.49828704, 
        0.47659049, 0.24289680, 0.88477595, 0.07807463, 0.56245739, 
        0.73490635, 0.21099431, 0.13164942, 0.75840044, 0.66877037, 
        0.28988183, 0.44046090, 0.24967434, 0.80048356, 0.26029740, 
        0.30416821, 0.64151867, 0.52067892, 0.12880774, 0.85465381, 
        0.02690525, 0.19149288, 0.49630295, 0.79682619, 0.43566145, 
        0.00288078, 0.81484193, 0.03763639, 0.68529083, 0.01339574, 
        0.38405386, 0.30537067, 0.22994703, 0.44000045, 0.27217985, 
        0.53831243, 0.02870435, 0.86282045, 0.61831306, 0.09164956, 
        0.25609707, 0.07445781, 0.72185784, 0.90058883, 0.30070608, 
        0.94476583, 0.56822213, 0.21933909, 0.96772793, 0.80063440, 
        0.26307906, 0.31183306, 0.16501252, 0.55436179, 0.68562285, 
        0.23829083, 0.86511559, 0.57868991, 0.81888344, 0.20126869, 
        0.93172350, 0.66028129, 0.21786948, 0.78515828, 0.10262106, 
        0.35390326, 0.79303876, 0.63427924, 0.90479631, 0.31024934, 
        0.60635447, 0.56198079, 0.63573813, 0.91854197, 0.99701497, 
        0.83085849, 0.31692291, 0.01925964, 0.97446405, 0.98751283, 
        0.60944293, 0.13751018, 0.69519957, 0.68956636, 0.56969015, 
        0.46440193, 0.88341765, 0.36754434, 0.89223647, 0.39786427, 
        0.85055280, 0.12749961, 0.79452122, 0.89449784, 0.14567830, 
        0.45716830, 0.74822309, 0.28200437, 0.42546044, 0.17464886, 
        0.68308746, 0.65496587, 0.52935411, 0.12736159, 0.61523955, 
        0.81590528, 0.63107864, 0.39786553, 0.20102294, 0.53292914, 
        0.75485590, 0.59847044, 0.32861691, 0.12125866, 0.58917183, 
        0.07638293, 0.86845380, 0.29192617, 0.03989733, 0.52180460, 
        0.32503407, 0.64071852, 0.69516575, 0.74254998, 0.54587026, 
        0.48713246, 0.32920155, 0.08719954, 0.63497059, 0.54328459, 
        0.64178757, 0.45583809, 0.70694291, 0.85212760, 0.86074305, 
        0.33163422, 0.85739792, 0.59908488, 0.74566046, 0.72157152
    };

    int i;
    std::vector<unsigned long> init(4);
    init[0]=0x123;
    init[1]=0x234;
    init[2]=0x345;
    init[3]=0x456;
    MersenneTwisterUniformRng mt19937(init);

    for (i=0; i<1000; i++) {
        if (referenceLongValues[i] != mt19937.nextInt32()) {
            CPPUNIT_FAIL("Mersenne Twister test failed at index "
                         + IntegerFormatter::toString(i));
        }
    }
    for (i=0; i<1000; i++) {
        double e = QL_FABS(referenceValues[i] -
                           mt19937.next().value);
        if (e > 1.0e-8) {
            CPPUNIT_FAIL("Mersenne Twister test failed at index: "
                         + IntegerFormatter::toString(i) + "\n"
                         "error: "
                         + DoubleFormatter::toExponential(e,2));
        }
    }

}

