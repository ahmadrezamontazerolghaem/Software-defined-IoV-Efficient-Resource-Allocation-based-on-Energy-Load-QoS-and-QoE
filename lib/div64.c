#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/math64.h>
/* Not needed on 64bit architectures */
#if BITS_PER_LONG == 32

uint32_t __attribute__((weak)) __div64_32(uint64_t *n, uint32_t base)
{ 
	uint64_t rem = *n;
	uint64_t b = base;
	uint64_t res, d = 1;
	uint32_t high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;
	if (high >= base) {
		high /= base;
		res = (uint64_t) high << 32;
		rem -= (uint64_t) (high*base) << 32;
	}

	while ((int64_t)b > 0 && b < rem) {
		b = b+b;
		d = d+d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}
		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}

EXPORT_SYMBOL(__div64_32);

#ifndef div_s64_rem
s64 div_s64_rem(s64 dividend, s32 divisor, s32 *remainder)
{
	u64 quotient;

	if (dividend < 0) {
		quotient = div_u64_rem(-dividend, abs(divisor), (u32 *)remainder);
		*remainder = -*remainder;
		if (divisor > 0)
			quotient = -quotient;
	} else {
		quotient = div_u64_rem(dividend, abs(divisor), (u32 *)remainder);
		if (divisor < 0)
			quotient = -quotient;
	}
	return quotient;
}
EXPORT_SYMBOL(div_s64_rem);
#endif

/**
 * div64_u64_rem - unsigned 64bit divide with 64bit divisor and remainder
 * @dividend:	64bit dividend
 * @divisor:	64bit divisor
 * @remainder:  64bit remainder
 *
 * This implementation is a comparable to algorithm used by div64_u64.
 * But this operation, which includes math for calculating the remainder,
 * is kept distinct to avoid slowing down the div64_u64 operation on 32bit
 * systems.
 */
#ifndef div64_u64_rem
u64 div64_u64_rem(u64 dividend, u64 divisor, u64 *remainder)
{
	u32 high = divisor >> 32;
	u64 quot;

	if (high == 0) {
		u32 rem32;
		quot = div_u64_rem(dividend, divisor, &rem32);
		*remainder = rem32;
	} else {
		int n = 1 + fls(high);
		quot = div_u64(dividend >> n, divisor >> n);

		if (quot != 0)
			quot--;

		*remainder = dividend - quot * divisor;
		if (*remainder >= divisor) {
			quot++;
			*remainder -= divisor;
		}
	}

	return quot;
}
EXPORT_SYMBOL(div64_u64_rem);
#endif

/**
 * div64_u64 - unsigned 64bit divide with 64bit divisor
 * @dividend:	64bit dividend
 * @divisor:	64bit divisor
 *
 * This implementation is a modified version of the algorithm proposed
 * by the book 'Hacker's Delight'.  The original source and full proof
 * can be found here and is available for use without restriction.
 *
 * 'http://www.hackersdelight.org/HDcode/newCode/divDouble.c.txt'
 */
#ifndef div64_u64
u64 div64_u64(u64 dividend, u64 divisor)
{
	u32 high = divisor >> 32;
	u64 quot;

	if (high == 0) {
		quot = div_u64(dividend, divisor);
	} else {
		int n = 1 + fls(high);
		quot = div_u64(dividend >> n, divisor >> n);

		if (quot != 0)
			quot--;
		if ((dividend - quot * divisor) >= divisor)
			quot++;
	}

	return quot;
}
EXPORT_SYMBOL(div64_u64);
#endif

/**
 * div64_s64 - signed 64bit divide with 64bit divisor
 * @dividend:	64bit dividend
 * @divisor:	64bit divisor
 */
#ifndef div64_s64
s64 div64_s64(s64 dividend, s64 divisor)
{
	s64 quot, t;

	quot = div64_u64(abs64(dividend), abs64(divisor));
	t = (dividend ^ divisor) >> 63;

	return (quot ^ t) - t;
}
EXPORT_SYMBOL(div64_s64);
#endif

#endif /* BITS_PER_LONG == 32 */

/*
 * Iterative div/mod for use when dividend is not expected to be much
 * bigger than divisor.
 */
u32 iter_div_u64_rem(u64 dividend, u32 divisor, u64 *remainder)
{
	return __iter_div_u64_rem(dividend, divisor, remainder);
}
EXPORT_SYMBOL(iter_div_u64_rem);
