// Template, major revision 6
// IGAD/NHTV - Jacco Bikker - 2006-2014

#pragma once

#include "math.h"
#include "stdlib.h"
#include "emmintrin.h"
#include "stdio.h"
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "glm/glm.hpp"

inline float Rand( float range ) { return ((float)rand() / RAND_MAX) * range; }
inline int IRand( int range ) { return rand() % range; }
int filesize( FILE* f );
#define MALLOC64(x) _aligned_malloc(x,64)
#define FREE64(x) _aligned_free(x)

namespace Tmpl8 {

#define MIN(a,b) (((a)>(b))?(b):(a))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define PI					3.14159265358979323846264338327950288419716939937510582097494459072381640628620899862803482534211706798f

#define PREFETCH(x)			_mm_prefetch((const char*)(x),_MM_HINT_T0)
#define PREFETCH_ONCE(x)	_mm_prefetch((const char*)(x),_MM_HINT_NTA)
#define PREFETCH_WRITE(x)	_m_prefetchw((const char*)(x))
#define loadss(mem)			_mm_load_ss((const float*const)(mem))
#define broadcastps(ps)		_mm_shuffle_ps((ps),(ps), 0)
#define broadcastss(ss)		broadcastps(loadss((ss)))

inline unsigned __int64 RDTSC()
{
   _asm _emit 0x0F
   _asm _emit 0x31
}

class TimerRDTSC
{
   unsigned __int64  start_cycle;
   unsigned __int64  end_cycle;
public:
   inline void Start() { start_cycle = RDTSC(); }
   inline void Stop() { end_cycle = RDTSC(); }
   unsigned __int64 Interval() { return end_cycle - start_cycle; }
};

struct timer 
{ 
	typedef long long value_type; 
	static double inv_freq; 
	value_type start; 
	timer() : start( get() ) { init(); } 
	float elapsed() const { return (float)((get() - start) * 0.001); } 
	static value_type get() 
	{ 
		LARGE_INTEGER c; 
		QueryPerformanceCounter( &c ); 
		return c.QuadPart; 
	} 
	static double to_time(const value_type vt) { return double(vt) * inv_freq; } 
	void reset() { start = get(); }
	static void init() 
	{ 
		LARGE_INTEGER f; 
		QueryPerformanceFrequency( &f ); 
		inv_freq = 1000./double(f.QuadPart); 
	} 
}; 

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned char byte;

#define BADFLOAT(x) ((*(uint*)&x & 0x7f000000) == 0x7f000000)

}; // namespace Tmpl8