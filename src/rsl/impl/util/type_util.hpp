#pragma once
#include <concepts>
#include <ratio>
#include <type_traits>
#include <utility>

#include "../defines.hpp"
#include "primitives.hpp"
#include "string_util.hpp"

namespace rsl
{
	template <typename T>
	[[nodiscard]] T* addressof(T& val) noexcept
	{
		return ::std::addressof(val); // Compiler magic behind the scenes.
	}

	template <typename T, T Val>
	struct integral_constant
	{
		static constexpr T value = Val;

		using value_type = T;
		using type = integral_constant;

		constexpr operator value_type() const noexcept { return value; }

		[[nodiscard]] constexpr value_type operator()() const noexcept { return value; }
	};

	template <typename>
	inline constexpr bool always_false_v = false; // False value attached to a dependent name (for static_assert).

	template <bool Val>
	using bool_constant = integral_constant<bool, Val>;

	using true_type = bool_constant<true>;
	using false_type = bool_constant<false>;

	template <typename T>
	inline constexpr bool is_empty_v = ::std::is_empty_v<T>; // Compiler magic behind the scenes.

	template <typename T>
	struct is_empty : bool_constant<is_empty_v<T>>
	{
	};

	template <typename>
	inline constexpr bool is_pointer_v = false;

	template <typename T>
	inline constexpr bool is_pointer_v<T*> = true;

	template <typename T>
	inline constexpr bool is_pointer_v<T* const> = true;

	template <typename T>
	inline constexpr bool is_pointer_v<T* volatile> = true;

	template <typename T>
	inline constexpr bool is_pointer_v<T* const volatile> = true;

	template <typename T>
	struct is_pointer : bool_constant<is_pointer_v<T>>
	{
	};

	template <typename, typename>
	inline constexpr bool is_same_v = false;

	template <typename T>
	inline constexpr bool is_same_v<T, T> = true;

	template <typename LHS, typename RHS>
	struct is_same : bool_constant<is_same_v<LHS, RHS>>
	{
	};

	template <bool Test, typename TrueType, typename FalseType>
	struct conditional
	{
		using type = TrueType;
	};

	template <typename TrueType, typename FalseType>
	struct conditional<false, TrueType, FalseType>
	{
		using type = FalseType;
	};

	template <bool Test, typename TrueType, typename FalseType>
	using conditional_t = typename conditional<Test, TrueType, FalseType>::type;

	namespace internal
	{
		template <bool First_value, typename First, typename... Rest>
		struct conjunction_impl
		{ // Handle false trait or last trait.
			using type = First;
		};

		template <typename True, typename Next, typename... Rest>
		struct conjunction_impl<true, True, Next, Rest...>
		{ // The first trait is true, try the next one.
			using type = typename conjunction_impl<Next::value, Next, Rest...>::type;
		};

		template <bool First_value, typename First, typename... Rest>
		struct disjuction_impl
		{ // Handle true trait or last trait.
			using type = First;
		};

		template <typename False, typename Next, typename... Rest>
		struct disjuction_impl<false, False, Next, Rest...>
		{ // First trait is false, try the next trait.
			using type = typename disjuction_impl<Next::value, Next, Rest...>::type;
		};
	} // namespace internal

	template <typename... Traits>
	struct conjunction : true_type
	{
	}; // If Traits is empty, true_type.

	template <typename First, typename... Rest>
	struct conjunction<First, Rest...> : internal::conjunction_impl<First::value, First, Rest...>::type
	{
		// The first false trait in Traits, or the last trait if none are false.
	};

	template <typename... Traits>
	inline constexpr bool conjunction_v = conjunction<Traits...>::value;

	template <typename Trait>
	struct negation : bool_constant<!static_cast<bool>(Trait::value)>
	{
	}; // The negated result of Trait

	template <typename Trait>
	inline constexpr bool negation_v = negation<Trait>::value;

	template <typename... Traits>
	struct disjunction : false_type
	{
	}; // If Traits is empty, false_type.

	template <typename First, typename... Rest>
	struct disjunction<First, Rest...> : internal::disjuction_impl<First::value, First, Rest...>::type
	{
		// The first true trait in Traits, or the last trait if none are true.
	};

	template <typename... Traits>
	inline constexpr bool disjunction_v = disjunction<Traits...>::value;

	template <typename T, typename... Types>
	inline constexpr bool is_any_of_v = (is_same_v<T, Types> || ...); // True if and only if T is in Types.

	template <typename T>
	struct remove_reference
	{
		using type = T;
	};

	template <typename T>
	struct remove_reference<const T&>
	{
		using type = const T;
	};

	template <typename T>
	struct remove_reference<T&>
	{
		using type = T;
	};

	template <typename T>
	struct remove_reference<T&&>
	{
		using type = T;
	};

	template <typename T>
	using remove_reference_t = typename remove_reference<T>::type;

	template <typename T>
	struct remove_cv
	{
		using type = T;
	};

	template <typename T>
	struct remove_cv<const T>
	{
		using type = T;
	};

	template <typename T>
	struct remove_cv<volatile T>
	{
		using type = T;
	};

	template <typename T>
	struct remove_cv<const volatile T>
	{
		using type = T;
	};

	template <typename T>
	using remove_cv_t = typename remove_cv<T>::type;

	template <typename T>
	using remove_cvr = remove_cv<remove_reference_t<T>>;

	template <typename T>
	using remove_cvr_t = typename remove_cvr<T>::type;

	template <typename T>
	inline constexpr bool is_void_v = is_same_v<remove_cv_t<T>, void>;

	template <typename T>
	struct is_void : bool_constant<is_void_v<T>>
	{
	};

	template <typename... Types>
	using void_t = void;

	template <typename T>
	inline constexpr bool is_integral_v = is_any_of_v<
		remove_cv_t<T>, bool, char, signed char, unsigned char, wchar_t, char8_t, char16_t, char32_t, short,
		unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

	template <typename T>
	struct is_integral : bool_constant<is_integral_v<T>>
	{
	};

	template <typename T>
	inline constexpr bool is_floating_point_v = is_any_of_v<remove_cv_t<T>, float, double, long double>;

	template <typename T>
	struct is_floating_point : bool_constant<is_floating_point_v<T>>
	{
	};

	template <typename T>
	inline constexpr bool is_arithmetic_v = // determine whether T is an arithmetic type
		is_integral_v<T> || is_floating_point_v<T>;

	template <typename T>
	struct is_arithmetic : bool_constant<is_arithmetic_v<T>>
	{
	};

	template <typename>
	inline constexpr bool is_const_v = false;

	template <typename T>
	inline constexpr bool is_const_v<const T> = true;

	template <typename T>
	struct is_const : bool_constant<is_const_v<T>>
	{
	};

	template <typename>
	inline constexpr bool is_reference_v = false;

	template <typename T>
	inline constexpr bool is_reference_v<T&> = true;

	template <typename T>
	inline constexpr bool is_reference_v<T&&> = true;

	template <typename T>
	struct is_reference : bool_constant<is_reference_v<T>>
	{
	};

	template <typename T>
	inline constexpr bool is_function_v = !is_const_v<const T> && !is_reference_v<T>;

	template <typename T>
	struct is_function : bool_constant<is_function_v<T>>
	{
	};

	namespace internal
	{
		// pointer type cannot be formed
		template <typename T, typename = void>
		struct add_pointer_impl
		{
			using type = T;
		};

		// pointer type can be formed
		template <typename T>
		struct add_pointer_impl<T, void_t<remove_reference_t<T>*>>
		{
			using type = remove_reference<T>::type*;
		};

		template <typename T>
		struct add_const_impl
		{
			using type = const T;
		};

		template <typename T>
		struct add_const_impl<T&>
		{
			using type = const T&;
		};

		template <typename T>
		struct add_const_impl<T&&>
		{
			using type = T&&;
		};

		template <typename T>
		struct add_const_impl<T*>
		{
			using type = const T*;
		};
	} // namespace internal

	template <typename T>
	struct add_pointer
	{
		using type = typename internal::add_pointer_impl<T>::type;
	};

	template <typename T>
	using add_pointer_t = typename internal::add_pointer_impl<T>::type;

	template <typename T>
	struct add_const
	{
		using type = typename internal::add_const_impl<T>::type;
	};

	template <typename T>
	using add_const_t = typename add_const<T>::type;

	template <typename T, typename = void>
	struct add_ref
	{
		using lvalue = T;
		using rvalue = T;
	};

	template <typename T>
	struct add_ref<T, void_t<T&>>
	{
		using lvalue = T&;
		using rvalue = T&&;
	};

	template <typename T>
	struct add_ref<const T, void_t<const T&>>
	{
		using lvalue = const T&;
		using rvalue = const T&&;
	};

	template <typename T>
	struct add_lval_ref
	{
		using type = typename add_ref<T>::lvalue;
	};

	template <typename T>
	using add_lval_ref_t = typename add_lval_ref<T>::type;

	template <typename T>
	struct add_rval_ref
	{
		using type = typename add_ref<T>::rvalue;
	};

	template <typename T>
	using add_rval_ref_t = typename add_rval_ref<T>::type;

	template <typename>
	inline constexpr bool is_array_v = false;

	template <typename T, size_type I>
	inline constexpr bool is_array_v<T[I]> = true;

	template <typename T>
	inline constexpr bool is_array_v<T[]> = true;

	template <typename T>
	struct is_array : bool_constant<is_array_v<T>>
	{
	};

	// remove array extent
	template <typename T>
	struct remove_extent
	{
		using type = T;
	};

	template <typename T, size_type I>
	struct remove_extent<T[I]>
	{
		using type = T;
	};

	template <typename T>
	struct remove_extent<T[]>
	{
		using type = T;
	};

	template <typename T>
	using remove_extent_t = typename remove_extent<T>::type;

	namespace internal
	{
		template <typename T>
		struct decay_impl
		{
			using T1 = remove_reference_t<T>;
			using T2 = conditional_t<is_function_v<T1>, add_pointer<T1>, remove_cv<T1>>;
			using type = typename conditional_t<is_array_v<T1>, add_pointer<remove_extent_t<T1>>, T2>::type;
		};
	} // namespace internal

	template <typename T>
	struct decay
	{
		using type = internal::decay_impl<T>::type;
	};

	template <typename T>
	using decay_t = typename internal::decay_impl<T>::type;

	template <template <typename...> typename T, typename U, size_type I, typename... Args>
	struct make_sequence : make_sequence<T, U, I - 1, Args..., U>
	{
	};

	template <template <typename...> typename T, typename U, typename... Args>
	struct make_sequence<T, U, 0, Args...>
	{
		using type = T<Args...>;
	};

	template <template <typename...> typename T, typename U, size_type I, typename... Args>
	using make_sequence_t = typename make_sequence<T, U, I, Args...>::type;

	template <typename Type, template <typename...> typename Template>
	constexpr bool is_specialization_v = false; // true if and only if Type is a specialization of Template
	template <template <typename...> typename Template, typename... Types>
	constexpr bool is_specialization_v<Template<Types...>, Template> = true;

	template <typename Type, template <typename...> typename Template>
	struct is_specialization : bool_constant<is_specialization_v<Type, Template>>
	{
	};

	template <typename... Types>
	struct type_sequence
	{
		using tuple_type = std::tuple<Types...>; // TODO: Make our own tuple type.
		constexpr static size_type size = sizeof...(Types);
	};

	template <typename T>
	concept type_sequence_c = is_specialization_v<T, type_sequence>;

	template <typename...>
	struct concat_sequence;

	template <typename... A, typename... B, type_sequence_c C, type_sequence_c... Rest>
	struct concat_sequence<type_sequence<A...>, type_sequence<B...>, C, Rest...>
	{
		using type = typename concat_sequence<type_sequence<A..., B...>, C, Rest...>::type;
	};

	template <typename... A, typename... B>
	struct concat_sequence<type_sequence<A...>, type_sequence<B...>>
	{
		using type = type_sequence<A..., B...>;
	};

	template <type_sequence_c... Sequences>
	using concat_sequence_t = typename concat_sequence<Sequences...>::type;

	template <type_sequence_c Sequence, typename T>
	struct type_sequence_contains;

	template <typename... Types, typename T>
	struct type_sequence_contains<type_sequence<Types...>, T> : disjunction<is_same<T, Types>...>
	{
	};

	template <type_sequence_c Sequence, typename T>
	constexpr bool type_sequence_contains_v = type_sequence_contains<Sequence, T>::value;

	template <typename T, T... Vals>
	struct integer_sequence
	{ // sequence of integer parameters
		static_assert(is_integral_v<T>, "integer_sequence<T, I...> requires T to be an integral type.");

		using value_type = T;

		[[nodiscard]] static constexpr size_type size() noexcept { return sizeof...(Vals); }
	};

	template <typename T, T Size>
	using make_integer_sequence = __make_integer_seq<integer_sequence, T, Size>;

	template <size_t... Vals>
	using index_sequence = integer_sequence<size_t, Vals...>;

	template <size_t Size>
	using make_index_sequence = make_integer_sequence<size_t, Size>;

	template <typename... Types>
	using index_sequence_for = make_index_sequence<sizeof...(Types)>;

    
    template <typename T>
	T returns_exactly() noexcept; // not defined

    template <typename T>
	[[nodiscard]] T fake_copy_init(T) noexcept;
	// fake_copy_init<T>(E):
	// (1) has type T [decay_t<decltype((E))> if T is deduced],
	// (2) is well-formed if and only if E is implicitly convertible to T and T is destructible, and
	// (3) is non-throwing if and only if both conversion from decltype((E)) to T and destruction of T are non-throwing.

	template <typename T>
	add_rval_ref_t<T> declval() noexcept
	{
		static_assert(always_false_v<T>, "Calling declval is ill-formed.");
	}

	namespace internal
	{
		template <typename From, typename To, typename = void>
		struct invoke_convertible : false_type
		{
		};

		template <typename From, typename To>
		struct invoke_convertible<From, To, void_t<decltype(fake_copy_init<To>(returns_exactly<From>()))>> :
			true_type
		{
		};

		template <typename From, typename To>
		struct invoke_nothrow_convertible : bool_constant<noexcept(fake_copy_init<To>(returns_exactly<From>()))>
		{
		};

		template <typename ReturnType, bool Nothrow>
		struct invoke_traits
		{
			using type = ReturnType;
			using is_invocable = true_type;
			using is_nothrow_invocable = bool_constant<Nothrow>;

			template <typename RetType>
			using is_invocable_ret = bool_constant<disjunction_v<is_void<RetType>, invoke_convertible<type, RetType>>>;

			template <typename RetType>
			using is_nothrow_invocable_ret = bool_constant<conjunction_v<
				is_nothrow_invocable,
				disjunction<
					is_void<RetType>,
					conjunction<invoke_convertible<type, RetType>, invoke_nothrow_convertible<type, RetType>>>>>;
		};

		template <typename Void, typename Callable>
		struct invoke_traits_zero
		{
			// Selected when Callable isn't callable with zero Args.
			using is_invocable = false_type;
			using is_nothrow_invocable = false_type;

			template <typename RetType>
			using is_invocable_ret = false_type;

			template <typename RetType>
			using is_nothrow_invocable_ret = false_type;
		};

		template <typename Callable>
		using decltype_invoke_zero = decltype(declval<Callable>()());

		template <typename Callable>
		struct invoke_traits_zero<void_t<decltype_invoke_zero<Callable>>, Callable> :
			invoke_traits<decltype_invoke_zero<Callable>, noexcept(declval<Callable>()())>
		{
		};

		template <typename Void, typename... Types>
		struct invoke_traits_nonzero
		{
			// Selected when Callable isn't callable with nonzero Args.
			using is_invocable = false_type;
			using is_nothrow_invocable = false_type;

			template <typename RetType>
			using is_invocable_ret = false_type;

			template <typename RetType>
			using is_nothrow_invocable_ret = false_type;
		};

		template <typename Callable, typename Ty1, typename... Types2>
		using decltype_invoke_nonzero =
			decltype(::std::invoke(declval<Callable>(), declval<Ty1>(), declval<Types2>()...));

		template <typename Callable, typename Ty1, typename... Types2>
		struct invoke_traits_nonzero<
			void_t<decltype_invoke_nonzero<Callable, Ty1, Types2...>>, Callable, Ty1, Types2...> :
			invoke_traits<
				decltype_invoke_nonzero<Callable, Ty1, Types2...>,
				noexcept(::std::invoke(declval<Callable>(), declval<Ty1>(), declval<Types2>()...))>
		{
		};

		template <typename Callable, typename... Args>
		using select_invoke_traits = conditional_t<
			sizeof...(Args) == 0, invoke_traits_zero<void, Callable>, invoke_traits_nonzero<void, Callable, Args...>>;

		template <typename ReturnType, typename Callable, typename... Args>
		using is_invocable_ret_impl =
			typename select_invoke_traits<Callable, Args...>::template is_invocable_ret<ReturnType>;
	} // namespace internal

	template <typename Callable, typename... Args>
	using invoke_result_t = typename internal::select_invoke_traits<Callable, Args...>::type;

	template <typename Callable, typename... Args>
	struct invoke_result
	{
		using type = invoke_result_t<Callable, Args...>;
	};

	template <typename Callable, typename... Args>
	struct is_invocable_args : internal::select_invoke_traits<Callable, Args...>::is_invocable
	{
		// Determines whether Callable is callable with Args
	};

	template <typename Callable, typename... Args>
	inline constexpr bool is_invocable_args_v =
		internal::select_invoke_traits<Callable, Args...>::is_invocable_args::value;

	template <typename Callable, typename... Args>
	struct is_nothrow_invocable_args : internal::select_invoke_traits<Callable, Args...>::is_nothrow_invocable
	{
		// Determines whether Callable is nothrow-callable with Args
	};

	template <typename Callable, typename... Args>
	inline constexpr bool is_nothrow_invocable_args_v =
		internal::select_invoke_traits<Callable, Args...>::is_nothrow_invocable::value;

	template <typename ReturnType, typename Callable, typename... Args>
	struct is_invocable_args_ret : internal::is_invocable_ret_impl<ReturnType, Callable, Args...>
	{
		// Determines whether Callable is callable with Args and return type ReturnType
	};

	template <typename ReturnType, typename Callable, typename... Args>
	inline constexpr bool is_invocable_args_ret_v =
		internal::is_invocable_ret_impl<ReturnType, Callable, Args...>::value;

	template <typename ReturnType, typename Callable, typename... Args>
	struct is_nothrow_invocable_args_ret :
		internal::select_invoke_traits<Callable, Args...>::template is_nothrow_invocable_ret<ReturnType>
	{
		// Determines whether Callable is nothrow-callable with Args and return type ReturnType
	};

	template <typename ReturnType, typename Callable, typename... Args>
	inline constexpr bool is_nothrow_invocable_args_ret_v =
		internal::select_invoke_traits<Callable, Args...>::template is_nothrow_invocable_ret<ReturnType>::value;

	namespace internal
	{
		struct any_type
		{
			template <typename T>
			constexpr operator T&(); // implicit conversion to any type.
		};

		template <typename Func, size_type... paramCounts>
		constexpr bool test_invocable_impl([[maybe_unused]] integer_sequence<size_type, paramCounts...> int_seq)
		{
			return ((make_sequence_t<is_invocable_args, any_type, paramCounts, Func>::value) || ...);
		}
	} // namespace internal

	template <typename Func, size_type maxParams = 32>
	struct is_invocable
	{
		static constexpr bool value = internal::test_invocable_impl<Func>(make_index_sequence<maxParams>{});
	};

	template <typename Func, size_type maxParams = 32>
	constexpr bool is_invocable_v = is_invocable<Func, maxParams>::value;

	template <typename Func, size_type maxParams = 32>
	constexpr bool is_function_ptr_v = (is_empty_v<Func> || is_pointer_v<Func>) && is_invocable_v<Func, maxParams>;

	template <typename Func, size_type maxParams = 32>
	constexpr bool is_functor_v = requires { &Func::operator(); } && is_invocable_v<Func, maxParams>;

	template <typename Type>
	constexpr bool is_ratio_v = false; // test for ratio type

	template <int_max numerator, int_max denominator>
	constexpr bool is_ratio_v<std::ratio<numerator, denominator>> = true;

	template <typename Type>
	struct is_ratio : bool_constant<is_ratio_v<Type>>
	{
	};

    // std::is_base_of uses compiler magic behind the scenes.
	template <typename derived_type, typename base_type>
	using inherits_from = typename std::enable_if<std::is_base_of<base_type, derived_type>::value, int>::type;

	// std::is_base_of uses compiler magic behind the scenes.
	template <typename derived_type, typename base_type>
	using doesnt_inherit_from = typename std::enable_if<!std::is_base_of<base_type, derived_type>::value, int>::type;

	template <typename T>
	struct is_vector : public false_type
	{
	};

	template <typename T>
	struct is_vector<std::vector<T>> : public true_type
	{
	};

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
	private:
		template <typename T, typename... Args>
		static constexpr auto check(void*) -> decltype(void(T{declval<Args>()...}), true_type());

		template <typename...>
		static constexpr auto check(...) -> false_type;

		using type = decltype(check<T, Args...>(nullptr));

	public:
		static constexpr bool value = type::value;
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
		using type = bool_constant<is_empty_v<T>>;
	};

	template <typename T>
	struct is_always_equal<T, void_t<typename T::is_always_equal>>
	{
		using type = typename T::is_always_equal;
	};

	template <typename T>
	using is_always_equal_t = is_always_equal<T>::type;

	template <typename From, typename To>
	concept convertible_to = std::convertible_to<From, To>; // Compiler magic behind the scenes.

#if defined(RYTHE_MSVC)

	template <typename T>
	constexpr std::string_view type_name() noexcept
	{
		std::string_view funcName(__FUNCSIG__);
		auto first = funcName.find_first_of(' ', funcName.find("type_name<") + 11) + 1;
		return funcName.substr(first, funcName.find_last_of('>') - first);
	}

#elif defined(RYTHE_GCC)

	template <typename T>
	constexpr std::string_view type_name() noexcept
	{
		std::string_view funcName(__PRETTY_FUNCTION__);
		auto first = funcName.find_first_not_of(' ', funcName.find_first_of('=') + 1);
		return funcName.substr(first, funcName.find_last_of(';') - first);
	}

#elif defined(RYTHE_CLANG)

	template <typename T>
	constexpr std::string_view type_name() noexcept
	{
		std::string_view funcName(__PRETTY_FUNCTION__);
		auto first = funcName.find_first_not_of(' ', funcName.find_first_of('=') + 1);
		return funcName.substr(first, funcName.find_last_of(']') - first);
	}

#endif

	template <typename T>
	constexpr std::string_view type_name(T&&) noexcept
	{
		return type_name<std::remove_cvref_t<T>>();
	}

#if defined(RYTHE_MSVC) || defined(RYTHE_CLANG_MSVC)
	namespace detail
	{
		template <typename T>
		std::string getNameOfTypeImpl()
		{
			const std::string name = typeid(T).name();
			std::vector<std::string> vec = split_string_at<' '>(name);
			if (vec.size() < 2)
			{
				return name;
			}

			std::string result;
			for (auto i = 1; i < vec.size(); i++)
			{
				if (ends_with(vec[i], "struct"))
					result += vec[i].substr(0, vec[i].size() - 6);
				else if (ends_with(vec[i], "class"))
					result += vec[i].substr(0, vec[i].size() - 5);
				else
					result += vec[i];
			}
			return result;
		}
	} // namespace detail
#elif defined(RYTHE_GCC)

	#include <cxxabi.h>
	namespace detail
	{
		template <typename T>
		std::string getNameOfTypeImpl()
		{
			// Demangle the typename generated by GCC
			int status;
			char* realName = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
			std::string result = {realName};
			free(realName);
			return result;
		}
	} // namespace detail
#elif defined(RYTHE_CLANG_GCC)

	#include <cxxabi.h>
	namespace detail
	{
		template <typename T>
		std::string getNameOfTypeImpl()
		{
			// Demangle the typename generated by GCC
			int status;
			char* realName = __cxxabiv1::__cxa_demangle(typeid(T).name(), 0, 0, &status);
			std::string result = {realName};
			free(realName);
			return result;
		}
	} // namespace detail
#endif

	/**@brief Returns compile-time evaluable type name.
	 * @warning This version is not compiler agnostic! If you need it to be compiler agnostic use `nameOfType`.
	 */
	template <typename T>
	constexpr std::string_view localNameOfType() noexcept
	{
#if defined(RYTHE_CLANG) || defined(RYTHE_GCC)
		cstring p = __PRETTY_FUNCTION__;

		while (*p++ != '=');

		for (; *p == ' '; ++p);

		cstring p2 = p;
		int count = 1;

		for (;; ++p2)
		{
			switch (*p2)
			{
				case '[': ++count; break;
				case ']':
				case ';':
					--count;
					if (!count)
						return {p, size_type(p2 - p)};
			}
		}

		return {};
#elif defined(RYTHE_MSVC)
		cstring p = __FUNCSIG__;

		while (*p != 'T' || *(p + 1) != 'y' || *(p + 2) != 'p' || *(p + 3) != 'e' || *(p + 4) != '<') p++;

		while (*p++ != ' ');

		cstring p2 = p;
		int count = 1;
		size_type size = 0;

		for (; size == 0; ++p2)
		{
			switch (*p2)
			{
				case '<': ++count; break;
				case '>':
					--count;
					if (!count)
					{
						size = (p2 - p);
					}
			}
		}

		return {p, size};

#else
	#error unknown compiler
#endif
	}


	/**@brief Returns type name with namespaces other than that it's undecorated.
	 * @tparam T type of which you want the name.
	 */
	template <typename T>
	cstring nameOfType()
	{
		static std::string name = detail::getNameOfTypeImpl<T>();
		return name.c_str();
	}

	/**@brief Returns type name with namespaces other than that it's undecorated.
	 * @tparam T type of which you want the name.
	 * @param expr Variable of which you wish to auto deduct type.
	 */
	template <typename T>
	cstring nameOfType(T&& expr)
	{
		return nameOfType<std::decay_t<T>>();
	}

	/**@brief Returns hash of a certain string
	 * @tparam N Length of the string literal
	 * @param name Name you wish to hash
	 * @note Since this version takes a const char[] it can only really be used with data coming from a string literal.
	 *       Because it takes in a const char[] this function is able to be constexpr and thus have minimal overhead.
	 */
	template <size_type N>
	constexpr id_type nameHash(const char (&name)[N]) noexcept
	{
		id_type hash = 0xcbf29ce484222325;
		constexpr uint64 prime = 0x00000100000001b3;

		size_type size = N;
		if (name[size - 1] == '\0')
			size--;

		for (size_type i = 0; i < size; i++)
		{
			hash = hash ^ static_cast<const byte>(name[i]);
			hash *= prime;
		}

		return hash;
	}

	/**@brief Returns hash of a certain string
	 * @tparam N Length of the string literal
	 * @param name Name you wish to hash
	 * @note Since this version takes a const char[] it can only really be used with data coming from a string literal.
	 *       Because it takes in a const char[] this function is able to be constexpr and thus have minimal overhead.
	 */
	template <size_type N>
	constexpr id_type nameHash(const std::array<char, N>& name) noexcept
	{
		id_type hash = 0xcbf29ce484222325;
		constexpr uint64 prime = 0x00000100000001b3;

		size_type size = N;
		if (name[size - 1] == '\0')
			size--;

		for (size_type i = 0; i < size; i++)
		{
			hash = hash ^ static_cast<const byte>(name[i]);
			hash *= prime;
		}

		return hash;
	}

	/**@brief Returns hash of a certain string
	 * @param name Name you wish to hash
	 */
	constexpr id_type nameHash(cstring name) noexcept
	{
		id_type hash = 0xcbf29ce484222325;
		constexpr uint64 prime = 0x00000100000001b3;

		for (size_type i = 0; i < rsl::constexpr_strlen(name); i++)
		{
			hash = hash ^ static_cast<const byte>(name[i]);
			hash *= prime;
		}

		return hash;
	}

	/**@brief Returns hash of a certain string
	 * @param name Name you wish to hash
	 */
	constexpr id_type nameHash(const std::string_view& name) noexcept
	{
		id_type hash = 0xcbf29ce484222325;
		constexpr uint64 prime = 0x00000100000001b3;

		size_type size = name.size();

		if (name[size - 1] == '\0')
			size--;

		for (size_type i = 0; i < size; i++)
		{
			hash = hash ^ static_cast<const byte>(name[i]);
			hash *= prime;
		}

		return hash;
	}
	//
	//    /**@brief Returns hash of a certain string
	// * @param name Name you wish to hash
	// */
	//    id_type nameHash(const std::string& name)
	//    {
	// #if defined(RYTHE_MSVC) || defined(RYTHE_CLANG_MSVC)
	//        static std::hash<std::string> hasher{};
	//        if (!name.empty() && name[name.size() - 1] == '\0')
	//            return nameHash(std::string_view(name));
	//
	//        return hasher(name);
	// #else
	//        // std::hash returns a different hash on GCC and Clang on Linux for certain CPU architectures.
	//        // These certain different hashes are faster to compute but can create issues if they aren't the same.
	//        return nameHash(std::string_view(name));
	// #endif
	//    }


	/**@brief Returns compile-time evaluable hash of the type name.
	 * @warning This version is not compiler agnostic! If you need it to be compiler agnostic use `typeHash`.
	 */
	template <typename T>
	constexpr id_type localTypeHash() noexcept
	{
		return nameHash(localNameOfType<T>());
	}

} // namespace rsl
