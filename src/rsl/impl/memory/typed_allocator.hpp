#pragma once

#include "allocator_storage.hpp"
#include "factory_storage.hpp"

namespace rsl
{
	template <typename T, allocator_type Alloc = default_allocator, factory_type Factory = default_factory<T>>
	class typed_allocator final
	{
	public:
		using value_type = T;
		using universal_type = Alloc;
		using allocator_storage_type = allocator_storage<Alloc>;
		using allocator_t = Alloc;
		using factory_storage_type = factory_storage<Factory>;
		using factory_t = Factory;

		template <typename Other>
		using retarget = typed_allocator<Other, Alloc, typename Factory::template retarget<Other>>;

		typed_allocator() = default;
		typed_allocator(const allocator_storage_type& allocStorage, const factory_storage_type& factoryStorage = {});

		template <not_same_as<T> Other>
		typed_allocator(const retarget<Other>& other);

		[[rythe_always_inline]] constexpr void set_allocator(const allocator_storage_type& allocStorage)
			noexcept(is_nothrow_copy_assignable_v<allocator_storage_type>);

		[[rythe_always_inline]] constexpr allocator_t& get_allocator() noexcept;
		[[rythe_always_inline]] constexpr const allocator_t& get_allocator() const noexcept;

		[[rythe_always_inline]] constexpr void set_factory(const factory_storage_type& factoryStorage)
			noexcept(is_nothrow_copy_assignable_v<factory_storage_type>);

		[[rythe_always_inline]] constexpr factory_t& get_factory() noexcept;
		[[rythe_always_inline]] constexpr const factory_t& get_factory() const noexcept;

		template <typename... Args>
		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr T*
		allocate(size_type count = 1, Args&&... args)
			noexcept(factory_traits<Factory>::template noexcept_constructable<Args...>);

		template <typename... Args>
		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr T*
		allocate_aligned(size_type count, size_type alignment, Args&&... args)
			noexcept(factory_traits<Factory>::template noexcept_constructable<Args...>);

		template <typename... Args>
		[[rythe_always_inline]] constexpr T* construct(T* ptr, size_type count = 1, Args&&... args)
			noexcept(factory_traits<Factory>::template noexcept_constructable<Args...>);

		template <typename... Args>
		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr T*
		reallocate(T* ptr, size_type oldCount, size_type newCount, Args&&... args) noexcept(
			factory_traits<Factory>::template noexcept_constructable<Args...> &&
			factory_traits<Factory>::noexcept_moveable
		);

		template <typename... Args>
		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr T*
		reallocate_aligned(T* ptr, size_type oldCount, size_type newCount, size_type alignment, Args&&... args)
			noexcept(
				factory_traits<Factory>::template noexcept_constructable<Args...> &&
				factory_traits<Factory>::noexcept_moveable
			);

		[[rythe_always_inline]] constexpr void deallocate(T* ptr, size_type count = 1) noexcept;
		[[rythe_always_inline]] constexpr void
		deallocate_aligned(T* ptr, size_type count, size_type alignment) noexcept;

		[[rythe_always_inline]] constexpr allocator_storage_type& get_allocator_storage() noexcept;
		[[rythe_always_inline]] constexpr const allocator_storage_type& get_allocator_storage() const noexcept;

		[[rythe_always_inline]] constexpr factory_storage_type& get_factory_storage() noexcept;
		[[rythe_always_inline]] constexpr const factory_storage_type& get_factory_storage() const noexcept;

	private:
		allocator_storage_type m_alloc;
		factory_storage_type m_factory;
	};

	template <allocator_type Alloc = default_allocator, untyped_factory_type Factory = type_erased_factory>
	class type_erased_allocator
	{
	public:
		using universal_type = Alloc;
		using allocator_storage_type = allocator_storage<Alloc>;
		using allocator_t = Alloc;
		using factory_storage_type = factory_storage<Factory>;
		using factory_t = Factory;

		template <typename Other>
		using retarget = type_erased_allocator<Alloc>;

		type_erased_allocator() = default;
		type_erased_allocator(const allocator_storage_type& allocStorage, const factory_storage_type& factoryStorage);

		[[rythe_always_inline]] constexpr void set_allocator(const allocator_storage_type& allocStorage)
			noexcept(is_nothrow_copy_assignable_v<allocator_storage_type>);

		[[rythe_always_inline]] constexpr allocator_t& get_allocator() noexcept;
		[[rythe_always_inline]] constexpr const allocator_t& get_allocator() const noexcept;

		[[rythe_always_inline]] constexpr void set_factory(const factory_storage_type& factoryStorage)
			noexcept(is_nothrow_copy_assignable_v<factory_storage_type>);

		[[rythe_always_inline]] constexpr factory_t& get_factory() noexcept;
		[[rythe_always_inline]] constexpr const factory_t& get_factory() const noexcept;

		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr void* allocate(size_type count = 1)
			noexcept(factory_traits<Factory>::template noexcept_constructable<>);

		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr void*
		allocate_aligned(size_type count, size_type alignment)
			noexcept(factory_traits<Factory>::template noexcept_constructable<>);

		[[rythe_always_inline]] constexpr void* construct(void* ptr, size_type count = 1)
			noexcept(factory_traits<Factory>::template noexcept_constructable<>);

		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr void*
		reallocate(void* ptr, size_type oldCount, size_type newCount) noexcept(
			factory_traits<Factory>::template noexcept_constructable<> && factory_traits<Factory>::noexcept_moveable
		);

		[[nodiscard]] [[rythe_allocating]] [[rythe_always_inline]] constexpr void*
		reallocate_aligned(void* ptr, size_type oldCount, size_type newCount, size_type alignment) noexcept(
			factory_traits<Factory>::template noexcept_constructable<> && factory_traits<Factory>::noexcept_moveable
		);

		[[rythe_always_inline]] constexpr void deallocate(void* ptr, size_type count = 1) noexcept;
		[[rythe_always_inline]] constexpr void
		deallocate_aligned(void* ptr, size_type count, size_type alignment) noexcept;

		[[rythe_always_inline]] constexpr allocator_storage_type& get_allocator_storage() noexcept;
		[[rythe_always_inline]] constexpr const allocator_storage_type& get_allocator_storage() const noexcept;

		[[rythe_always_inline]] constexpr factory_storage_type& get_factory_storage() noexcept;
		[[rythe_always_inline]] constexpr const factory_storage_type& get_factory_storage() const noexcept;

	private:
		allocator_storage_type m_alloc;
		factory_storage_type m_factory;
	};
} // namespace rsl

#include "typed_allocator.inl"
