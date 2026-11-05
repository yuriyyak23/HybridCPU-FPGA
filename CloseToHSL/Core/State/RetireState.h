#pragma once

#include "../Architecture/Registers/Retire/RetireCoordinator.h"
#include "../Runtime/Completion/Commit/ArchitecturalCompletionCommitOwner.h"
#include "../Pipeline/Core/PipelineContourCertificate.h"
#include <string>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Containment for the existing retirement authority and cross-stage
	/// publication certificates. RetireCoordinator retains functional authority.
	/// </summary>
	class RetireState final : public std::enable_shared_from_this<RetireState>
	{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal RetireCoordinator Coordinator = null!;
	public:
		std::shared_ptr<RetireCoordinator> Coordinator = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal ArchitecturalCompletionCommitOwner CompletionCommitOwner = null!;
		std::shared_ptr<ArchitecturalCompletionCommitOwner> CompletionCommitOwner = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal ArchitecturalCompletionCommitOwner.ProducerRegistration CanonicalPipelineCompletionProducer = null!;
		std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> CanonicalPipelineCompletionProducer = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal ArchitecturalCompletionCommitOwner.ProducerRegistration CanonicalCpuTranslationFaultCompletionProducer = null!;
		std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> CanonicalCpuTranslationFaultCompletionProducer = nullptr;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
// ORIGINAL LINE: internal ArchitecturalCompletionCommitOwner.ProducerRegistration CanonicalCpuSecondStageTranslationFaultCompletionProducer = null!;
		std::shared_ptr<ArchitecturalCompletionCommitOwner::ProducerRegistration> CanonicalCpuSecondStageTranslationFaultCompletionProducer = nullptr;
		PipelineContourCertificate DecodePublicationCertificate;
		PipelineContourCertificate ExecuteCompletionCertificate;
		PipelineContourCertificate RetireVisibilityCertificate;
		unsigned long long LastRetiredBundlePc = 0;
		unsigned long long RetiredBundleSequence = 0;
		unsigned char LastRetiredLaneIndex = 0;
		unsigned int LastRetiredOpcode = 0;
		bool LastRetiredWritesRegister = false;
		unsigned short LastRetiredDestinationRegister = 0;
		unsigned long long LastRetiredResultValue = 0;
		std::wstring LastRetiredMicroOpType = std::wstring{};
		std::wstring LastRetiredMicroOpDescription = std::wstring{};
	};

}
