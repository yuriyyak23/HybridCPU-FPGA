#pragma once
#include "MemoryBankGeometryGeneration.h"
#include <memory>
#include <stdexcept>
#include <utility>
#include <climits>

namespace YAKSys_Hybrid_CPU::Memory
{

	/// <summary>
	/// Immutable checked representation of one physical memory-bank geometry
	/// tuple. Representation does not prove subsystem ownership, allocation
	/// feasibility, publication, request binding, admission, execution,
	/// completion, cancellation, replay, retirement, store visibility or
	/// telemetry authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct PhysicalMemoryBankGeometry
//	{
//		public const int MinBankCount = 1;
//		public const int MaxBankCount = int.MaxValue;
//		public const int MinBankWidthBytes = 1;
//		public const int MaxBankWidthBytes = int.MaxValue;
//
//		public int BankCount
//		{
//			get;
//		}
//
//		public int BankWidthBytes
//		{
//			get;
//		}
//
//		public MemoryBankGeometryGeneration Generation
//		{
//			get;
//		}
//
//		/// <summary>
//		/// Reports whether all three tuple components are representationally
//		/// valid. This does not prove publication or owner authority.
//		/// </summary>
//		public bool IsWellFormed
//		{
//			get
//			{
//				return AreComponentsRepresentable(BankCount, BankWidthBytes, Generation);
//			}
//		}
//
//		public PhysicalMemoryBankGeometry(int bankCount, int bankWidthBytes, MemoryBankGeometryGeneration generation)
//		{
//			if (!IsBankCountRepresentable(bankCount))
//			{
//				throw new ArgumentOutOfRangeException(nameof(bankCount), bankCount, "A physical memory-bank count must be positive.");
//			}
//
//			if (!IsBankWidthRepresentable(bankWidthBytes))
//			{
//				throw new ArgumentOutOfRangeException(nameof(bankWidthBytes), bankWidthBytes, "A physical memory-bank width must be positive.");
//			}
//
//			if (!generation.IsIssued)
//			{
//				throw new ArgumentOutOfRangeException(nameof(generation), generation, "A physical memory-bank geometry generation must be issued.");
//			}
//
//			BankCount = bankCount;
//			BankWidthBytes = bankWidthBytes;
//			Generation = generation;
//		}
//
//		/// <summary>
//		/// Returns whether a raw count is representable in a checked geometry.
//		/// Allocation feasibility remains a platform-owner decision.
//		/// </summary>
//		public static bool IsBankCountRepresentable(int bankCount)
//		{
//			return bankCount >= MinBankCount;
//		}
//
//		/// <summary>
//		/// Returns whether a raw width is representable in a checked geometry.
//		/// Allocation feasibility remains a platform-owner decision.
//		/// </summary>
//		public static bool IsBankWidthRepresentable(int bankWidthBytes)
//		{
//			return bankWidthBytes >= MinBankWidthBytes;
//		}
//
//		/// <summary>
//		/// Returns whether all components can form a checked geometry tuple.
//		/// </summary>
//		public static bool AreComponentsRepresentable(int bankCount, int bankWidthBytes, MemoryBankGeometryGeneration generation)
//		{
//			return IsBankCountRepresentable(bankCount) && IsBankWidthRepresentable(bankWidthBytes) && generation.IsIssued;
//		}
//
//		public static PhysicalMemoryBankGeometry Create(int bankCount, int bankWidthBytes, MemoryBankGeometryGeneration generation)
//		{
//			return new(bankCount, bankWidthBytes, generation);
//		}
//
//		public static bool TryCreate(int bankCount, int bankWidthBytes, MemoryBankGeometryGeneration generation, out PhysicalMemoryBankGeometry geometry)
//		{
//			if (AreComponentsRepresentable(bankCount, bankWidthBytes, generation))
//			{
//				geometry = new PhysicalMemoryBankGeometry(bankCount, bankWidthBytes, generation);
//				return true;
//			}
//
//			geometry = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return IsWellFormed ? string.Format("physical-memory-bank-geometry(count={0}, ", BankCount) + string.Format("width-bytes={0}, generation={1})", BankWidthBytes, Generation.Value) : "no-physical-memory-bank-geometry";
//		}
//	}

}

namespace YAKSys_Hybrid_CPU::Memory
{
class PhysicalMemoryBankGeometry final
{
public:
    static constexpr int MinBankCount = 1;
    static constexpr int MaxBankCount = INT_MAX;
    static constexpr int MinBankWidthBytes = 1;
    static constexpr int MaxBankWidthBytes = INT_MAX;
    const int BankCount;
    const int BankWidthBytes;
    const std::shared_ptr<MemoryBankGeometryGeneration> Generation;
    const bool IsWellFormed;

    PhysicalMemoryBankGeometry(int bankCount, int bankWidthBytes, std::shared_ptr<MemoryBankGeometryGeneration> generation)
        : BankCount(bankCount), BankWidthBytes(bankWidthBytes), Generation(std::move(generation)),
          IsWellFormed(AreComponentsRepresentable(bankCount, bankWidthBytes, Generation))
    {
        if (!IsWellFormed) throw std::invalid_argument("Invalid physical memory-bank geometry.");
    }
    static bool IsBankCountRepresentable(int bankCount) { return bankCount >= MinBankCount; }
    static bool IsBankWidthRepresentable(int bankWidthBytes) { return bankWidthBytes >= MinBankWidthBytes; }
    static bool AreComponentsRepresentable(int bankCount, int bankWidthBytes, const std::shared_ptr<MemoryBankGeometryGeneration>& generation)
    {
        return IsBankCountRepresentable(bankCount) && IsBankWidthRepresentable(bankWidthBytes) && generation && generation->isIssued();
    }
    static std::shared_ptr<PhysicalMemoryBankGeometry> Create(int bankCount, int bankWidthBytes, std::shared_ptr<MemoryBankGeometryGeneration> generation)
    {
        return std::make_shared<PhysicalMemoryBankGeometry>(bankCount, bankWidthBytes, std::move(generation));
    }
};
}