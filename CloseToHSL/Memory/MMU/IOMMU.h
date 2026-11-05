#pragma once



namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{
	/// <summary>
	/// Access permissions for IOMMU mappings.
	/// </summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum IOMMUAccessPermissions
	enum class IOMMUAccessPermissions
	{
		None = 0,
		Read = 1,
		Write = 2,
		ReadWrite = Read | Write
	};
}

namespace YAKSys_Hybrid_CPU::Memory
{
	/// <summary>
	/// Input-Output Memory Management Unit (IOMMU).
	/// Provides address translation and access control for DMA devices.
	///
	/// Design principles:
	/// - Two-level page table (1024 directories x 1024 pages)
	/// - 4KB page size (matching typical system MMU)
	/// - TLB-style entry format: PPN | Permissions | Present
	/// - HLS-compatible: fixed-size arrays, no Dictionary
	///
	/// Phase 5 additions:
	/// - SIP-style memory domains for thread isolation
	/// - Domain allocation and enforcement
	/// - Formal isolation proof generation
	///
	/// Memory model:
	/// - Devices access memory via IO Virtual Addresses (IOVA)
	/// - IOMMU translates IOVA → Physical Address
	/// - Enforces read/write permissions per mapping
	/// - Per-thread domains ensure Singularity-style isolation
	/// </summary>
}
