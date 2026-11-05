#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <exception>
#include "../../../tangible_exception_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Typed fail-closed fault for decode projection/materialization seams.
	/// This is narrower than a generic InvalidOperationException so decode fallback
	/// does not silently absorb unrelated internal bugs.
	/// </summary>
	class DecodeProjectionFaultException final : public InvalidOperationException, public std::enable_shared_from_this<DecodeProjectionFaultException>
	{
	private:
		const std::exception_ptr InnerException;

	public:
		DecodeProjectionFaultException(const std::wstring &message);

		DecodeProjectionFaultException(const std::wstring &message, std::runtime_error innerException);

		std::exception_ptr getInnerException() const noexcept { return InnerException; }

	protected:
		std::shared_ptr<DecodeProjectionFaultException> shared_from_this()
		{
			return std::enable_shared_from_this<DecodeProjectionFaultException>::shared_from_this();
		}
	};
}
