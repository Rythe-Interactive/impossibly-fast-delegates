#pragma once
#include "../memory/memory_resource_base.hpp"
#include "iterators.hpp"

namespace rsl
{
	template <
		typename T, allocator_type Alloc = default_allocator, factory_type Factory = default_factory<T>,
		contiguous_iterator Iter = T*, contiguous_iterator ConstIter = const T*>
	class contiguous_container_base : public internal::select_memory_resource<T, Alloc, Factory>::type
	{
	public:
		using mem_rsc = internal::select_memory_resource<T, Alloc, Factory>::type;
		using value_type = T;
		using iterator_type = Iter;
		using const_iterator_type = ConstIter;
		using reverse_iterator_type = std::reverse_iterator<iterator_type>;
		using const_reverse_iterator_type = std::reverse_iterator<const_iterator_type>;
		using view_type = std::span<value_type>;
		using const_view_type = std::span<const value_type>;
		using allocator_storage_type = mem_rsc::allocator_storage_type;
		using allocator_t = mem_rsc::allocator_t;
		using factory_storage_type = mem_rsc::factory_storage_type;
		using factory_t = mem_rsc::factory_t;

		[[rythe_always_inline]] constexpr contiguous_container_base()
			noexcept(is_nothrow_constructible_v<mem_rsc>) =
			default;
		virtual ~contiguous_container_base() = default;

		[[rythe_always_inline]] explicit contiguous_container_base(const allocator_storage_type& allocStorage
		) noexcept(is_nothrow_constructible_v<mem_rsc, const allocator_storage_type&>);
		[[rythe_always_inline]] explicit contiguous_container_base(const factory_storage_type& factoryStorage)
			noexcept(is_nothrow_constructible_v<mem_rsc, const factory_storage_type&>);
		[[rythe_always_inline]] contiguous_container_base(
			const allocator_storage_type& allocStorage, const factory_storage_type& factoryStorage
		) noexcept(is_nothrow_constructible_v<mem_rsc, const allocator_storage_type&, const factory_storage_type&>);


		[[nodiscard]] [[rythe_always_inline]] constexpr size_type size() const noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr bool empty() const noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr size_type capacity() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr value_type& at(size_type i) noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const value_type& at(size_type i) const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr value_type& operator[](size_type i) noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const value_type& operator[](size_type i) const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr value_type* data() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const value_type* data() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr view_type view() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_view_type view() const noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr operator view_type() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr operator const_view_type() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr value_type& front() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const value_type& front() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr value_type& back() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const value_type& back() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr iterator_type begin() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_iterator_type begin() const noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_iterator_type cbegin() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr iterator_type end() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_iterator_type end() const noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_iterator_type cend() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr reverse_iterator_type rbegin() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_reverse_iterator_type rbegin() const noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_reverse_iterator_type crbegin() const noexcept;

		[[nodiscard]] [[rythe_always_inline]] constexpr reverse_iterator_type rend() noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_reverse_iterator_type rend() const noexcept;
		[[nodiscard]] [[rythe_always_inline]] constexpr const_reverse_iterator_type crend() const noexcept;

	protected:
		constexpr static bool copy_assign_noexcept = is_nothrow_copy_assignable_v<value_type>;
		constexpr static bool copy_construct_noexcept = is_nothrow_copy_constructible_v<value_type>;
		constexpr static bool move_assign_noexcept = is_nothrow_move_assignable_v<value_type>;
		constexpr static bool move_construct_noexcept = is_nothrow_move_constructible_v<value_type>;

		template <typename... Args>
		constexpr static bool construct_noexcept = is_nothrow_constructible_v<value_type, Args...>;

		[[rythe_always_inline]] constexpr void
		copy_assign_from_unsafe_impl(size_type offset, size_type end, const value_type* src)
			noexcept(copy_assign_noexcept);

		[[rythe_always_inline]] constexpr void
		copy_construct_from_unsafe_impl(size_type offset, size_type end, const value_type* src)
			noexcept(copy_construct_noexcept);

		[[rythe_always_inline]] constexpr void
		move_assign_from_unsafe_impl(size_type offset, size_type end, const value_type* src)
			noexcept(move_assign_noexcept);

		[[rythe_always_inline]] constexpr void
		move_construct_from_unsafe_impl(size_type offset, size_type end, const value_type* src)
			noexcept(move_construct_noexcept);

		template <typename... Args>
		[[rythe_always_inline]] constexpr void emplace_unsafe_impl(size_type offset, size_type end, Args&&... args)
			noexcept(construct_noexcept<Args...>);

		[[rythe_always_inline]] constexpr void reset_unsafe_impl(size_type offset = 0, size_type end = npos) noexcept;

		size_type m_size;
		size_type m_capacity;
	};
} // namespace rsl

#include "contiguous_container_base.inl"
