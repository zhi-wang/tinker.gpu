#pragma once
#include "macro.h"
#include <cstring>


TINKER_NAMESPACE_BEGIN
namespace platform {
namespace cu {
template <class T>
T reduce_sum(const T* a, size_t nelem, int sync);


template <class T>
T reduce_logic_or(const T* a, size_t nelem, int sync);


template <class T>
void dotprod(T* ans, const T* a, const T* b, int nelem, int sync);
}
}
TINKER_NAMESPACE_END
