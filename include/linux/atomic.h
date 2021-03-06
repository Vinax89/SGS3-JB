#ifndef _LINUX_ATOMIC_H
#define _LINUX_ATOMIC_H
#include <asm/atomic.h>

/**
 * atomic_inc_not_zero_hint - increment if not null
 * @v: pointer of type atomic_t
 * @hint: probable value of the atomic before the increment
 *
 * This version of atomic_inc_not_zero() gives a hint of probable
 * value of the atomic. This helps processor to not read the memory
 * before doing the atomic read/modify/write cycle, lowering
 * number of bus transactions on some arches.
 *
 * Returns: 0 if increment was not done, 1 otherwise.
 */
#ifndef atomic_inc_not_zero_hint
static inline int atomic_inc_not_zero_hint(atomic_t *v, int hint)
{
	int val, c = hint;

	/* sanity test, should be removed by compiler if hint is a constant */
	if (!hint)
		return atomic_inc_not_zero(v);

	do {
		val = atomic_cmpxchg(v, c, c + 1);
		if (val == c)
			return 1;
		c = val;
	} while (c);

	return 0;
}
#endif

#ifdef CONFIG_ZSWAP
/*
 * atomic_dec_if_positive - decrement by 1 if old value positive
 * @v: pointer of type atomic_t
 *
 * The function returns the old value of *v minus 1, even if
 * the atomic variable, v, was not decremented.
 */
#ifndef atomic_dec_if_positive
static inline int atomic_dec_if_positive(atomic_t *v)
{
	int c, old, dec;
	c = atomic_read(v);
	for (;;) {
		dec = c - 1;
		if (unlikely(dec < 0))
			break;
		old = atomic_cmpxchg((v), c, dec);
		if (likely(old == c))
			break;
		c = old;
	}
	return dec;
}
#endif
#endif /* CONFIG_ZSWAP */

#ifndef CONFIG_ARCH_HAS_ATOMIC_OR
static inline void atomic_or(int i, atomic_t *v)
{
	int old;
	int new;

	do {
		old = atomic_read(v);
		new = old | i;
	} while (atomic_cmpxchg(v, old, new) != old);
}
#endif /* #ifndef CONFIG_ARCH_HAS_ATOMIC_OR */

#endif /* _LINUX_ATOMIC_H */
