/*************************************************
 * Anthor  : LuoZhongYao@gmail.com
 * Modified: 2016 Sep 24
 ************************************************/
#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdbool.h>

__BEGIN_DECLS

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
# define __same_type(a, b) __builtin_types_compatible_p(__typeof__(a), __typeof__(b))
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))

#define container_of(ptr, type, member) ({                      \
        const __typeof__( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define STREAM_TO_U16(p)  (((u8*)p)[0] | ((u8*)p)[1] << 8)
#define STREAM_TO_U32(p)  (((u8*)p)[0] | ((u8*)p)[1] << 8 | ((u8*)p)[2] << 16 | ((u8*)p)[3] << 24)

extern void ms_delay(u32 timeout);

__END_DECLS

#endif

