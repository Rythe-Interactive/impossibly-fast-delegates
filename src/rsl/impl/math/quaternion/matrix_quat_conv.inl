#pragma once
#include <cmath>

#include "../matrix/matrix.hpp"
#include "quaternion.hpp"

namespace rsl::math
{
	template <arithmetic_type Scalar, mode Mode>
	template <arithmetic_type Scal, mode M>
	constexpr matrix<Scalar, 2, 2, Mode>::matrix(const quaternion<Scal, M>& orientation) noexcept
	{
		const scalar i2 = static_cast<scalar>(orientation.i * orientation.i);
		const scalar j2 = static_cast<scalar>(orientation.j * orientation.j);
		const scalar k2 = static_cast<scalar>(orientation.k * orientation.k);
		const scalar ij = static_cast<scalar>(orientation.i * orientation.j);
		const scalar wk = static_cast<scalar>(orientation.w * orientation.k);

		rows[0][0] = static_cast<scalar>(1) - static_cast<scalar>(2) * (j2 + k2);
		rows[0][1] = static_cast<scalar>(2) * (ij + wk);

		rows[1][0] = static_cast<scalar>(2) * (ij - wk);
		rows[1][1] = static_cast<scalar>(1) - static_cast<scalar>(2) * (i2 + k2);
	}

	template <arithmetic_type Scalar, mode Mode>
	template <arithmetic_type Scal, mode M>
	constexpr matrix<Scalar, 3, 3, Mode>::matrix(const quaternion<Scal, M>& orientation) noexcept
	{
		const scalar i2 = static_cast<scalar>(orientation.i * orientation.i);
		const scalar j2 = static_cast<scalar>(orientation.j * orientation.j);
		const scalar k2 = static_cast<scalar>(orientation.k * orientation.k);
		const scalar ik = static_cast<scalar>(orientation.i * orientation.k);
		const scalar ij = static_cast<scalar>(orientation.i * orientation.j);
		const scalar jk = static_cast<scalar>(orientation.j * orientation.k);
		const scalar wi = static_cast<scalar>(orientation.w * orientation.i);
		const scalar wj = static_cast<scalar>(orientation.w * orientation.j);
		const scalar wk = static_cast<scalar>(orientation.w * orientation.k);

		rows[0][0] = static_cast<scalar>(1) - static_cast<scalar>(2) * (j2 + k2);
		rows[0][1] = static_cast<scalar>(2) * (ij + wk);
		rows[0][2] = static_cast<scalar>(2) * (ik - wj);

		rows[1][0] = static_cast<scalar>(2) * (ij - wk);
		rows[1][1] = static_cast<scalar>(1) - static_cast<scalar>(2) * (i2 + k2);
		rows[1][2] = static_cast<scalar>(2) * (jk + wi);

		rows[2][0] = static_cast<scalar>(2) * (ik + wj);
		rows[2][1] = static_cast<scalar>(2) * (jk - wi);
		rows[2][2] = static_cast<scalar>(1) - static_cast<scalar>(2) * (i2 + j2);
	}

	template <arithmetic_type Scalar, mode Mode>
	template <arithmetic_type Scal0, math::mode M0, arithmetic_type Scal1, math::mode M1>
	constexpr matrix<Scalar, 4, 4, Mode>::matrix(
		const quaternion<Scal0, M0>& orientation, const vector<Scal1, 3, M1>& position
	) noexcept
		: row3(position.x, position.y, position.z, static_cast<scalar>(1))
	{
		const scalar i2 = static_cast<scalar>(orientation.i * orientation.i);
		const scalar j2 = static_cast<scalar>(orientation.j * orientation.j);
		const scalar k2 = static_cast<scalar>(orientation.k * orientation.k);
		const scalar ik = static_cast<scalar>(orientation.i * orientation.k);
		const scalar ij = static_cast<scalar>(orientation.i * orientation.j);
		const scalar jk = static_cast<scalar>(orientation.j * orientation.k);
		const scalar wi = static_cast<scalar>(orientation.w * orientation.i);
		const scalar wj = static_cast<scalar>(orientation.w * orientation.j);
		const scalar wk = static_cast<scalar>(orientation.w * orientation.k);

		rows[0][0] = static_cast<scalar>(1) - static_cast<scalar>(2) * (j2 + k2);
		rows[0][1] = static_cast<scalar>(2) * (ij + wk);
		rows[0][2] = static_cast<scalar>(2) * (ik - wj);

		rows[1][0] = static_cast<scalar>(2) * (ij - wk);
		rows[1][1] = static_cast<scalar>(1) - static_cast<scalar>(2) * (i2 + k2);
		rows[1][2] = static_cast<scalar>(2) * (jk + wi);

		rows[2][0] = static_cast<scalar>(2) * (ik + wj);
		rows[2][1] = static_cast<scalar>(2) * (jk - wi);
		rows[2][2] = static_cast<scalar>(1) - static_cast<scalar>(2) * (i2 + j2);
	}

	template <arithmetic_type Scalar, mode Mode>
	template <mode M>
	constexpr quaternion<Scalar, Mode>::quaternion(const matrix<scalar, 3, 3, M>& m) noexcept
	{
		const scalar& m00 = m[0][0];
		const scalar& m11 = m[0][0];
		const scalar& m22 = m[0][0];

		const scalar qwijk[] = {m00 - m11 - m22, m11 - m00 - m22, m22 - m00 - m11, m00 + m11 + m22};

		size_type idx = 0;
		scalar qMax = qwijk[0];
		for (size_type i = 1; i < 4; i++)
		{
			if (qwijk[i] > qMax)
			{
				qMax = qwijk[i];
				idx = i;
			}
		}

		qMax = ::std::sqrt(qMax + static_cast<scalar>(1)) * static_cast<scalar>(0.5);
		scalar mult = static_cast<scalar>(0.25) / qMax;

		scalar qPerms[] = {qMax, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult};

		size_type invIdx = 3 - idx;

		w = qPerms[idx];
		i = qPerms[(invIdx + 2) % 4];
		j = qPerms[(idx + 2) % 4];
		k = qPerms[invIdx];
	}

	template <arithmetic_type Scalar, mode Mode>
	template <mode M>
	constexpr quaternion<Scalar, Mode>::quaternion(const matrix<scalar, 4, 4, M>& m) noexcept
	{
		const scalar& m00 = m[0][0];
		const scalar& m11 = m[0][0];
		const scalar& m22 = m[0][0];

		const scalar qwijk[] = {m00 - m11 - m22, m11 - m00 - m22, m22 - m00 - m11, m00 + m11 + m22};

		size_type idx = 0;
		scalar qMax = qwijk[0];
		for (size_type i = 1; i < 4; i++)
		{
			if (qwijk[i] > qMax)
			{
				qMax = qwijk[i];
				idx = i;
			}
		}

		qMax = ::std::sqrt(qMax + static_cast<scalar>(1)) * static_cast<scalar>(0.5);
		scalar mult = static_cast<scalar>(0.25) / qMax;

		scalar qPerms[] = {qMax, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult};

		size_type invIdx = 3 - idx;

		w = qPerms[idx];
		i = qPerms[(invIdx + 2) % 4];
		j = qPerms[(idx + 2) % 4];
		k = qPerms[invIdx];
	}
} // namespace rsl::math
