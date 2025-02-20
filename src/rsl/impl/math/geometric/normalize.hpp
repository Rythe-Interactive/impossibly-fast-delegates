#pragma once
#include <cmath>

#include "../../util/primitives.hpp"
#include "../arithmetic/division.hpp"
#include "../geometric/length.hpp"
#include "../vector/vector.hpp"

namespace rsl::math
{
	template <typename vec_type, ::std::enable_if_t<is_vector_v<vec_type>, bool> = true>
	[[nodiscard]] [[rythe_always_inline]] auto normalize(const vec_type& v) noexcept;

	template <typename quat_type, ::std::enable_if_t<is_quat_v<quat_type>, bool> = true>
	[[nodiscard]] [[rythe_always_inline]] auto normalize(const quat_type& v) noexcept;
} // namespace rsl::math

#include "normalize.inl"
