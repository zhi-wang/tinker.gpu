#ifndef TINKER_MOD_NONPOL_HH_
#define TINKER_MOD_NONPOL_HH_

#include "macro.h"

TINKER_NAMESPACE_BEGIN namespace nonpol {
const double epso = 0.1100e0;
const double epsh = 0.0135e0;
const double rmino = 1.7025e0;
const double rminh = 1.3275e0;
const double awater = 0.033428e0;
const double slevy = 1.0e0;
extern double& solvprs;
extern double& surften;
extern double& spcut;
extern double& spoff;
extern double& stcut;
extern double& stoff;
extern double*& rcav;
extern double*& rdisp;
extern double*& cdisp;

#ifdef TINKER_MOD_CPP_
extern "C" double TINKER_MOD(nonpol, solvprs);
extern "C" double TINKER_MOD(nonpol, surften);
extern "C" double TINKER_MOD(nonpol, spcut);
extern "C" double TINKER_MOD(nonpol, spoff);
extern "C" double TINKER_MOD(nonpol, stcut);
extern "C" double TINKER_MOD(nonpol, stoff);
extern "C" double* TINKER_MOD(nonpol, rcav);
extern "C" double* TINKER_MOD(nonpol, rdisp);
extern "C" double* TINKER_MOD(nonpol, cdisp);

double& solvprs = TINKER_MOD(nonpol, solvprs);
double& surften = TINKER_MOD(nonpol, surften);
double& spcut = TINKER_MOD(nonpol, spcut);
double& spoff = TINKER_MOD(nonpol, spoff);
double& stcut = TINKER_MOD(nonpol, stcut);
double& stoff = TINKER_MOD(nonpol, stoff);
double*& rcav = TINKER_MOD(nonpol, rcav);
double*& rdisp = TINKER_MOD(nonpol, rdisp);
double*& cdisp = TINKER_MOD(nonpol, cdisp);
#endif
} TINKER_NAMESPACE_END

#endif
