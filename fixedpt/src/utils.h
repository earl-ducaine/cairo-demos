/* Generic useful macro definitions */

#ifndef UTILS_H
#define UTILS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#undef ABS
#define ABS(x) ((x) < 0 ? -(x) : (x))

/**
 * Returns the smaller of two values.
 *
 * @param a The first item to compare.
 * @param b The second item to compare.
 * @return The lesser of the two values.
 */
#undef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * Returns the larger of two values.
 *
 * @param a The first item to compare.
 * @param b The second item to compare.
 * @return The greater of the two values.
 */
#undef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * Returns a value constrained between a minimum and maximum.
 * Effectively returns MIN(min, MAX(max, x))
 */
#undef CLAMP
#define CLAMP(x,min,max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#undef  ARRAY_LENGTH
#define ARRAY_LENGTH(__array) ((int) (sizeof (__array) / sizeof (__array[0])))

#undef STRINGIFY
#undef STRINGIFY_ARG
#define STRINGIFY(macro_or_string)    STRINGIFY_ARG(macro_or_string)
#define STRINGIFY_ARG(contents)       #contents

#undef DEGREES_TO_RADIANS
#define DEGREES_TO_RADIANS(x) ((x) * (LUXOR_PI / 180.f))

#undef RADIANS_TO_DEGREES
#define RADIANS_TO_DEGREES(x) ((x) * (180.f / LUXOR_PI))

/**
 * Returns a pointer to the containing struct of a given member item.
 *
 * To demonstrate, the following example retrieves a pointer to
 * `example_container` given only its `destroy_foo` member:
 *
 * @code
 * struct example_container {
 *     struct foo_t destroy_foo;
 *     // other members...
 * };
 *
 * void example_container_destroy(struct foo_t *foo, void *data)
 * {
 *     struct example_container *ctr;
 *
 *     ctr = luxor_container_of(foo, ctr, destroy_foo);
 *     // destroy ctr...
 * }
 * @endcode
 *
 * @param ptr A valid pointer to the contained item.
 * @param type A pointer to the type of content that the list item
 *        stores. Type does not need be a valid pointer; a null or an
 *        uninitialised pointer will suffice.
 * @param member The named location of ptr within the sample type.
 * @return The container for the specified pointer.
 */
#undef container_of
#define container_of(ptr, type, member) ({                              \
	const __typeof__( ((type *)0)->member ) *__mptr = (ptr);        \
	(type *)( (char *)__mptr - offsetof(type, member) );})

static inline void *
zalloc(size_t s)
{
	return calloc(1, s);
}

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

#ifdef  __cplusplus
}
#endif

#endif /* UTILS_H */
