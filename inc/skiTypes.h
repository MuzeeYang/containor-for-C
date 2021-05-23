#ifndef __SKI_TYPES_H__
#define __SKI_TYPES_H__

#if (__WORDSIZE == 32 || __WORDSIZE == 64)
typedef long long			skiS64_t;
typedef unsigned long long	skiU64_t;
typedef int					skiS32_t;
typedef unsigned int		skiU32_t;
typedef short				skiS16_t;
typedef unsigned short		skiU16_t;
typedef char				skiS8_t;
typedef unsigned char		skiU8_t;
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

typedef void* skiHandler_t;
typedef void* skiPosition_t;
typedef typeof(sizeof(NULL)) skiSize_t; 
typedef skiSize_t skiIndex_t;
typedef unsigned int skiHashCode_t;
typedef int (*skiFunc1_t)(void*);
typedef int (*skiFunc2_t)(void*, void*);
typedef int (*skiFunc3_t)(void*, void*, void*);
typedef int (*skiFuncSPP_t)(char*, void*, void*);
typedef int (*skiFuncIPP_t)(skiIndex_t, void*, void*);

#define SKI_ATMC_CSA_(_v, _o, _n)	__sync_bool_compare_and_swap(&(_v), _o, _n)
#define memset __builtin_memset
#define memcpy __builtin_memcpy
#define memmove __builtin_memmove
#define memcmp __builtin_memcmp

#endif
