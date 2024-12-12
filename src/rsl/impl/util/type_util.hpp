#pragma once
#include <concepts>
#include <ratio>
#include <utility>

#include "common.hpp"

#include "../defines.hpp"
#include "string_util.hpp"

namespace rsl
{
	// I'm choosing to go with the constraint option instead of SFINAE, the Microsoft STL has choosen different.
	// See https://github.com/microsoft/STL/issues/2620 and https://github.com/microsoft/STL/pull/2624 for further
	// context. Theoretically the constraint should be superior cross compilers, but bugs inside MSVC has in the past
	// prevented it from working propperly in all edge cases. I don't care about these edge cases nor do I care about
	// past versions of MSVC.
	template <typename T, typename... Args>
		requires requires(void* ptr, Args&&... args) { ::new (ptr) T(static_cast<Args&&>(args)...); }
	constexpr T* construct_at(T* const location, Args&&... args)
		noexcept(noexcept(::new(static_cast<void*>(location)) T(::rsl::forward<Args>(args)...)))
	{
		return ::new (static_cast<void*>(location)) T(::rsl::forward<Args>(args)...);
	}

	namespace internal
	{
		template <class NoThrowFwdIt, class NoThrowSentinel>
		constexpr void destroy_range(NoThrowFwdIt first, const NoThrowSentinel last) noexcept;

		template <class T>
		constexpr void destroy_in_place(T& val) noexcept
		{
			if constexpr (is_array_v<T>)
			{
				destroy_range(val, val + extent_v<T>);
			}
			else
			{
				val.~T();
			}
		}

		template <class NoThrowFwdIt, class NoThrowSentinel>
		constexpr void destroy_range(NoThrowFwdIt first, const NoThrowSentinel last) noexcept
		{
			// note that this is an optimization for debug mode codegen; in release mode the BE removes all of this
			if constexpr (!::std::is_trivially_destructible_v<::std::iter_value_t<NoThrowFwdIt>>)
			{
				for (; first != last; ++first)
				{
					destroy_in_place(*first);
				}
			}
		}
	} // namespace internal

	template <typename T, size_type Size>
	[[nodiscard]] constexpr T* begin(T (&arr)[Size]) noexcept;

	template <typename T, size_type Size>
	[[nodiscard]] constexpr T* end(T (&arr)[Size]) noexcept;

	template <typename T>
	constexpr void destroy_at(T* const location) noexcept
	{
		if constexpr (is_array_v<T>)
		{
			internal::destroy_range(::rsl::begin(*location), ::rsl::end(*location));
		}
		else
		{
			location->~T();
		}
	}

	template <template <typename> typename Compare, typename T, T A, T B>
	struct do_compare
	{
		static constexpr inline Compare<T> comp{};
		static constexpr inline bool value = comp(A, B);
	};

	template <template <typename> typename Compare, typename T, T A, T B>
	inline constexpr bool do_compare_v = do_compare<Compare, T, A, B>::value;

	template <rsl::size_type I, typename Type, typename... Types>
	struct element_at : element_at<I - 1, Types...>
	{
	};

	template <typename Type, typename... Types>
	struct element_at<0, Type, Types...>
	{
		using type = Type;
	};

	template <rsl::size_type I, typename Type, typename... Types>
	using element_at_t = typename element_at<I, Type, Types...>::type;

	template <typename T, typename... Args>
	struct is_brace_constructible
	{
		static constexpr bool value = requires(Args&&... args) { T{::rsl::forward<Args>(args)...}; };
	};

	template <typename T, typename... Args>
	inline constexpr bool is_brace_constructible_v = is_brace_constructible<T, Args...>::value;


	template <rsl::size_type I, typename Check, typename...>
	struct element_at_is_same_as;

	template <rsl::size_type I, typename Check, typename Type, typename... Types>
	struct element_at_is_same_as<I, Check, Type, Types...> : element_at_is_same_as<I - 1, Check, Type, Types...>
	{
	};

	template <typename Check, typename Type, typename... Types>
	struct element_at_is_same_as<0, Check, Type, Types...>
	{
		static constexpr bool value = is_same_v<Check, Type>;
	};

	template <rsl::size_type I, typename Check>
	struct element_at_is_same_as<I, Check>
	{
		static constexpr bool value = false;
	};

	template <rsl::size_type I, typename Check, typename... Types>
	inline constexpr bool element_at_is_same_as_v = element_at_is_same_as<I, Check, Types...>::value;

	struct lval_ref_signal
	{
	};

	using ref_signal = lval_ref_signal;

	struct rval_ref_signal
	{
	};

	using move_signal = rval_ref_signal;

	struct const_signal
	{
	};

	struct pointer_signal
	{
	};

	template <typename T, typename... DecorationSignals>
	struct decorate_type;

	template <typename T, typename DecorationSignal>
	struct decorate_type<T, DecorationSignal>
	{
		static_assert(false, "Uknown signal.");
	};

	template <typename T>
	struct decorate_type<T>
	{
		using type = T;
	};

	template <typename T, typename DecorationSignal, typename... Rest>
	struct decorate_type<T, DecorationSignal, Rest...>
	{
		using type = decorate_type<typename decorate_type<T, DecorationSignal>::type, Rest...>::type;
	};

	template <typename T>
	struct decorate_type<T, lval_ref_signal>
	{
		using type = typename add_lval_ref<T>::type;
	};

	template <typename T>
	struct decorate_type<T, rval_ref_signal>
	{
		using type = typename add_rval_ref<T>::type;
	};

	template <typename T>
	struct decorate_type<T, const_signal>
	{
		using type = typename add_const<T>::type;
	};

	template <typename T>
	struct decorate_type<T, pointer_signal>
	{
		using type = typename add_pointer<T>::type;
	};

	template <typename T, typename... DecorationSignals>
	using decorate_type_t = decorate_type<T, DecorationSignals...>::type;

	template <typename T, typename = void>
	struct is_always_equal
	{
		using type = bool_constant<::rsl::is_empty_v<T>>;
	};

	template <typename T>
	struct is_always_equal<T, void_t<typename T::is_always_equal>>
	{
		using type = typename T::is_always_equal;
	};

	template <typename T>
	using is_always_equal_t = is_always_equal<T>::type;

	template <typename Derived, typename Base>
	concept derived_from =
		::rsl::is_base_of_v<Base, Derived> && ::rsl::is_convertible_v<const volatile Derived*, const volatile Base*>;

	template <typename From, typename To>
	concept convertible_to = ::rsl::is_convertible_v<From, To> && requires { static_cast<To>(::rsl::declval<From>()); };

	template <typename LHS, typename RHS>
	concept assignable_from =
		::rsl::is_lvalue_reference_v<LHS> &&
		::std::common_reference_with<const ::rsl::remove_reference_t<LHS>&, const ::rsl::remove_reference_t<RHS>&> &&
		requires(LHS lhs, RHS&& rhs) {
			{ lhs = static_cast<RHS&&>(rhs) } -> same_as<LHS>;
		};

	template <typename T>
	concept destructible = ::std::destructible<T>; // Compiler magic behind the scenes.

	template <typename T, typename... Args>
	concept constructible_from = ::std::constructible_from<T, Args...>; // Compiler magic behind the scenes.

	template <typename T>
	concept default_initializable = constructible_from<T> && requires {
		T{};
		::new (static_cast<void*>(nullptr)) T; // is-default-initializable<T>
	};

	template <typename T>
	concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

	template <typename T>
	concept copy_constructible = move_constructible<T> && constructible_from<T, T&> && convertible_to<T&, T> &&
								 constructible_from<T, const T&> && convertible_to<const T&, T> &&
								 constructible_from<T, const T> && convertible_to<const T, T>;

	template <typename T>
	concept movable = is_object_v<T> && move_constructible<T> && assignable_from<T&, T> && ::std::swappable<T>;

	template <typename T>
	concept copyable = copy_constructible<T> && movable<T> && assignable_from<T&, T&> &&
					   assignable_from<T&, const T&> && assignable_from<T&, const T>;

	template <typename Func, typename... Args>
	concept invocable = requires(Func&& func, Args&&... args) { _STD invoke(static_cast<Func&&>(func), static_cast<Args&&>(args)...);
	};

#define RYTHE_HAS_FUNC(x)                                                                                              \
	namespace internal                                                                                                 \
	{                                                                                                                  \
		template <typename, typename T>                                                                                \
		struct RYTHE_CONCAT(has_, RYTHE_CONCAT(x, _impl))                                                              \
		{                                                                                                              \
			static_assert(                                                                                             \
				::rsl::integral_constant<bool_t<T>, false>::value,                                                     \
				"Second template param needs to be of function type."                                                  \
			);                                                                                                         \
		};                                                                                                             \
                                                                                                                       \
		template <typename C, typename Ret, typename... Args>                                                          \
		struct RYTHE_CONCAT(has_, RYTHE_CONCAT(x, _impl))<C, Ret(Args...)>                                             \
		{                                                                                                              \
			static constexpr bool value = requires(C& val, Args&&... args) {                                           \
				{ val.x(::rsl::forward<Args>(args)...) } -> ::rsl::convertible_to<Ret>;                                \
			};                                                                                                         \
		};                                                                                                             \
	}                                                                                                                  \
                                                                                                                       \
	template <typename C, typename F>                                                                                  \
	constexpr bool RYTHE_CONCAT(has_, RYTHE_CONCAT(x, _v)) =                                                           \
		RYTHE_CONCAT(internal::has_, RYTHE_CONCAT(x, _impl))<C, F>::value;                                             \
                                                                                                                       \
	template <typename C, typename F>                                                                                  \
	concept RYTHE_CONCAT(has_, x) = RYTHE_CONCAT(has_, RYTHE_CONCAT(x, _v))<C, F>;                                     \
                                                                                                                       \
	namespace internal                                                                                                 \
	{                                                                                                                  \
		template <typename, typename T>                                                                                \
		struct RYTHE_CONCAT(has_static_, RYTHE_CONCAT(x, _impl))                                                       \
		{                                                                                                              \
			static_assert(                                                                                             \
				::rsl::integral_constant<bool_t<T>, false>::value,                                                     \
				"Second template param needs to be of function type."                                                  \
			);                                                                                                         \
		};                                                                                                             \
                                                                                                                       \
		template <typename C, typename Ret, typename... Args>                                                          \
		struct RYTHE_CONCAT(has_static_, RYTHE_CONCAT(x, _impl))<C, Ret(Args...)>                                      \
		{                                                                                                              \
			static constexpr bool value = requires(Args&&... args) {                                                   \
				{ C::x(::rsl::forward<Args>(args)...) } -> ::rsl::convertible_to<Ret>;                                 \
			};                                                                                                         \
		};                                                                                                             \
	}                                                                                                                  \
                                                                                                                       \
	template <typename C, typename F>                                                                                  \
	constexpr bool RYTHE_CONCAT(has_static_, RYTHE_CONCAT(x, _v)) =                                                    \
		RYTHE_CONCAT(internal::has_static_, RYTHE_CONCAT(x, _impl))<C, F>::value;                                      \
                                                                                                                       \
	template <typename C, typename F>                                                                                  \
	concept RYTHE_CONCAT(has_static_, x) = RYTHE_CONCAT(has_static_, RYTHE_CONCAT(x, _v))<C, F>;
} // namespace rsl
