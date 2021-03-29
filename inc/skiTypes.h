#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

#if (__WORDSIZE == 32 || __WORDSIZE == 64)
typedef long long			int64_t;
typedef unsigned long long	uint64_t;
typedef int					int32_t;
typedef unsigned int		uint32_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef char				int8_t;
typedef unsigned char		uint8_t;
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif
