#include "linear_rand.hpp"
#pragma once

namespace rsl::math
{
	template <typename Scalar>
	[[nodiscard]] Scalar linear_rand(Scalar min, Scalar max) noexcept
	{
		if constexpr (::std::is_floating_point_v<Scalar>)
		{
			return min + (max - min) * RandomNumberGenerator::generateRandomFloat<Scalar>();
		}
		else
		{
			static_assert(
				::std::is_arithmetic_v<Scalar>,
				"Input scalar type to linear_rand is neither a scalar nor any other supported type."
			);
			return min + RandomNumberGenerator::generateRandomInteger<Scalar>() % (max - min);
		}
	}

	namespace internal
	{
		template <typename T>
		struct compute_linear_rand;

		template <typename Scalar, size_type Size>
		struct compute_linear_rand<vector<Scalar, Size>>
		{
			using vec_type = vector<Scalar, Size>;

			[[nodiscard]] static auto compute(const vec_type& min, const vec_type& max) noexcept
			{
				vec_type result;
				for (size_type i = 0; i < Size; i++)
				{
					result[i] = linear_rand(min[i], max[i]);
				}
				return result;
			}


			[[nodiscard]] static auto compute(Scalar min, Scalar max) noexcept
			{
				vec_type result;
				for (size_type i = 0; i < Size; i++)
				{
					result[i] = linear_rand(min, max);
				}
				return result;
			}
		};

		template <typename Scalar>
		struct compute_linear_rand<vector<Scalar, 1>>
		{
			[[nodiscard]] static auto compute(Scalar min, Scalar max) noexcept { return linear_rand(min, max); }
		};
	} // namespace internal

	template <
		typename vec_type, typename Scalar,
		::std::enable_if_t<is_vector_v<vec_type> && vec_type::size != 1 && !is_vector_v<Scalar>, bool>>
	[[nodiscard]] vec_type linear_rand(Scalar min, Scalar max) noexcept
	{
		return internal::compute_linear_rand<vec_type>::compute(
			static_cast<typename vec_type::scalar>(min), static_cast<typename vec_type::scalar>(max)
		);
	}

	template <
		typename vec_type0, typename vec_type1,
		::std::enable_if_t<
			is_vector_v<vec_type0> && is_vector_v<vec_type1> && vec_type0::size == vec_type1::size, bool>>
	[[nodiscard]] auto linear_rand(const vec_type0& min, const vec_type1& max) noexcept
	{
		using scalar = lowest_precision_t<typename vec_type0::scalar, typename vec_type1::scalar>;
		constexpr size_type size = math::min(vec_type0::size, vec_type1::size);
		return internal::compute_linear_rand<vector<scalar, size>>::compute(min, max);
	}
} // namespace rsl::math
