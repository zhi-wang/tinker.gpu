#include "gpu/decl_dataop.h"
#include "gpu/decl_mdstate.h"
#include "gpu/decl_nblist.h"
#include "gpu/decl_switch.h"
#include "gpu/e_mpole.h"
#include "rc_cudart.h"
#include "util/fort_str.h"

TINKER_NAMESPACE_BEGIN
namespace gpu {
int electyp;
std::string electyp_str;

local_frame_def_st* zaxis;

real (*pole)[mpl_total];
real (*rpole)[mpl_total];

double mpole_switch_cut, mpole_switch_off;

real* em;
int* nem;
real* vir_em;
real *trqx, *trqy, *trqz;

int use_empole() { return potent::use_mpole; }

void get_empole_type(int& typ, std::string& typ_str) {
  if (limits::use_ewald) {
    typ = elec_ewald;
    typ_str = "EWALD";
  } else {
    typ = elec_coulomb;
    typ_str = "COULOMB";
  }
}
}
TINKER_NAMESPACE_END

extern "C" {
void tinker_gpu_mlist_build() {
  m_tinker_using_namespace;
  if (gpu::use_mpole_list()) {
    gpu::nblist_construct(gpu::mlist_obj_, gpu::mlst);
  }
}
void tinker_gpu_mlist_update() {
  m_tinker_using_namespace;
  if (gpu::use_mpole_list()) {
    gpu::nblist_update(gpu::mlist_obj_, gpu::mlst);
  }
}
}

TINKER_NAMESPACE_BEGIN
namespace gpu {
void e_mpole_data(int op) {
  if (!use_empole())
    return;

  if (op == op_destroy) {
    check_cudart(cudaFree(zaxis));
    check_cudart(cudaFree(pole));
    check_cudart(cudaFree(rpole));

    check_cudart(cudaFree(em));
    check_cudart(cudaFree(nem));
    check_cudart(cudaFree(vir_em));
    check_cudart(cudaFree(trqx));
    check_cudart(cudaFree(trqy));
    check_cudart(cudaFree(trqz));
  }

  if (op == op_create) {
    get_empole_type(electyp, electyp_str);

    const size_t rs = sizeof(real);
    size_t size;

    if (electyp == elec_coulomb)
      switch_cut_off(switch_mpole, mpole_switch_cut, mpole_switch_off);
    else if (electyp == elec_ewald)
      switch_cut_off(switch_ewald, mpole_switch_cut, mpole_switch_off);

    // Regarding chkpole routine:
    // 1. The chiralities of the atoms will not change in the simulations;
    // 2. chkpole routine has been called in mechanic routine so that the values
    // in mpole::pole are correct;
    // 3. yaxis values are directly copied from Tinker, and are NOT
    // subtracted by 1 becasue of the checks in chkpole;
    // 4. GPU chkpole kernel is necessary when unexpected changes of charalities
    // may happen, e.g. in Monte Carlo simulations.
    size = sizeof(local_frame_def_st);
    check_cudart(cudaMalloc(&zaxis, n * size));
    static_assert(sizeof(local_frame_def_st) == 4 * sizeof(int), "");
    std::vector<int> zaxisbuf(4 * n);
    for (int i = 0; i < n; ++i) {
      int base = 4 * i;
      zaxisbuf[base] = mpole::zaxis[i] - 1;
      zaxisbuf[base + 1] = mpole::xaxis[i] - 1;
      zaxisbuf[base + 2] = mpole::yaxis[i];
      fstr_view str = mpole::polaxe[i];
      int val;
      if (str == "Z-Only")
        val = pole_z_only;
      else if (str == "Z-then-X")
        val = pole_z_then_x;
      else if (str == "Bisector")
        val = pole_bisector;
      else if (str == "Z-Bisect")
        val = pole_z_bisect;
      else if (str == "3-Fold")
        val = pole_3_fold;
      else
        val = pole_none;
      zaxisbuf[base + 3] = val;
    }
    copyin_data(reinterpret_cast<int*>(zaxis), zaxisbuf.data(), 4 * n);

    size = rs * mpl_total;
    check_cudart(cudaMalloc(&pole, n * size));
    check_cudart(cudaMalloc(&rpole, n * size));
    std::vector<double> polebuf(mpl_total * n);
    for (int i = 0; i < n; ++i) {
      int b1 = mpl_total * i;
      int b2 = mpole::maxpole * i;
      // Tinker c = 0, dx = 1, dy = 2, dz = 3
      // Tinker qxx = 4, qxy = 5, qxz = 6
      //        qyx    , qyy = 8, qyz = 9
      //        qzx    , qzy    , qzz = 12
      polebuf[b1 + mpl_pme_0] = mpole::pole[b2 + 0];
      polebuf[b1 + mpl_pme_x] = mpole::pole[b2 + 1];
      polebuf[b1 + mpl_pme_y] = mpole::pole[b2 + 2];
      polebuf[b1 + mpl_pme_z] = mpole::pole[b2 + 3];
      polebuf[b1 + mpl_pme_xx] = mpole::pole[b2 + 4];
      polebuf[b1 + mpl_pme_xy] = mpole::pole[b2 + 5];
      polebuf[b1 + mpl_pme_xz] = mpole::pole[b2 + 6];
      polebuf[b1 + mpl_pme_yy] = mpole::pole[b2 + 8];
      polebuf[b1 + mpl_pme_yz] = mpole::pole[b2 + 9];
      polebuf[b1 + mpl_pme_zz] = mpole::pole[b2 + 12];
    }
    copyin_data(reinterpret_cast<real*>(pole), polebuf.data(), mpl_total * n);

    check_cudart(cudaMalloc(&em, rs));
    check_cudart(cudaMalloc(&nem, sizeof(int)));
    check_cudart(cudaMalloc(&vir_em, 9 * rs));
    if (use_data & use_grad)
      check_cudart(cudaMalloc(&trqx, rs * n));
    check_cudart(cudaMalloc(&trqy, rs * n));
    check_cudart(cudaMalloc(&trqz, rs * n));
  }
}

void zero_torque() {
  if (use_data & use_grad) {
    zero_data(trqx, n);
    zero_data(trqy, n);
    zero_data(trqz, n);
  }
}
}
TINKER_NAMESPACE_END