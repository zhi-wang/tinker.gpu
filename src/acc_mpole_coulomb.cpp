#include "acc_seq.h"
#include "couple.h"
#include "e_mpole.h"
#include "md.h"
#include "nblist.h"

TINKER_NAMESPACE_BEGIN
template <int USE>
void empole_coulomb_tmpl() {
  constexpr int do_e = USE & calc::energy;
  constexpr int do_a = USE & calc::analyz;
  constexpr int do_g = USE & calc::grad;
  constexpr int do_v = USE & calc::virial;
  static_assert(do_v ? do_g : true, "");
  static_assert(do_a ? do_e : true, "");

  const real f = electric / dielec;

  const real off = mpole_switch_off;
  const real off2 = off * off;
  const int maxnlst = mlist_unit->maxnlst;
  const auto* mlst = mlist_unit.deviceptr();

  const auto* coupl = couple_unit.deviceptr();

  static std::vector<real> mscalebuf;
  mscalebuf.resize(n, 1);
  real* mscale = mscalebuf.data();

  #pragma acc parallel loop independent\
              deviceptr(x,y,z,gx,gy,gz,box,coupl,mlst,\
                        rpole,\
                        em,nem,vir_em,trqx,trqy,trqz)\
              firstprivate(mscale[0:n])
  for (int i = 0; i < n; ++i) {

    // set exclusion coefficients for connected atoms

    const int n12i = coupl->n12[i];
    const int n13i = coupl->n13[i];
    const int n14i = coupl->n14[i];
    const int n15i = coupl->n15[i];
    #pragma acc loop independent
    for (int j = 0; j < n12i; ++j)
      mscale[coupl->i12[i][j]] = m2scale;
    #pragma acc loop independent
    for (int j = 0; j < n13i; ++j)
      mscale[coupl->i13[i][j]] = m3scale;
    #pragma acc loop independent
    for (int j = 0; j < n14i; ++j)
      mscale[coupl->i14[i][j]] = m4scale;
    #pragma acc loop independent
    for (int j = 0; j < n15i; ++j)
      mscale[coupl->i15[i][j]] = m5scale;

    real xi = x[i];
    real yi = y[i];
    real zi = z[i];
    real ci = rpole[i][mpl_pme_0];
    real dix = rpole[i][mpl_pme_x];
    real diy = rpole[i][mpl_pme_y];
    real diz = rpole[i][mpl_pme_z];
    real qixx = rpole[i][mpl_pme_xx];
    real qixy = rpole[i][mpl_pme_xy];
    real qixz = rpole[i][mpl_pme_xz];
    real qiyy = rpole[i][mpl_pme_yy];
    real qiyz = rpole[i][mpl_pme_yz];
    real qizz = rpole[i][mpl_pme_zz];

    int nmlsti = mlst->nlst[i];
    int base = i * maxnlst;
    #pragma acc loop independent
    for (int kk = 0; kk < nmlsti; ++kk) {
      int k = mlst->lst[base + kk];
      real xr = x[k] - xi;
      real yr = y[k] - yi;
      real zr = z[k] - zi;

      image(xr, yr, zr, box);
      real r2 = xr * xr + yr * yr + zr * zr;
      if (r2 <= off2) {
        real r = REAL_SQRT(r2);
        real ck = rpole[k][mpl_pme_0];
        real dkx = rpole[k][mpl_pme_x];
        real dky = rpole[k][mpl_pme_y];
        real dkz = rpole[k][mpl_pme_z];
        real qkxx = rpole[k][mpl_pme_xx];
        real qkxy = rpole[k][mpl_pme_xy];
        real qkxz = rpole[k][mpl_pme_xz];
        real qkyy = rpole[k][mpl_pme_yy];
        real qkyz = rpole[k][mpl_pme_yz];
        real qkzz = rpole[k][mpl_pme_zz];

        real dir = dix * xr + diy * yr + diz * zr;
        real qix = qixx * xr + qixy * yr + qixz * zr;
        real qiy = qixy * xr + qiyy * yr + qiyz * zr;
        real qiz = qixz * xr + qiyz * yr + qizz * zr;
        real qir = qix * xr + qiy * yr + qiz * zr;
        real dkr = dkx * xr + dky * yr + dkz * zr;
        real qkx = qkxx * xr + qkxy * yr + qkxz * zr;
        real qky = qkxy * xr + qkyy * yr + qkyz * zr;
        real qkz = qkxz * xr + qkyz * yr + qkzz * zr;
        real qkr = qkx * xr + qky * yr + qkz * zr;
        real dik = dix * dkx + diy * dky + diz * dkz;
        real qik = qix * qkx + qiy * qky + qiz * qkz;
        real diqk = dix * qkx + diy * qky + diz * qkz;
        real dkqi = dkx * qix + dky * qiy + dkz * qiz;
        real qiqk = 2 * (qixy * qkxy + qixz * qkxz + qiyz * qkyz) +
            qixx * qkxx + qiyy * qkyy + qizz * qkzz;

        real rr1 = f * mscale[k] * REAL_RECIP(r);
        real rr2 = REAL_RECIP(r2);
        real rr3 = rr1 * rr2;
        real rr5 = 3 * rr3 * rr2;
        real rr7 = 5 * rr5 * rr2;
        real rr9 = 7 * rr7 * rr2;

        real term1 = ci * ck;
        real term2 = ck * dir - ci * dkr + dik;
        real term3 = ci * qkr + ck * qir - dir * dkr + 2 * (dkqi - diqk + qiqk);
        real term4 = dir * qkr - dkr * qir - 4 * qik;
        real term5 = qir * qkr;

        if_constexpr(do_e) {
          real e = term1 * rr1 + term2 * rr3 + term3 * rr5 + term4 * rr7 +
              term5 * rr9;
          #pragma acc atomic update
          *em += e;
          if_constexpr(do_a) {
            if (e != 0) {
              #pragma acc atomic update
              *nem += 1;
            }
          }
        } // end if (do_e)

        if_constexpr(do_g) {

          // gradient

          real qixk = qixx * qkx + qixy * qky + qixz * qkz;
          real qiyk = qixy * qkx + qiyy * qky + qiyz * qkz;
          real qizk = qixz * qkx + qiyz * qky + qizz * qkz;
          real qkxi = qkxx * qix + qkxy * qiy + qkxz * qiz;
          real qkyi = qkxy * qix + qkyy * qiy + qkyz * qiz;
          real qkzi = qkxz * qix + qkyz * qiy + qkzz * qiz;

          real diqkx = dix * qkxx + diy * qkxy + diz * qkxz;
          real diqky = dix * qkxy + diy * qkyy + diz * qkyz;
          real diqkz = dix * qkxz + diy * qkyz + diz * qkzz;
          real dkqix = dkx * qixx + dky * qixy + dkz * qixz;
          real dkqiy = dkx * qixy + dky * qiyy + dkz * qiyz;
          real dkqiz = dkx * qixz + dky * qiyz + dkz * qizz;

          real rr11 = 9 * rr9 * rr2;

          real de = term1 * rr3 + term2 * rr5 + term3 * rr7 + term4 * rr9 +
              term5 * rr11;

          term1 = -ck * rr3 + dkr * rr5 - qkr * rr7;
          term2 = ci * rr3 + dir * rr5 + qir * rr7;
          term3 = 2 * rr5;
          term4 = 2 * (-ck * rr5 + dkr * rr7 - qkr * rr9);
          term5 = 2 * (-ci * rr5 - dir * rr7 - qir * rr9);
          real term6 = 4 * rr7;

          real frcx = de * xr + term1 * dix + term2 * dkx +
              term3 * (diqkx - dkqix) + term4 * qix + term5 * qkx +
              term6 * (qixk + qkxi);
          real frcy = de * yr + term1 * diy + term2 * dky +
              term3 * (diqky - dkqiy) + term4 * qiy + term5 * qky +
              term6 * (qiyk + qkyi);
          real frcz = de * zr + term1 * diz + term2 * dkz +
              term3 * (diqkz - dkqiz) + term4 * qiz + term5 * qkz +
              term6 * (qizk + qkzi);

          #pragma acc atomic update
          gx[i] += frcx;
          #pragma acc atomic update
          gy[i] += frcy;
          #pragma acc atomic update
          gz[i] += frcz;
          #pragma acc atomic update
          gx[k] -= frcx;
          #pragma acc atomic update
          gy[k] -= frcy;
          #pragma acc atomic update
          gz[k] -= frcz;

          // torque

          real dirx = diy * zr - diz * yr;
          real diry = diz * xr - dix * zr;
          real dirz = dix * yr - diy * xr;
          real dkrx = dky * zr - dkz * yr;
          real dkry = dkz * xr - dkx * zr;
          real dkrz = dkx * yr - dky * xr;
          real dikx = diy * dkz - diz * dky;
          real diky = diz * dkx - dix * dkz;
          real dikz = dix * dky - diy * dkx;

          real qirx = qiz * yr - qiy * zr;
          real qiry = qix * zr - qiz * xr;
          real qirz = qiy * xr - qix * yr;
          real qkrx = qkz * yr - qky * zr;
          real qkry = qkx * zr - qkz * xr;
          real qkrz = qky * xr - qkx * yr;
          real qikx = qky * qiz - qkz * qiy;
          real qiky = qkz * qix - qkx * qiz;
          real qikz = qkx * qiy - qky * qix;

          real qikrx = qizk * yr - qiyk * zr;
          real qikry = qixk * zr - qizk * xr;
          real qikrz = qiyk * xr - qixk * yr;
          real qkirx = qkzi * yr - qkyi * zr;
          real qkiry = qkxi * zr - qkzi * xr;
          real qkirz = qkyi * xr - qkxi * yr;

          real diqkrx = diqkz * yr - diqky * zr;
          real diqkry = diqkx * zr - diqkz * xr;
          real diqkrz = diqky * xr - diqkx * yr;
          real dkqirx = dkqiz * yr - dkqiy * zr;
          real dkqiry = dkqix * zr - dkqiz * xr;
          real dkqirz = dkqiy * xr - dkqix * yr;

          real dqikx = diy * qkz - diz * qky + dky * qiz - dkz * qiy -
              2 *
                  (qixy * qkxz + qiyy * qkyz + qiyz * qkzz - qixz * qkxy -
                   qiyz * qkyy - qizz * qkyz);
          real dqiky = diz * qkx - dix * qkz + dkz * qix - dkx * qiz -
              2 *
                  (qixz * qkxx + qiyz * qkxy + qizz * qkxz - qixx * qkxz -
                   qixy * qkyz - qixz * qkzz);
          real dqikz = dix * qky - diy * qkx + dkx * qiy - dky * qix -
              2 *
                  (qixx * qkxy + qixy * qkyy + qixz * qkyz - qixy * qkxx -
                   qiyy * qkxy - qiyz * qkxz);

          real ttmi[3], ttmk[3];
          ttmi[0] = -rr3 * dikx + term1 * dirx + term3 * (dqikx + dkqirx) -
              term4 * qirx - term6 * (qikrx + qikx);
          ttmi[1] = -rr3 * diky + term1 * diry + term3 * (dqiky + dkqiry) -
              term4 * qiry - term6 * (qikry + qiky);
          ttmi[2] = -rr3 * dikz + term1 * dirz + term3 * (dqikz + dkqirz) -
              term4 * qirz - term6 * (qikrz + qikz);
          ttmk[0] = rr3 * dikx + term2 * dkrx - term3 * (dqikx + diqkrx) -
              term5 * qkrx - term6 * (qkirx - qikx);
          ttmk[1] = rr3 * diky + term2 * dkry - term3 * (dqiky + diqkry) -
              term5 * qkry - term6 * (qkiry - qiky);
          ttmk[2] = rr3 * dikz + term2 * dkrz - term3 * (dqikz + diqkrz) -
              term5 * qkrz - term6 * (qkirz - qikz);

          #pragma acc atomic update
          trqx[i] += ttmi[0];
          #pragma acc atomic update
          trqy[i] += ttmi[1];
          #pragma acc atomic update
          trqz[i] += ttmi[2];
          #pragma acc atomic update
          trqx[k] += ttmk[0];
          #pragma acc atomic update
          trqy[k] += ttmk[1];
          #pragma acc atomic update
          trqz[k] += ttmk[2];

          // virial

          if_constexpr(do_v) {
            real vxx = -xr * frcx;
            real vxy = -0.5f * (yr * frcx + xr * frcy);
            real vxz = -0.5f * (zr * frcx + xr * frcz);
            real vyy = -yr * frcy;
            real vyz = -0.5f * (zr * frcy + yr * frcz);
            real vzz = -zr * frcz;

            #pragma acc atomic update
            vir_em[0] += vxx;
            #pragma acc atomic update
            vir_em[1] += vxy;
            #pragma acc atomic update
            vir_em[2] += vxz;
            #pragma acc atomic update
            vir_em[3] += vxy;
            #pragma acc atomic update
            vir_em[4] += vyy;
            #pragma acc atomic update
            vir_em[5] += vyz;
            #pragma acc atomic update
            vir_em[6] += vxz;
            #pragma acc atomic update
            vir_em[7] += vyz;
            #pragma acc atomic update
            vir_em[8] += vzz;
          } // end if (do_v)
        }   // end if (do_g)
      }     // end if (r2 <= off2)
    }       // end for (int kk)

    // reset exclusion coefficients for connected atoms

    #pragma acc loop independent
    for (int j = 0; j < n12i; ++j)
      mscale[coupl->i12[i][j]] = 1;
    #pragma acc loop independent
    for (int j = 0; j < n13i; ++j)
      mscale[coupl->i13[i][j]] = 1;
    #pragma acc loop independent
    for (int j = 0; j < n14i; ++j)
      mscale[coupl->i14[i][j]] = 1;
    #pragma acc loop independent
    for (int j = 0; j < n15i; ++j)
      mscale[coupl->i15[i][j]] = 1;
  } // end for (int i)
}

void empole_coulomb(int vers) {
  if (vers == calc::v0)
    empole_coulomb_tmpl<calc::v0>();
  else if (vers == calc::v1)
    empole_coulomb_tmpl<calc::v1>();
  else if (vers == calc::v3)
    empole_coulomb_tmpl<calc::v3>();
  else if (vers == calc::v4)
    empole_coulomb_tmpl<calc::v4>();
  else if (vers == calc::v5)
    empole_coulomb_tmpl<calc::v5>();
  else if (vers == calc::v6)
    empole_coulomb_tmpl<calc::v6>();
}
TINKER_NAMESPACE_END