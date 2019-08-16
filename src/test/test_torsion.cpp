#include "files.h"
#include "test.h"
#include "test_rt.h"

using namespace TINKER_NAMESPACE;

static const char* torsionterm_only = R"**(
torsionterm  only
)**";

static int usage = calc::xyz | calc::vmask;

static const double ref_g_tors_trpcage[][3] = {
    {-0.2438, -0.1946, 0.0309},  {-3.5996, 1.6062, 4.2184},
    {0.3831, -3.9719, -0.2718},  {0.1336, 1.8115, -1.0857},
    {0.7125, 1.0607, -0.1721},   {0.0580, -0.0080, 0.0017},
    {0.2593, -0.5365, 0.0906},   {0.9229, 0.6338, -1.0976},
    {4.6461, -4.1852, -2.0006},  {-1.7624, 2.0261, 0.6045},
    {0.1441, 0.1300, -0.0166},   {-1.3917, -1.1861, 0.4378},
    {-1.2920, 0.0092, -0.6700},  {0.0318, 0.9063, 0.9351},
    {0.2246, 0.2039, -0.0845},   {0.6259, 0.5380, -0.2019},
    {0.6157, 1.6071, -1.6401},   {-1.1673, 0.0329, 1.0876},
    {0.8846, 1.5995, 0.3391},    {-0.3331, -1.0707, -0.1836},
    {-0.0044, 0.2398, -0.1401},  {-0.2779, -0.2592, 0.1044},
    {1.7452, -1.3110, -1.4161},  {-0.1755, 0.1737, -0.3240},
    {0.0367, -0.3573, -0.2408},  {0.1161, 0.4891, -0.3038},
    {-0.4537, -0.6996, 0.9442},  {-0.4734, 1.3808, 0.0132},
    {-0.2852, 0.0828, 1.0961},   {-0.0224, 0.0580, 0.0256},
    {-0.0471, -0.0427, -0.0711}, {0.1453, -0.0699, 0.0666},
    {-0.0249, -0.1993, 0.1253},  {-0.1275, 0.0713, -0.0539},
    {0.2097, 0.0855, -0.0405},   {0.0036, -0.5406, -0.0733},
    {-0.7410, 0.3198, 0.7374},   {0.2791, -1.6530, 0.1596},
    {-0.0735, 0.6989, -0.2308},  {-0.0476, -0.1726, -0.0396},
    {-0.0277, 0.3038, -0.5205},  {1.5075, 0.4358, 0.3018},
    {-1.2295, -1.5124, -0.2191}, {0.1291, 0.2009, 0.0382},
    {-0.0738, -0.1376, -0.0230}, {0.1144, 0.1904, 0.0319},
    {0.1941, 0.3240, 0.0492},    {-0.2799, -0.4633, -0.0703},
    {0.2011, 0.3217, 0.0415},    {-0.3174, 0.0115, -0.3628},
    {-0.0727, 0.0682, 0.1941},   {0.0776, 0.1224, 0.0160},
    {0.1232, 0.2156, 0.0305},    {0.1128, 0.1826, 0.0229},
    {0.0747, 0.1270, 0.0160},    {-0.1324, -0.2085, -0.0268},
    {0.6253, -0.7566, -0.7552},  {-0.8599, 0.7547, -0.2032},
    {0.4078, 1.2547, 0.1625},    {-0.0774, -0.4753, -0.0593},
    {-0.0432, 0.2205, -0.0741},  {-0.2572, -0.2856, 0.0719},
    {0.2599, -0.1073, 0.7938},   {0.0145, -0.0792, -0.9863},
    {-0.1230, -0.2661, 0.4272},  {0.2157, -0.3501, -0.4007},
    {-0.0496, 0.6745, -0.0189},  {-0.0383, -0.2125, 0.9846},
    {-0.0455, 0.5197, -0.0772},  {-0.0280, -0.0646, 0.1081},
    {-0.0968, -0.0299, -0.1475}, {0.0848, 0.0743, -0.0005},
    {0.0673, -0.1231, -0.1495},  {0.1300, 0.0322, 0.1891},
    {-0.1450, 0.0741, -0.0161},  {1.2344, 0.3769, 0.0638},
    {-2.5133, 0.7246, 1.8565},   {0.4174, -0.9591, -1.0895},
    {0.0012, 0.0823, -0.0112},   {-0.0245, -0.1621, -0.0298},
    {-0.0862, 0.2851, -0.5669},  {0.3276, 0.4412, 0.1516},
    {1.1749, -0.9977, 0.0890},   {-1.8382, 0.7927, -0.8926},
    {0.0487, -0.0175, 0.0355},   {0.7742, -0.2996, 0.6037},
    {-0.0574, 0.2915, -0.3980},  {0.0088, -0.5940, 0.0686},
    {0.4042, 0.3343, -0.1354},   {-0.0797, -0.3195, 0.1498},
    {-0.1577, 0.0713, -0.1371},  {0.1779, -0.0590, 0.1252},
    {0.5512, -2.4070, 0.3956},   {0.8075, 1.3026, -0.9157},
    {-0.6763, 0.9895, 0.0896},   {-0.0261, -0.2687, -0.0436},
    {-0.0482, 0.2466, -0.0702},  {-0.2312, -0.3255, 0.0867},
    {-0.6003, 1.5208, 0.8250},   {0.5618, -1.3301, -0.2176},
    {0.0136, -0.1309, 0.0815},   {-0.0240, 0.2760, -0.1734},
    {0.0382, -0.2601, 0.1590},   {0.0458, -0.3307, 0.2212},
    {-0.0216, 0.0807, -0.0536},  {-0.0744, 0.5613, -0.3533},
    {0.0952, -0.5876, 0.3555},   {-0.0791, 0.4936, -0.3007},
    {-0.0633, -0.0157, -0.1080}, {-0.1601, 0.0450, 0.0038},
    {-0.0132, 0.0932, -0.0571},  {0.0122, -0.0945, 0.0594},
    {0.0505, -0.3301, 0.1999},   {-0.0269, 0.1756, -0.1089},
    {-0.0188, 0.1254, -0.0765},  {0.0181, -0.1204, 0.0734},
    {-0.5393, 1.7272, 0.4672},   {0.5305, -1.7485, -1.6146},
    {0.0244, -1.4625, 1.0433},   {-0.0682, 0.5350, -0.1525},
    {-0.0161, -0.1902, -0.0481}, {0.2469, 0.3059, -0.0870},
    {-1.9051, 0.8553, 1.2376},   {-0.3578, -0.1111, -0.2816},
    {-0.2859, 0.3300, 0.0053},   {-0.2446, -0.5156, -0.0849},
    {1.0697, 0.8271, 0.1996},    {0.4653, -1.3450, -0.7619},
    {1.1568, 0.0476, 0.2067},    {0.0186, -0.0232, -0.0072},
    {-0.0333, 0.0276, -0.0433},  {-0.0058, 0.0320, 0.1021},
    {0.0822, 0.1830, 0.0472},    {0.0122, -0.0585, -0.1585},
    {-0.0991, -0.1135, 0.1499},  {0.1367, -0.5377, 0.0095},
    {-0.2154, 1.2234, 0.2913},   {-0.2276, -0.1268, -0.0917},
    {-0.2263, -0.5140, -0.0308}, {-0.0006, 0.0054, -0.0017},
    {-0.4940, -0.3447, -0.2248}, {1.2570, -0.9390, -0.6638},
    {-0.5964, 0.3480, -0.3266},  {0.4017, -1.4030, 1.1481},
    {-0.4150, 0.0891, -0.0190},  {0.2175, -0.9164, 0.3079},
    {-0.4519, -0.6647, 0.6255},  {-0.2932, 1.0392, -0.1166},
    {0.0393, -0.8763, -0.4417},  {0.2008, 0.5184, 0.9083},
    {-1.1249, 1.5481, -0.1155},  {1.0079, 0.0483, -1.2681},
    {-0.4096, 0.7965, 0.7694},   {0.5454, 0.4238, -1.1825},
    {0.0093, -0.1294, 0.0408},   {0.1313, -0.0465, 0.0309},
    {-0.1003, -0.0852, 0.0134},  {0.5265, 2.1787, 0.2608},
    {1.7594, 0.4793, 1.6602},    {4.4777, -1.1955, -0.8513},
    {-1.0446, 0.3692, -0.1156},  {-0.0625, -0.1397, -0.0162},
    {-0.0416, 0.0177, -0.0842},  {-1.4972, -1.5234, -0.1407},
    {-0.2148, -0.5606, -0.1053}, {0.2074, 1.1776, 0.2541},
    {-0.4353, -1.1751, -0.2227}, {-0.0019, 0.0607, 0.0451},
    {0.1295, 0.0763, -0.2010},   {-5.0974, 2.7217, -1.0997},
    {-0.5761, -1.6057, 1.7515},  {0.9206, -0.1343, -0.5093},
    {-0.7676, -0.2348, 0.2327},  {-0.1277, 0.0324, -0.0123},
    {0.0941, -0.1641, -0.0883},  {0.1745, 0.2452, -0.0012},
    {4.2406, 0.3451, -1.6015},   {-2.7182, 0.7235, 2.1525},
    {0.7756, -5.3365, -0.1650},  {-0.3049, 0.7988, -0.1903},
    {-0.0859, -0.0233, 0.0245},  {-0.0395, 0.1043, -0.0423},
    {-0.0489, -0.0367, 0.1897},  {0.1016, 10.7595, -3.1445},
    {0.0548, -3.8013, 2.2611},   {-0.3657, -0.2783, 0.1068},
    {-0.0087, -0.0065, -0.4726}, {0.8352, -0.2678, 0.1493},
    {-0.9046, -2.2207, 0.5425},  {1.0751, 1.5644, -1.9448},
    {-0.2990, -3.3232, 1.3207},  {-0.3708, 1.3227, -1.2183},
    {1.2724, 0.5469, 0.9962},    {-1.3184, 0.6577, 1.3213},
    {0.4348, -2.0215, 0.4186},   {0.9353, 0.4259, -0.8517},
    {-1.1715, 0.6530, 0.0124},   {-0.3416, -0.5764, -0.9382},
    {-0.6055, 0.5095, 0.9844},   {-0.3664, -0.1008, 0.2430},
    {0.2308, 0.0573, -0.3776},   {0.0014, 0.0046, -0.0667},
    {0.5266, 0.1501, 0.3898},    {-0.5449, -0.8628, 0.2303},
    {0.2239, 0.1947, -0.1353},   {0.6509, -0.0243, 0.3706},
    {-0.0426, 0.6308, -0.4601},  {-0.2759, -0.2482, 0.1120},
    {1.6746, 1.2525, -3.0173},   {-1.5127, -0.7767, 1.5811},
    {-0.2698, -0.2834, 0.4133},  {-0.0394, -0.0372, 0.0944},
    {-0.2473, -0.1212, 0.3484},  {0.6955, -0.1782, 0.1927},
    {-0.0514, 0.6492, -0.2636},  {-0.0967, 0.1713, -0.0670},
    {0.6614, 0.0025, 0.2415},    {-0.4050, -0.6294, 0.0774},
    {-0.0148, 0.0308, -0.0159},  {0.1562, 0.1421, -0.3968},
    {0.2604, 0.3045, -0.5015},   {-0.1102, -0.1975, 0.2767},
    {-0.3958, -0.1354, 0.5497},  {0.0381, 0.0441, -0.0969},
    {-0.2233, 0.1230, 0.1525},   {0.0865, -0.2271, 0.0850},
    {1.7077, 0.0836, -2.0675},   {-1.2394, 1.0677, 2.9233},
    {-0.8509, 0.2548, 5.5367},   {-0.0655, 0.1371, -1.2142},
    {-0.0866, -0.0335, 0.1097},  {-0.3316, -0.0514, -0.5225},
    {2.6218, -3.3007, -0.0558},  {0.1375, 0.4884, -0.6626},
    {-0.2933, 0.5645, 0.9465},   {0.7318, -1.5155, -1.1261},
    {-0.2091, -0.6895, 0.4029},  {0.1070, 0.3832, -0.2289},
    {0.0008, 0.0985, -0.0854},   {-1.0600, 0.4418, 0.2578},
    {0.4758, 0.7403, -0.5984},   {-1.5132, -0.5366, 0.6825},
    {0.1837, 1.5577, 0.5690},    {-1.0077, 0.0790, -0.0136},
    {0.4535, 0.4206, 0.5145},    {0.0982, 0.4106, -0.2163},
    {0.0726, 0.2691, -0.1530},   {-0.1080, -0.3945, 0.2290},
    {-0.0834, -0.2873, 0.1530},  {0.2256, 0.7233, -0.3665},
    {0.7068, -2.1819, -13.9708}, {-0.9794, 1.7690, 5.1245},
    {2.7269, 1.1104, -0.0697},   {-0.0332, -0.0232, -0.0030},
    {0.4194, -0.7378, 0.5292},   {-0.2874, 0.5867, 1.7892},
    {-0.6965, -1.9391, -2.0584}, {0.0048, 0.9700, 4.3012},
    {-0.7681, -0.2999, -1.7856}, {1.7730, -0.7139, -0.0238},
    {0.8415, 1.8489, -0.4114},   {-0.2449, -0.1687, 2.1334},
    {-0.1471, -1.2058, -0.5006}, {-0.2397, 1.0305, -0.7950},
    {-7.3207, -2.0683, 0.2510},  {3.6389, 0.3865, -1.8990},
    {1.8788, -3.9197, -0.4149},  {-1.2776, 1.3150, 0.0191},
    {0.2124, 0.0455, 0.7595},    {1.0796, 0.1495, 0.3086},
    {-2.4963, 0.4228, 1.1408},   {3.7739, 0.8557, -0.0886},
    {-1.7245, 0.1590, -0.3078},  {0.1883, -1.2363, 1.1209},
    {0.3638, -1.3650, -1.4809},  {1.8478, 0.9274, 0.5434},
    {-0.8710, 0.3203, 1.0084},   {-0.5319, -0.3252, -1.2178},
    {-5.2844, 11.7194, 0.8310},  {1.5701, -4.8258, 0.0458},
    {-0.1344, -0.1570, -0.9194}, {0.0031, 0.0589, -0.1631},
    {1.0590, -0.0503, 0.3917},   {0.2633, -2.5372, -0.6687},
    {0.1939, 2.8920, -0.8940},   {1.9988, -4.2544, -0.0178},
    {-0.9031, 1.6697, -0.8037},  {0.7409, 0.5191, 1.7161},
    {-1.6248, -0.7392, 1.0398},  {1.3805, -1.6190, -0.3429},
    {0.6023, 1.0856, -0.2572},   {-1.2628, -0.0324, -0.1007},
    {-0.2298, -1.4628, 2.5375},  {-0.5198, 2.1536, -1.1785},
    {0.0452, 0.0281, -0.0848},   {-0.0303, -0.1608, 0.2864},
    {0.0691, 0.5156, -1.0550},   {0.4517, 0.2838, 0.2647},
    {-0.0698, -2.4663, 0.5071},  {0.8321, 2.5504, -2.1001},
    {0.4209, -0.0213, -0.0991},  {-0.3798, 0.2073, 0.0717},
    {-0.6758, -2.0470, 1.8181},  {0.0410, 0.1840, -0.3332}};
TEST_CASE("Torsion-Trpcage", "[ff][etors][trpcage]") {
  const char* k = "test_trpcage.key";
  const char* x1 = "test_trpcage.xyz";
  const char* p = "amoebapro13.prm";

  std::string k0 = trpcage_key;
  k0 += torsionterm_only;
  TestFile fke(k, k0);

  TestFile fx1(x1, trpcage_xyz);
  TestFile fpr(p, commit_6fe8e913::amoebapro13_prm);

  const char* argv[] = {"dummy", x1};
  int argc = 2;
  test_begin_with_args(argc, argv);
  rc_flag = usage;
  initialize();

  const double eps_e = 0.0001;
  const double ref_e = 12.4527;
  const int ref_count = 843;
  const double eps_g = test_get_eps(0.0003, 0.0001);
  const double eps_v = 0.001;
  const double ref_v[][3] = {{-0.522, 2.350, -4.405},
                             {2.350, -0.934, -2.815},
                             {-4.405, -2.815, 1.456}};

  COMPARE_BONDED_FORCE(etors, et, ref_e, eps_e, ntors, ref_count, gx, gy, gz,
                       ref_g_tors_trpcage, eps_g, vir_et, ref_v, eps_v);

  finish();
  test_end();
}
