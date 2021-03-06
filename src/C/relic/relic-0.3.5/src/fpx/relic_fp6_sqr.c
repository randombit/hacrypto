/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2013 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of squaring in a sextic extension of a prime field.
 *
 * @version $Id: relic_fp6_sqr.c 1590 2013-09-03 08:20:00Z dfaranha $
 * @ingroup fpx
 */

#include "relic_core.h"
#include "relic_fp_low.h"
#include "relic_pp_low.h"

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

#if PP_EXT == BASIC || !defined(STRIP)

void fp6_sqr_basic(fp6_t c, fp6_t a) {
	fp2_t t0, t1, t2, t3, t4;

	fp2_null(t0);
	fp2_null(t1);
	fp2_null(t2);
	fp2_null(t3);
	fp2_null(t4);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		fp2_new(t2);
		fp2_new(t3);
		fp2_new(t4);

		/* t0 = a_0^2 */
		fp2_sqr(t0, a[0]);

		/* t1 = 2 * a_1 * a_2 */
		fp2_mul(t1, a[1], a[2]);
		fp2_dbl(t1, t1);

		/* t2 = a_2^2. */
		fp2_sqr(t2, a[2]);

		/* c2 = a_0 + a_2. */
		fp2_add(c[2], a[0], a[2]);

		/* t3 = (a_0 + a_2 + a_1)^2. */
		fp2_add(t3, c[2], a[1]);
		fp2_sqr(t3, t3);

		/* c2 = (a_0 + a_2 - a_1)^2. */
		fp2_sub(c[2], c[2], a[1]);
		fp2_sqr(c[2], c[2]);

		/* c2 = (c2 + t3)/2. */
		fp2_add(c[2], c[2], t3);
		fp_hlv(c[2][0], c[2][0]);
		fp_hlv(c[2][1], c[2][1]);

		/* t3 = t3 - c2 - t1. */
		fp2_sub(t3, t3, c[2]);
		fp2_sub(t3, t3, t1);

		/* c2 = c2 - t0 - t2. */
		fp2_sub(c[2], c[2], t0);
		fp2_sub(c[2], c[2], t2);

		/* c0 = t0 + t1 * E. */
		fp2_mul_nor(t4, t1);
		fp2_add(c[0], t0, t4);

		/* c1 = t3 + t2 * E. */
		fp2_mul_nor(t4, t2);
		fp2_add(c[1], t3, t4);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		fp2_free(t2);
		fp2_free(t3);
		fp2_free(t4);
	}
}

#if 0
void fp6_sqr_basic2(fp6_t c, fp6_t a) {
	fp2_t t0, t1, t2, t3, t4;

	fp2_null(t0);
	fp2_null(t1);
	fp2_null(t2);
	fp2_null(t3);
	fp2_null(t4);

	TRY {
		fp2_new(t0);
		fp2_new(t1);
		fp2_new(t2);
		fp2_new(t3);
		fp2_new(t4);

		/* t0 = a_0^2 */
		fp2_sqr(t0, a[0]);

		/* t1 = 2 * a_0 * a_1 */
		fp2_mul(t1, a[0], a[1]);
		fp2_dbl(t1, t1);

		/* t2 = (a_0 - a_1 + a_2)^2 */
		fp2_sub(t2, a[0], a[1]);
		fp2_add(t2, t2, a[2]);
		fp2_sqr(t2, t2);

		/* t3 = 2 * a_1 * a_2 */
		fp2_mul(t3, a[1], a[2]);
		fp2_dbl(t3, t3);

		/* t4 = a_2^2 */
		fp2_sqr(t4, a[2]);

		/* c_0 = t0 + E * t3 */
		fp2_mul_nor(c[0], t3);
		fp2_add(c[0], c[0], t0);

		/* c_1 = t1 + E * t4 */
		fp2_mul_nor(c[1], t4);
		fp2_add(c[1], c[1], t1);

		/* c_2 = t1 + t2 + t3 - t0 - t4 */
		fp2_add(c[2], t1, t2);
		fp2_add(c[2], c[2], t3);
		fp2_sub(c[2], c[2], t0);
		fp2_sub(c[2], c[2], t4);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		fp2_free(t0);
		fp2_free(t1);
		fp2_free(t2);
		fp2_free(t3);
		fp2_free(t4);
	}
}
#endif

#endif

#if PP_EXT == LAZYR || !defined(STRIP)

void fp6_sqr_unr(dv6_t c, fp6_t a) {
	dv2_t u0, u1, u2, u3, u4, u5;
	fp2_t t0, t1, t2, t3;

	dv2_null(u0);
	dv2_null(u1);
	dv2_null(u2);
	dv2_null(u3);
	dv2_null(u4);
	dv2_null(u5);
	fp2_null(t0);
	fp2_null(t1);
	fp2_null(t2);
	fp2_null(t3);

	TRY {
		dv2_new(u0);
		dv2_new(u1);
		dv2_new(u2);
		dv2_new(u3);
		dv2_new(u4);
		dv2_new(u5);
		fp2_new(t0);
		fp2_new(t1);
		fp2_new(t2);
		fp2_new(t3);

		/* u0 = a_0^2 */
		fp2_sqrn_low(u0, a[0]);

		/* t1 = 2 * a_1 * a_2 */
		fp2_dblm_low(t0, a[1]);

#ifdef FP_SPACE
		fp2_mulc_low(u1, t0, a[2]);
#else
		fp2_muln_low(u1, t0, a[2]);
#endif

		/* u2 = a_2^2. */
		fp2_sqrn_low(u2, a[2]);

		/* t4 = a_0 + a_2. */
		fp2_addm_low(t3, a[0], a[2]);

		/* u3 = (a_0 + a_2 + a_1)^2. */
		fp2_addm_low(t2, t3, a[1]);
		fp2_sqrn_low(u3, t2);

		/* u4 = (a_0 + a_2 - a_1)^2. */
		fp2_subm_low(t1, t3, a[1]);
		fp2_sqrn_low(u4, t1);

		/* u4 = (u4 + u3)/2. */
#ifdef FP_SPACE
		fp2_addd_low(u4, u4, u3);
#else
		fp2_addc_low(u4, u4, u3);
#endif
		fp_hlvd_low(u4[0], u4[0]);
		fp_hlvd_low(u4[1], u4[1]);

		/* u3 = u3 - u4 - u1. */
#ifdef FP_SPACE
		fp2_addd_low(u5, u1, u4);
#else
		fp2_addc_low(u5, u1, u4);
#endif
		fp2_subc_low(u3, u3, u5);

		/* c2 = u4 - u0 - u2. */
#ifdef FP_SPACE
		fp2_addd_low(u5, u0, u2);
#else
		fp2_addc_low(u5, u0, u2);
#endif
		fp2_subc_low(c[2], u4, u5);

		/* c0 = u0 + u1 * E. */
		fp2_nord_low(u4, u1);
		fp2_addc_low(c[0], u0, u4);

		/* c1 = u3 + u2 * E. */
		fp2_nord_low(u4, u2);
		fp2_addc_low(c[1], u3, u4);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		dv2_free(u0);
		dv2_free(u1);
		dv2_free(u2);
		dv2_free(u3);
		dv2_free(u4);
		dv2_free(u5);
		fp2_free(t0);
		fp2_free(t1);
		fp2_free(t2);
		fp2_free(t3);
	}
}

void fp6_sqr_lazyr(fp6_t c, fp6_t a) {
	dv6_t t;

	dv6_null(t);

	TRY {
		dv6_new(t);
		fp6_sqr_unr(t, a);
		fp2_rdcn_low(c[0], t[0]);
		fp2_rdcn_low(c[1], t[1]);
		fp2_rdcn_low(c[2], t[2]);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		dv6_free(t);
	}
}

#if 0
void fp6_sqr_lazyr2(fp6_t c, fp6_t a) {
	dv2_t u0, u1, u2, u3, u4;
	fp2_t t0, t1;

	dv2_null(u0);
	dv2_null(u1);
	dv2_null(u2);
	dv2_null(u3);
	dv2_null(u4);
	fp2_null(t0);
	fp2_null(t1);

	TRY {
		dv2_new(u0);
		dv2_new(u1);
		dv2_new(u2);
		dv2_new(u3);
		dv2_new(u4);
		fp2_new(t0);
		fp2_new(t1);

		/* u0 = a_0^2 */
		fp2_sqrn_low(u0, a[0]);

		/* u1 = 2 * a_0 * a_1 */
		fp2_dbl(t0, a[0]);

		/* u3 = 2 * a_1 * a_2 */
		fp2_dbl(t1, a[1]);

#ifdef FP_SPACE
		fp2_mulc_low(u1, t0, a[1]);
		fp2_mulc_low(u3, t1, a[2]);
#else
		fp2_muln_low(u1, t0, a[1]);
		fp2_muln_low(u3, t1, a[2]);
#endif
		/* u2 = (a_0 - a_1 + a_2)^2 */
		fp2_sub(t0, a[0], a[1]);
		fp2_add(t0, t0, a[2]);
		fp2_sqrn_low(u2, t0);

		/* c_0 = u0 + E * u3 */
		fp2_nord_low(u4, u3);
		fp2_addc_low(u4, u4, u0);
		fp2_rdcn_low(c[0], u4);

		/* t4 = a_2^2 */
		fp2_sqrn_low(u4, a[2]);

		/* c_2 = u1 + u2 + u3 - u0 - u4 */
#ifdef FP_SPACE
		fp2_addd_low(u0, u0, u4);
		fp2_addd_low(u3, u3, u1);
		fp2_addd_low(u3, u3, u2);
		fp2_subc_low(u3, u3, u0);
#else
		fp2_addc_low(u0, u0, u4);
		fp2_addc_low(u3, u3, u1);
		fp2_subc_low(u3, u3, u0);
		fp2_addc_low(u3, u3, u2);
#endif
		fp2_rdcn_low(c[2], u3);

		/* c_1 = u1 + E * u4 */
		fp2_nord_low(u2, u4);
		fp2_addc_low(u2, u2, u1);
		fp2_rdcn_low(c[1], u2);
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		dv2_free(u0);
		dv2_free(u1);
		dv2_free(u2);
		dv2_free(u3);
		dv2_free(u4);
		fp2_free(t0);
		fp2_free(t1);
	}
}
#endif

#endif
