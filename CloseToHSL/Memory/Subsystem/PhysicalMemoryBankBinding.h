#pragma once
#include "PhysicalMemoryBankIndex.h"
#include "MemoryBankGeometryGeneration.h"
#include <memory>
#include <stdexcept>
#include <utility>

namespace YAKSys_Hybrid_CPU::Memory
{

	/// <summary>
	/// Immutable checked representation of one topology-local physical bank
	/// position paired with one issued geometry generation. Representation does
	/// not prove geometry ownership, index membership, same-snapshot provenance,
	/// request acceptance, queue membership, execution, completion,
	/// cancellation, replay, retirement, store visibility or publication
	/// authority.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct PhysicalMemoryBankBinding
//	{
//		public PhysicalMemoryBankIndex BankIndex
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
//		/// Reports whether both components are representationally usable as a
//		/// binding. This does not prove membership in or provenance from a
//		/// published geometry.
//		/// </summary>
//		public bool IsWellFormed
//		{
//			get
//			{
//				return AreComponentsRepresentable(BankIndex, Generation);
//			}
//		}
//
//		public PhysicalMemoryBankBinding(PhysicalMemoryBankIndex bankIndex, MemoryBankGeometryGeneration generation)
//		{
//			if (!PhysicalMemoryBankIndex.IsRepresentable(bankIndex.Value))
//			{
//				throw new ArgumentOutOfRangeException(nameof(bankIndex), bankIndex, "A physical memory-bank binding index must be non-negative.");
//			}
//
//			if (!generation.IsIssued)
//			{
//				throw new ArgumentOutOfRangeException(nameof(generation), generation, "A physical memory-bank binding generation must be issued.");
//			}
//
//			BankIndex = bankIndex;
//			Generation = generation;
//		}
//
//		/// <summary>
//		/// Returns whether both checked component representations can form a
//		/// binding carrier. Geometry membership remains an owner decision.
//		/// </summary>
//		public static bool AreComponentsRepresentable(PhysicalMemoryBankIndex bankIndex, MemoryBankGeometryGeneration generation)
//		{
//			return PhysicalMemoryBankIndex.IsRepresentable(bankIndex.Value) && generation.IsIssued;
//		}
//
//		public static PhysicalMemoryBankBinding Create(PhysicalMemoryBankIndex bankIndex, MemoryBankGeometryGeneration generation)
//		{
//			return new(bankIndex, generation);
//		}
//
//		public static bool TryCreate(PhysicalMemoryBankIndex bankIndex, MemoryBankGeometryGeneration generation, out PhysicalMemoryBankBinding binding)
//		{
//			if (AreComponentsRepresentable(bankIndex, generation))
//			{
//				binding = new PhysicalMemoryBankBinding(bankIndex, generation);
//				return true;
//			}
//
//			binding = default;
//			return false;
//		}
//
//		public override string ToString()
//		{
//			return IsWellFormed ? string.Format("physical-memory-bank-binding(index={0}, ", BankIndex.Value) + string.Format("generation={0})", Generation.Value) : "no-physical-memory-bank-binding";
//		}
//	}

}

namespace YAKSys_Hybrid_CPU::Memory
{
class PhysicalMemoryBankBinding final
{
public:
    const std::shared_ptr<PhysicalMemoryBankIndex> BankIndex;
    const std::shared_ptr<MemoryBankGeometryGeneration> Generation;
    const bool IsWellFormed;
    PhysicalMemoryBankBinding(std::shared_ptr<PhysicalMemoryBankIndex> bankIndex, std::shared_ptr<MemoryBankGeometryGeneration> generation)
        : BankIndex(std::move(bankIndex)), Generation(std::move(generation)), IsWellFormed(AreComponentsRepresentable(BankIndex, Generation))
    { if (!IsWellFormed) throw std::invalid_argument("Invalid physical memory-bank binding."); }
    static bool AreComponentsRepresentable(const std::shared_ptr<PhysicalMemoryBankIndex>& bankIndex, const std::shared_ptr<MemoryBankGeometryGeneration>& generation)
    { return bankIndex && PhysicalMemoryBankIndex::IsRepresentable(bankIndex->getValue()) && generation && generation->isIssued(); }
    static std::shared_ptr<PhysicalMemoryBankBinding> Create(std::shared_ptr<PhysicalMemoryBankIndex> bankIndex, std::shared_ptr<MemoryBankGeometryGeneration> generation)
    { return std::make_shared<PhysicalMemoryBankBinding>(std::move(bankIndex), std::move(generation)); }
};
}