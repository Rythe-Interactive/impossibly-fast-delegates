#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace rsl
{
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;
	using uint_max = std::uintmax_t;

	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;
	using int_max = std::intmax_t;

	using size_type = std::size_t;
	using index_type = std::size_t;
	using diff_type = std::ptrdiff_t;
	using ptr_type = std::uintptr_t;

	using float32 = float;
	using float64 = double;
	using float_max = long double;
    
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

    using f32 = float32;
    using f64 = float64;
	using flt_max = float_max;

	using time32 = float32;
	using time64 = float64;
	using time_large = int_max;

	using cstring = const char*;

	using uint = uint32;

	using byte = uint8;
	using byte_vec = std::vector<byte>;

	using bitfield8 = byte;
	using bitfield16 = uint16;
	using bitfield32 = uint32;
	using bitfield64 = uint64;

	using priority_type = int8;
#define default_priority 0
#define PRIORITY_MAX CHAR_MAX
#define PRIORITY_MIN CHAR_MIN

	using id_type = size_type;

#define invalid_id 0

	using days = int32;
	using hours = int32;
	using minutes = int32;
	using seconds = time32;
	using milli_seconds = time32;
	using micro_seconds = time64;
	using nano_seconds = time_large;

    using days_f = time32;
	using hours_f = time32;
	using minutes_f = time32;
	using seconds_f = time32;
	using milli_seconds_f = time32;
	using micro_seconds_f = time64;
	using nano_seconds_f = float_max;

	enum npos_type : size_type
	{
	};

	constexpr npos_type npos = static_cast<npos_type>(-1);
	static_assert(npos == std::string::npos);
} // namespace rsl
