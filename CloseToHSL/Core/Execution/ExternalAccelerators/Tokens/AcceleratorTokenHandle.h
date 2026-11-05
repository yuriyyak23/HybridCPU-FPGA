#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <format>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{
	struct AcceleratorTokenHandle final {
		const unsigned long long Value;
		const bool IsValid;
		explicit AcceleratorTokenHandle(unsigned long long value = 0) : Value(value), IsValid(value != 0) {}
		static std::shared_ptr<AcceleratorTokenHandle> FromOpaqueValue(unsigned long long value) {
			if (!value) throw std::out_of_range("L7-SDC accelerator token handle value zero is invalid.");
			return std::make_shared<AcceleratorTokenHandle>(value);
		}
		static const std::shared_ptr<AcceleratorTokenHandle> Invalid;
		std::wstring ToString() const { return IsValid ? std::format(L"0x{:016X}", Value) : L"invalid"; }
		bool operator==(const AcceleratorTokenHandle& other) const noexcept { return Value == other.Value; }
	};
	inline const std::shared_ptr<AcceleratorTokenHandle> AcceleratorTokenHandle::Invalid = std::make_shared<AcceleratorTokenHandle>(0);


// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorTokenHandle(ulong Value)
//	{
//		public static AcceleratorTokenHandle Invalid
//		{
//			get
//			{
//				return new(0);
//			}
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return Value != 0;
//			}
//		}
//
//		public static AcceleratorTokenHandle FromOpaqueValue(ulong value)
//		{
//			if (value == 0)
//			{
//				throw new ArgumentOutOfRangeException(nameof(value), "L7-SDC accelerator token handle value zero is invalid.");
//			}
//
//			return new AcceleratorTokenHandle(value);
//		}
//
//		public override string ToString()
//		{
//			return IsValid ? string.Format("0x{0:X16}", Value) : "invalid";
//		}
//	}

}
