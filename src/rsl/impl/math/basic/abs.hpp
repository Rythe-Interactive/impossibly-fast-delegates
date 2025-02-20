#pragma once
#include <cmath>
#include <limits>

#include "../../defines.hpp"
#include "../../util/primitives.hpp"
#include "../../util/utilities.hpp"
#include "../util/type_util.hpp"

namespace rsl::math
{
	namespace internal
	{
		template <typename T>
		[[nodiscard]] [[rythe_always_inline]] static auto _abs_impl_(T val)
		{
			using value_type = ::std::remove_cvref_t<T>;
			if constexpr (::std::is_floating_point_v<value_type>)
			{
				value_type copy = val;
				byte* significantByte = reinterpret_cast<byte*>(&copy);

				if constexpr (endian::native == endian::little)
				{
					significantByte += sizeof(value_type) - 1;
				}

				*significantByte &= static_cast<byte>(0b0111'1111);

				return copy;
			}
			else
			{
				return val < 0 ? -val : val;
			}
		}

		template <typename Scalar, size_type Size>
		struct compute_abs
		{
			static constexpr size_type size = Size;
			using value_type = vector<Scalar, size>;

			[[nodiscard]] static value_type compute(const value_type& val) noexcept
			{
				if constexpr (!::std::is_signed_v<Scalar>)
				{
					return val;
				}
				else
				{
					value_type result;
					for (size_type i = 0; i < size; i++)
					{
						result[i] = internal::_abs_impl_(val[i]);
					}
					return result;
				}
			}
		};

		template <typename Scalar>
		struct compute_abs<Scalar, 1u>
		{
			static constexpr size_type size = 1u;
			using value_type = vector<Scalar, size>;

			[[nodiscard]] static Scalar compute(Scalar val) noexcept
			{
				if constexpr (!::std::is_signed_v<Scalar>)
				{
					return val;
				}
				else
				{
					return internal::_abs_impl_(val);
				}
			}
		};
	} // namespace internal

	template <typename T>
	[[nodiscard]] [[rythe_always_inline]] static auto abs(T val)
	{
		using value_type = ::std::remove_cvref_t<T>;
		if constexpr (is_vector_v<value_type>)
		{
			return internal::compute_abs<typename value_type::scalar, value_type::size>::compute(val);
		}
		else if constexpr (!::std::is_signed_v<value_type>)
		{
			return val;
		}
		else
		{
			return internal::_abs_impl_(val);
		}
	}
} // namespace rsl::math
