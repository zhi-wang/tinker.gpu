#include "files.h"
#include "test/ff.h"
#include "test/rt.h"
#include "test/test.h"

m_tinker_using_namespace;
using namespace test;

#define COMPARE_GRAD_                                                          \
  {                                                                            \
    grad_t grad(gpu::n);                                                       \
    double* dst = &grad[0][0];                                                 \
    gpu::copyout_data2(0, 3, dst, gpu::gx, gpu::n);                            \
    gpu::copyout_data2(1, 3, dst, gpu::gy, gpu::n);                            \
    gpu::copyout_data2(2, 3, dst, gpu::gz, gpu::n);                            \
    for (int i = 0; i < 6; ++i) {                                              \
      for (int j = 0; j < 3; ++j) {                                            \
        REQUIRE(grad[i * 30][j] == Approx(ref_grad[i][j]).epsilon(eps));       \
      }                                                                        \
    }                                                                          \
  }
#define COMPARE_CODE_BLOCK1_                                                   \
  {                                                                            \
    gpu::zero_egv();                                                           \
    tinker_gpu_evdw_hal0();                                                    \
    COMPARE_ENERGY_(gpu::ev, ref_eng, eps);                                    \
                                                                               \
    gpu::zero_egv();                                                           \
    tinker_gpu_evdw_hal1();                                                    \
    COMPARE_ENERGY_(gpu::ev, ref_eng, eps);                                    \
    COMPARE_GRAD_;                                                             \
    COMPARE_VIR_(gpu::vir_ev, ref_v, eps);                                     \
                                                                               \
    gpu::zero_egv();                                                           \
    tinker_gpu_evdw_hal3();                                                    \
    COMPARE_ENERGY_(gpu::ev, ref_eng, eps);                                    \
    COMPARE_COUNT_(gpu::nev, ref_count);                                       \
                                                                               \
    gpu::zero_egv();                                                           \
    tinker_gpu_evdw_hal4();                                                    \
    COMPARE_ENERGY_(gpu::ev, ref_eng, eps);                                    \
    COMPARE_GRAD_;                                                             \
                                                                               \
    gpu::zero_egv();                                                           \
    tinker_gpu_evdw_hal5();                                                    \
    COMPARE_GRAD_;                                                             \
                                                                               \
    gpu::zero_egv();                                                           \
    tinker_gpu_evdw_hal6();                                                    \
    COMPARE_GRAD_;                                                             \
    COMPARE_VIR_(gpu::vir_ev, ref_v, eps);                                     \
  }

TEST_CASE("CLN025", "[ff][ehal][cln025]") {
  const char* x = "test_cln025.xyz";
  const char* k = "test_cln025.key";
  const char* p = "amoebabio09.prm";
  std::string k0 = cln025_key;
  k0 += "vdwterm    only\n";

  file fx(x, cln025_xyz);
  file px(p, amoebabio09_prm);

  int usage = 0;
  usage |= gpu::use_xyz;
  usage |= gpu::use_energy;
  usage |= gpu::use_grad;
  usage |= gpu::use_virial;

  SECTION("ehal -- gas phase, no cutoff") {
    file kx(k, k0);

    const char* argv[] = {"dummy", x};
    int argc = 2;

    const double eps = 2.0e-4;
    const double ref_eng = 88.8860;
    const int ref_count = 13225;
    // atom 1, 31, 61, 91, 121, 151
    const double ref_grad[][3] = {
        {-10.4202, 0.6937, 2.7019}, {4.0910, 2.0984, -2.4349},
        {-0.5266, 3.5665, 0.4037},  {-0.2577, -1.9100, 6.9858},
        {1.0470, 3.2855, -4.3019},  {4.3475, -2.2594, 1.8280}};
    const double ref_v[][3] = {{-800.488, -37.589, -2.250},
                               {-37.589, -758.657, 41.895},
                               {-2.250, 41.895, -681.179}};

    test_begin_1_xyz(argc, argv);
    gpu::use_data = usage;
    tinker_gpu_data_create();

    COMPARE_CODE_BLOCK1_;

    tinker_gpu_data_destroy();
    test_end();
  }

  SECTION("ehal -- pbc, cutoff") {
    std::string k1 = k0;
    k1 += "neighbor-list\n";
    k1 += "vdw-cutoff        6.0\n";
    k1 += "a-axis           18.0\n";
    file kx(k, k1);

    const char* argv[] = {"dummy", x};
    int argc = 2;

    const double eps = 2.0e-4;
    const double ref_eng = 104.3872;
    const int ref_count = 4116;
    // atom 1, 31, 61, 91, 121, 151
    const double ref_grad[][3] = {
        {-10.5674, 0.6411, 2.6721}, {4.0652, 2.1284, -2.4015},
        {-0.5214, 3.5720, 0.3980},  {-0.1419, -1.9235, 6.9387},
        {1.0707, 3.2989, -4.2483},  {4.3235, -2.2325, 1.8485}};
    const double ref_v[][3] = {{-914.956, -8.336, 26.361},
                               {-8.336, -781.555, 29.820},
                               {26.361, 29.820, -706.064}};

    test_begin_1_xyz(argc, argv);
    gpu::use_data = usage;
    tinker_gpu_data_create();

    COMPARE_CODE_BLOCK1_;

    tinker_gpu_data_destroy();
    test_end();
  }
}

#undef COMPARE_GRAD_
#undef COMPARE_CODE_BLOCK1_