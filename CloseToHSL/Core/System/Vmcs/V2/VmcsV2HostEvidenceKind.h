#pragma once

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum VmcsV2HostEvidenceKind : uint
	enum class VmcsV2HostEvidenceKind : int
	{
		None = 0,
		DecodedBundleFacts = 1 << 0,
		MicroOpCache = 1 << 1,
		LaneBindingEvidence = 1 << 2,
		ReplayAssistState = 1 << 3,
		HostAcceleratorBackendHandles = 1 << 4,
		ValidationCache = 1 << 5,
		TlbCacheContents = 1 << 6,
		IotlbCacheContents = 1 << 7
	};

}
