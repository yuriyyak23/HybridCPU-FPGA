#pragma once

#include <string>
#include <format>
#include <memory>
#include "../../tangible_exception_helper.h"
#include "../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Thrown when a runtime surface that previously fell back to Processor.MainMemory
	/// is invoked without an explicit bound memory instance.
	/// </summary>
	class MainMemoryBindingUnavailableException final : public InvalidOperationException, public std::enable_shared_from_this<MainMemoryBindingUnavailableException>
	{
	private:
		const std::wstring BindingSurface;
		const std::wstring Operation;

	public:
		MainMemoryBindingUnavailableException(const std::wstring &bindingSurface, const std::wstring &operation);

		std::wstring getBindingSurface() const;

		std::wstring getOperation() const;

	private:
		static std::wstring Normalize(const std::wstring &value);

	protected:
		std::shared_ptr<MainMemoryBindingUnavailableException> shared_from_this()
		{
			return std::enable_shared_from_this<MainMemoryBindingUnavailableException>::shared_from_this();
		}
	};
}
