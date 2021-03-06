#include "moar.h"
#include <math.h>

#ifdef _WIN32
#include <float.h>
#endif

#ifdef INFINITY
const MVMnum64 MVM_NUM_POSINF =  INFINITY;
const MVMnum64 MVM_NUM_NEGINF = -INFINITY;
#else
#ifdef _MSC_VER
#define MVM_NUM_POSINF  (DBL_MAX+DBL_MAX)
#define MVM_NUM_NEGINF -(DBL_MAX+DBL_MAX)
#else
const MVMnum64 MVM_NUM_POSINF =  1.0 / 0.0;
const MVMnum64 MVM_NUM_NEGINF = -1.0 / 0.0;
#endif
#endif

#ifdef NAN
const MVMnum64 MVM_NUM_NAN = NAN;
#else
#ifdef _MSC_VER
#define MVM_NUM_NAN (MVM_NUM_POSINF-MVM_NUM_POSINF)
#else
const MVMnum64 MVM_NUM_NAN = 0.0 / 0.0;
#endif
#endif

MVMint64 MVM_num_isnanorinf(MVMThreadContext *tc, MVMnum64 n) {
    return n == MVM_NUM_POSINF || n == MVM_NUM_NEGINF || n != n;
}

MVMnum64 MVM_num_posinf(MVMThreadContext *tc) {
    return MVM_NUM_POSINF;
}

MVMnum64 MVM_num_neginf(MVMThreadContext *tc) {
    return MVM_NUM_NEGINF;
}

MVMnum64 MVM_num_nan(MVMThreadContext *tc) {
    return MVM_NUM_NAN;
}
