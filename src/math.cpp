
#include "math.hpp"

#if __SSE2__
#	include <emmintrin.h>
static __m128d _sse2_min_0 = _mm_set_sd(0);
static __m128d _sse2_max_1 = _mm_set_sd(1);
#endif

double Math::clamp( double value ) {

#if __SSE2__
	_mm_store_sd( &value, _mm_min_sd( _mm_max_sd( _mm_set_sd(value), _sse2_min_0), _sse2_max_1 ) );
	return value;
#else
	return value < 0 ? 0 : value > 1 ? 1 : value;
#endif

}
