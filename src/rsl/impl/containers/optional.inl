#pragma once
#include "optional.hpp"

namespace rsl
{
	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional() noexcept
		: m_factory(),
		  m_hasValue(false)
	{
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(nullptr_type) noexcept
		: m_factory(),
		  m_hasValue(false)
	{
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(const optional& other)
		noexcept(is_nothrow_copy_constructible_v<factory_storage_type> && is_nothrow_copy_constructible_v<value_type>)
		: m_factory(other.m_factory),
		  m_hasValue(false)
	{
		if (other.m_hasValue)
		{
			emplace(other.m_value);
		}
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(optional&& other)
		noexcept(is_nothrow_move_constructible_v<factory_storage_type> && is_nothrow_move_constructible_v<value_type>)
		: m_factory(move(other.m_factory))
	{
		if (other.m_hasValue)
		{
			emplace(move(other.m_value));
		}
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(const factory_storage_type& factoryStorage)
		noexcept(is_nothrow_copy_constructible_v<factory_storage_type>)
		: m_factory(factoryStorage),
		  m_hasValue(false)
	{
	}

	template <typename T, factory_type Factory>
	template <typename... Args>
	inline constexpr optional<T, Factory>::optional(in_place_signal_type, Args&&... args)
		noexcept(is_nothrow_constructible_v<value_type, Args...>)
		: m_factory()
	{
		emplace(forward<Args>(args)...);
	}

	template <typename T, factory_type Factory>
	template <typename... Args>
	inline constexpr optional<T, Factory>::optional(
		const factory_storage_type& factoryStorage, in_place_signal_type, Args&&... args
	) noexcept(is_nothrow_constructible_v<value_type, Args...>)
		: m_factory(factoryStorage)
	{
		emplace(forward<Args>(args)...);
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(const value_type& value)
		noexcept(is_nothrow_copy_constructible_v<value_type>)
		: m_factory()
	{
		emplace(value);
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(const factory_storage_type& factoryStorage, const value_type& value)
		noexcept(is_nothrow_copy_constructible_v<value_type>)
		: m_factory(factoryStorage)
	{
		emplace(value);
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(value_type&& value)
		noexcept(is_nothrow_move_constructible_v<value_type>)
		: m_factory()
	{
		emplace(move(value));
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::optional(const factory_storage_type& factoryStorage, value_type&& value)
		noexcept(is_nothrow_move_constructible_v<value_type>)
		: m_factory(factoryStorage)
	{
		emplace(move(value));
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::~optional() noexcept
	{
		reset();
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>& optional<T, Factory>::operator=(nullptr_type) noexcept
	{
		reset();
		return *this;
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>& optional<T, Factory>::operator=(const optional& other)
		noexcept(is_nothrow_copy_assignable_v<factory_storage_type> && is_nothrow_copy_constructible_v<value_type>)
	{
		reset();

		m_factory = other.m_factory;

		if (other.m_hasValue)
		{
			emplace(other.m_value);
		}

		return *this;
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>& optional<T, Factory>::operator=(optional&& other)
		noexcept(is_nothrow_move_assignable_v<factory_storage_type> && is_nothrow_move_constructible_v<value_type>)
	{
		reset();

		m_factory = move(other.m_factory);

		if (other.m_hasValue)
		{
			emplace(move(other.m_value));
		}

		return *this;
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>& optional<T, Factory>::operator=(const value_type& value)
		noexcept(is_nothrow_copy_constructible_v<value_type>)
	{
		reset();
		emplace(value);
		return *this;
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>& optional<T, Factory>::operator=(value_type&& value)
		noexcept(is_nothrow_move_constructible_v<value_type>)
	{
		reset();
		emplace(move(value));
		return *this;
	}

	template <typename T, factory_type Factory>
	inline constexpr T* optional<T, Factory>::operator->() noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return &m_value;
	}

	template <typename T, factory_type Factory>
	inline constexpr const T* optional<T, Factory>::operator->() const noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return &m_value;
	}

	template <typename T, factory_type Factory>
	inline constexpr T& optional<T, Factory>::operator*() & noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return m_value;
	}

	template <typename T, factory_type Factory>
	inline constexpr const T& optional<T, Factory>::operator*() const& noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return m_value;
	}

	template <typename T, factory_type Factory>
	inline constexpr T&& optional<T, Factory>::operator*() && noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return move(m_value);
	}

	template <typename T, factory_type Factory>
	inline constexpr const T&& optional<T, Factory>::operator*() const&& noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return move(m_value);
	}

	template <typename T, factory_type Factory>
	inline constexpr T& optional<T, Factory>::value() & noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return m_value;
	}

	template <typename T, factory_type Factory>
	inline constexpr const T& optional<T, Factory>::value() const& noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return m_value;
	}

	template <typename T, factory_type Factory>
	inline constexpr T&& optional<T, Factory>::value() && noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return move(m_value);
	}

	template <typename T, factory_type Factory>
	inline constexpr const T&& optional<T, Factory>::value() const&& noexcept
	{
		rsl_assert_invalid_access(m_hasValue);
		return move(m_value);
	}

	template <typename T, factory_type Factory>
	inline constexpr bool optional<T, Factory>::holds_value() noexcept
	{
		return m_hasValue;
	}

	template <typename T, factory_type Factory>
	inline constexpr optional<T, Factory>::operator bool() noexcept
	{
		return holds_value();
	}

	template <typename T, factory_type Factory>
	template <typename... Args>
	inline constexpr T& optional<T, Factory>::emplace(Args&&... args)
		noexcept(is_nothrow_constructible_v<value_type, Args...>)
	{
		reset();
		m_hasValue = true;
		m_factory->construct(&m_value, 1, forward<Args>(args)...);
		return m_value;
	}

	template <typename T, factory_type Factory>
	inline constexpr void optional<T, Factory>::reset() noexcept
	{
		if (m_hasValue)
		{
			m_factory->destroy(&m_value, 1);
			m_hasValue = false;
		}
	}
} // namespace rsl
