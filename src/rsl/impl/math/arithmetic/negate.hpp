#pragma once
#include "../../util/primitives.hpp"
#include "../util/type_util.hpp"
#include "../vector/vector.hpp"

namespace rsl::math
{
	namespace internal
	{
		template <typename Scalar, size_type Size>
		struct compute_negate
		{
			static constexpr size_type size = Size;
			using value_type = vector<Scalar, size>;

			[[nodiscard]] constexpr static value_type compute(const value_type& a) noexcept
			{
				value_type result;
				for (size_type i = 0; i < size; i++)
				{
					result[i] = -a[i];
				}
				return result;
			}
		};

		template <typename Scalar>
		struct compute_negate<Scalar, 1u>
		{
			static constexpr size_type size = 1u;
			using value_type = vector<Scalar, size>;

			[[nodiscard]] constexpr static Scalar compute(Scalar a) noexcept { return -a; }
		};
	} // namespace internal

	template <typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
	[[nodiscard]] constexpr auto negate(const vec_type& a) noexcept
	{
		return internal::compute_negate<typename vec_type::scalar, vec_type::size>::compute(a);
	}

	template <typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
	[[nodiscard]] constexpr auto operator-(const vec_type& a)
	{
		return internal::compute_negate<typename vec_type::scalar, vec_type::size>::compute(a);
	}

	template <typename vec_type, std::enable_if_t<is_vector_v<vec_type>, bool> = true>
	constexpr vec_type& negate_assign(vec_type& a) noexcept
	{
		return a = internal::compute_negate<typename vec_type::scalar, vec_type::size>::compute(a);
	}
} // namespace rsl::math
