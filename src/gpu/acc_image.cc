#include "gpu/acc_image.h"
#include "gpu/acc_mathfunc.h"

TINKER_NAMESPACE_BEGIN
namespace gpu {
#pragma acc routine seq
static inline void box_null_image() {}

#pragma acc routine seq
static inline void box_null_imagen() {}

#pragma acc routine seq
static inline real box_null_vobox() { return INFINITY; }

#pragma acc routine seq
static inline void box_ortho_image(real& __restrict__ xr, real& __restrict__ yr,
                                   real& __restrict__ zr,
                                   const box_st* __restrict__ pb) {
  real lx = pb->lvec[0][0];
  real rx = pb->recip[0][0];
  real a = REAL_FLOOR(xr * rx + 0.5f);
  xr -= a * lx;

  real ly = pb->lvec[1][1];
  real ry = pb->recip[1][1];
  real b = REAL_FLOOR(yr * ry + 0.5f);
  yr -= b * ly;

  real lz = pb->lvec[2][2];
  real rz = pb->recip[2][2];
  real c = REAL_FLOOR(zr * rz + 0.5f);
  zr -= c * lz;
}

#pragma acc routine seq
static inline void box_ortho_imagen(real& __restrict__ xr,
                                    real& __restrict__ yr,
                                    real& __restrict__ zr,
                                    const box_st* __restrict__ pb) {
  real lx = pb->lvec[0][0];
  real lx2 = lx * 0.5f;
  real a = REAL_ABS(xr);
  xr = (a > lx2 ? a - lx : a);

  real ly = pb->lvec[1][1];
  real ly2 = ly * 0.5f;
  real b = REAL_ABS(yr);
  yr = (b > ly2 ? b - ly : b);

  real lz = pb->lvec[2][2];
  real lz2 = lz * 0.5f;
  real c = REAL_ABS(zr);
  zr = (c > lz2 ? c - lz : c);
}

#pragma acc routine seq
static inline real box_ortho_volbox(const box_st* __restrict__ pb) {
  return pb->lvec[0][0] * pb->lvec[1][1] * pb->lvec[2][2];
}

#pragma acc routine seq
void image(real& __restrict__ xr, real& __restrict__ yr, real& __restrict__ zr,
           const box_st* __restrict__ pb) {
  switch (pb->shape) {
  case box_ortho:
    box_ortho_image(xr, yr, zr, pb);
    break;
  default /* box_null */:
    box_null_image();
    break;
  }
}

#pragma acc routine seq
void imagen(real& __restrict__ xr, real& __restrict__ yr, real& __restrict__ zr,
            const box_st* __restrict__ pb) {
  switch (pb->shape) {
  case box_ortho:
    box_ortho_imagen(xr, yr, zr, pb);
    break;
  default /* box_null */:
    box_null_imagen();
    break;
  }
}

#pragma acc routine seq
real volbox(const box_st* __restrict__ pb) {
  real val;
  switch (pb->shape) {
  case box_ortho:
    val = box_ortho_volbox(pb);
    break;
  default /* box_null */:
    val = box_null_vobox();
    break;
  }
  return val;
}
}
TINKER_NAMESPACE_END