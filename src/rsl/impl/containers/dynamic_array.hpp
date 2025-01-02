#pragma once

#include "../util/assert.hpp"
#include "../util/primitives.hpp"
#include "contiguous_container_base.hpp"

namespace rsl
{
	template <typename T, allocator_type Alloc = default_allocator, typed_factory_type Factory = default_factory<T>>
	class dynamic_array final : public contiguous_container_base<T, Alloc, Factory, T*, const T*>
	{
	public:
		using container_base = contiguous_container_base<T, Alloc, Factory, T*, const T*>;
		using mem_rsc = container_base::mem_rsc;
		using value_type = T;
		using iterator_type = container_base::iterator_type;
		using const_iterator_type = container_base::const_iterator_type;
		using reverse_iterator_type = container_base::reverse_iterator_type;
		using const_reverse_iterator_type = container_base::const_reverse_iterator_type;
		using view_type = container_base::view_type;
		using const_view_type = container_base::const_view_type;
		using allocator_storage_type = allocator_storage<Alloc>;
		using allocator_t = Alloc;
		using factory_storage_type = factory_storage<Factory>;
		using factory_t = Factory;

		[[rythe_always_inline]] constexpr dynamic_array()
			noexcept(is_nothrow_constructible_v<container_base>) = default;

		[[rythe_always_inline]] constexpr dynamic_array(const dynamic_array& src)
			noexcept(container_base::copy_construct_noexcept);
		[[rythe_always_inline]] constexpr dynamic_array(dynamic_array&& src) noexcept;

		[[rythe_always_inline]] explicit constexpr dynamic_array(const allocator_storage_type& allocStorage)
			noexcept(is_nothrow_constructible_v<container_base, const allocator_storage_type&>);
		[[rythe_always_inline]] explicit constexpr dynamic_array(const factory_storage_type& factoryStorage)
			noexcept(is_nothrow_constructible_v<container_base, const factory_storage_type&>);
		[[rythe_always_inline]] constexpr dynamic_array(
			const allocator_storage_type& allocStorage, const factory_storage_type& factoryStorage
		)
			noexcept(is_nothrow_constructible_v<
					 container_base, const allocator_storage_type&, const factory_storage_type&>);

		template <size_type N>
		[[rythe_always_inline]] constexpr dynamic_array(const value_type (&arr)[N])
			noexcept(container_base::copy_construct_noexcept);
		template <size_type N>
		[[rythe_always_inline]] constexpr dynamic_array(value_type (&&arr)[N])
			noexcept(container_base::move_construct_noexcept);
		[[rythe_always_inline]] constexpr dynamic_array(view_type src)
			noexcept(container_base::copy_construct_noexcept);
		[[rythe_always_inline]] constexpr dynamic_array(size_type capacity) noexcept;
		template <typename... Args>
		[[rythe_always_inline]] constexpr dynamic_array(size_type count, in_place_signal_type, Args&&... args)
			noexcept(container_base::template construct_noexcept<Args...>);

		[[rythe_always_inline]] constexpr dynamic_array& operator=(const dynamic_array& src)
			noexcept(container_base::copy_assign_noexcept && container_base::copy_construct_noexcept);
		[[rythe_always_inline]] constexpr dynamic_array& operator=(dynamic_array&& src) noexcept;
		template <size_type N>
		[[rythe_always_inline]] constexpr dynamic_array& operator=(const value_type (&arr)[N])
			noexcept(container_base::copy_assign_noexcept && container_base::copy_construct_noexcept);
		template <size_type N>
		[[rythe_always_inline]] constexpr dynamic_array& operator=(value_type (&&arr)[N])
			noexcept(container_base::move_assign_noexcept && container_base::move_construct_noexcept);
		[[rythe_always_inline]] constexpr dynamic_array& operator=(view_type src)
			noexcept(container_base::copy_assign_noexcept && container_base::copy_construct_noexcept);

		template <typename... Args>
		[[rythe_always_inline]] constexpr void resize(size_type newSize, Args&&... args)
			noexcept(container_base::template construct_noexcept<Args...> && container_base::move_construct_noexcept);
		[[rythe_always_inline]] constexpr void reserve(size_type newCapacity)
			noexcept(container_base::move_construct_noexcept);

		[[rythe_always_inline]] constexpr void push_back(const value_type& value)
			noexcept(container_base::copy_construct_noexcept);
		[[rythe_always_inline]] constexpr void push_back(value_type&& value)
			noexcept(container_base::move_construct_noexcept);

		template <typename... Args>
		[[rythe_always_inline]] constexpr value_type& emplace_back(Args&&... args)
			noexcept(container_base::template construct_noexcept<Args...> && container_base::move_construct_noexcept);

		[[rythe_always_inline]] constexpr void pop_back(size_type count = 1ull) noexcept;

		[[rythe_always_inline]] constexpr void clear() noexcept;

		[[rythe_always_inline]] constexpr void assign(size_type count, const value_type& value);
		template <input_iterator InputIt>
		[[rythe_always_inline]] constexpr void assign(InputIt first, InputIt last);

		[[rythe_always_inline]] constexpr iterator_type iterator_at(size_type i) noexcept;
		[[rythe_always_inline]] constexpr const_iterator_type iterator_at(size_type i) const noexcept;

		[[rythe_always_inline]] constexpr size_type insert(size_type pos, const value_type& value)
			noexcept(container_base::move_construct_noexcept && container_base::copy_construct_noexcept);
		[[rythe_always_inline]] constexpr size_type insert(size_type pos, value_type&& value)
			noexcept(container_base::move_construct_noexcept);
		[[rythe_always_inline]] constexpr size_type insert(size_type pos, size_type count, const value_type& value)
			noexcept(container_base::move_construct_noexcept && container_base::copy_construct_noexcept);
		template <input_iterator InputIt>
		[[rythe_always_inline]] constexpr size_type insert(
			size_type pos, InputIt first, InputIt last
		) noexcept(container_base::move_construct_noexcept && container_base::template construct_noexcept<iter_value_t<InputIt>>);

		[[rythe_always_inline]] constexpr size_type erase(size_type pos)
			noexcept(container_base::move_construct_noexcept);
		[[rythe_always_inline]] constexpr size_type erase(size_type first, size_type last)
			noexcept(container_base::move_construct_noexcept);
		template <typename Comp>
		[[rythe_always_inline]] constexpr size_type erase(Comp&& comparable)
			noexcept(container_base::move_construct_noexcept);
		template <typename Func>
		[[rythe_always_inline]] constexpr size_type erase(Func&& comparer)
			noexcept(container_base::move_construct_noexcept)
			requires invocable<Func, bool(const_iterator_type)>;

	private:
		[[rythe_always_inline]] constexpr void maybe_grow() noexcept(container_base::move_construct_noexcept);

		[[rythe_always_inline]] constexpr void
		copy_assign_impl(const value_type* src, size_type srcSize, mem_rsc::typed_alloc_type* alloc = nullptr)
			noexcept(container_base::copy_assign_noexcept && container_base::copy_construct_noexcept);

		[[rythe_always_inline]] constexpr void move_data_assign_impl(const value_type* src, size_type srcSize)
			noexcept(container_base::move_assign_noexcept && container_base::move_construct_noexcept);

		[[rythe_always_inline]] constexpr void split_reserve(size_type pos, size_type count, size_type newSize)
			noexcept(container_base::move_construct_noexcept);
	};
} // namespace rsl

#include "dynamic_array.inl"
