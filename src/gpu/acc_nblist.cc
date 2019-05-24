#include "gpu/acc_image.h"
#include "gpu/decl_mdstate.h"
#include "gpu/decl_nblist.h"

TINKER_NAMESPACE_BEGIN
namespace gpu {
void nblist_construct_double_loop_(nblist_st* lst) {
  #pragma acc data deviceptr(lst)
  #pragma acc parallel loop
  for (int i = 0; i < n; ++i) {
    lst->nlst[i] = n - i - 1;
    lst->lst[i] = i + 1;
  }
}

// void nblist_update_double_loop_() {}

void nblist_construct_nblist_(nblist_st* lst) {
  #pragma acc data deviceptr(lst,box)
  #pragma acc parallel loop
  for (int i = 0; i < n; ++i) {
    real xi = lst->x[i];
    real yi = lst->y[i];
    real zi = lst->z[i];
    lst->xold[i] = xi;
    lst->yold[i] = yi;
    lst->zold[i] = zi;

    lst->nlst[i] = 0;
    const int maxnlst = lst->maxnlst;
    const real buf2 = (lst->cutoff + lst->buffer) * (lst->cutoff + lst->buffer);
    #pragma acc loop seq
    for (int k = i + 1; k < n; ++k) {
      real xr = xi - lst->x[k];
      real yr = yi - lst->y[k];
      real zr = zi - lst->z[k];
      image(xr, yr, zr, box);
      real r2 = xr * xr + yr * yr + zr * zr;
      if (r2 <= buf2) {
        const int j = lst->nlst[i];
        lst->nlst[i] += 1;
        lst->lst[i * maxnlst + j] = k;
      }
    }
  }
}

void nblist_update_nblist_(nblist_st*) {}

void nblist_construct(const nblist_st& st, nblist_st* lst) {
  if (st.maxnlst == 1) {
    nblist_construct_double_loop_(lst);
  } else {
    nblist_construct_nblist_(lst);
  }
}

void nblist_update(const nblist_st& st, nblist_st* lst) {
  if (st.maxnlst == 1) {
    // nblist_update_double_loop_();
  } else {
    nblist_update_nblist_(lst);
  }
}
}
TINKER_NAMESPACE_END