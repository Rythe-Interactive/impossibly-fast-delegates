#pragma once
#include "normalize.hpp"

namespace rsl::math::internal
{
	template <typename T>
	struct compute_normalize;

	template <typename Scalar>
	struct compute_normalize<quaternion<Scalar>>
	{
		using quat_type = quaternion<Scalar>;

		[[nodiscard]] [[rythe_always_inline]] static quat_type compute(const quat_type& q) noexcept
		{
			return q / length(q);
		}
	};
} // namespace rsl::math::internal
