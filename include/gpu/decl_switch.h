#ifndef TINKER_GPU_DECL_SWITCH_H_
#define TINKER_GPU_DECL_SWITCH_H_

#include "util/macro.h"

TINKER_NAMESPACE_BEGIN
namespace gpu {
enum {
  switch_default = 0,
  switch_vdw,
  switch_repuls,
  switch_disp,
  switch_charge,
  switch_chgdpl,
  switch_dipole,
  switch_mpole,
  switch_chgtrn,
  switch_ewald,
  switch_dewald,
  switch_usolve,
  switch_gkv,
  switch_gksa,
};

void switch_cut_off(int switch_type, double& cut, double& off);
}
TINKER_NAMESPACE_END

#endif