#pragma once

namespace Tunkio
{
	template<typename T, T NullValue, T InvalidValue, auto CloseFn>
	class AutoHandle
	{
	public:
		AutoHandle() = default;

		explicit AutoHandle(T handle) :
			m_handle(handle)
		{
		}

		virtual ~AutoHandle()
		{
			Reset();
		}

		const T Value() const
		{
			return m_handle;
		}

		bool IsValid() const
		{
			if (m_handle != InvalidValue)
			{
				return m_handle > NullValue;
			}

			return false;
		}

		void Reset(T handle = NullValue)
		{
			if (m_handle == handle)
			{
				return;
			}

			if (IsValid())
			{
				CloseFn(m_handle);
			}

			m_handle = handle;
		}

		AutoHandle& operator = (AutoHandle&& other)
		{
			std::swap(m_handle, other.m_handle);
			return *this;
		}

		AutoHandle& operator = (AutoHandle& other)
		{
			std::swap(m_handle, other.m_handle);
			return *this;
		}

		AutoHandle& operator = (const T& other)
		{
			std::swap(m_handle, other);
			return *this;
		}

	protected:
		T m_handle = NullValue;
	};
}