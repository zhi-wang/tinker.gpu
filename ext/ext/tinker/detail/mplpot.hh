#ifndef TINKER_MOD_MPLPOT_HH_
#define TINKER_MOD_MPLPOT_HH_

#include "macro.h"

TINKER_NAMESPACE_BEGIN namespace mplpot {
extern double& m2scale;
extern double& m3scale;
extern double& m4scale;
extern double& m5scale;
extern int& use_chgpen;

#ifdef TINKER_MOD_CPP_
extern "C" double TINKER_MOD(mplpot, m2scale);
extern "C" double TINKER_MOD(mplpot, m3scale);
extern "C" double TINKER_MOD(mplpot, m4scale);
extern "C" double TINKER_MOD(mplpot, m5scale);
extern "C" int TINKER_MOD(mplpot, use_chgpen);

double& m2scale = TINKER_MOD(mplpot, m2scale);
double& m3scale = TINKER_MOD(mplpot, m3scale);
double& m4scale = TINKER_MOD(mplpot, m4scale);
double& m5scale = TINKER_MOD(mplpot, m5scale);
int& use_chgpen = TINKER_MOD(mplpot, use_chgpen);
#endif
} TINKER_NAMESPACE_END

#endif
