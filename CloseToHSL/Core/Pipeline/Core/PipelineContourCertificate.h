#pragma once

namespace YAKSys_Hybrid_CPU::Core
{
	enum class PipelineContourKind : unsigned char
	{
		None = 0,
		DecodePublication = 1,
		ExecuteCompletion = 2,
		RetireVisibility = 3
	};

	enum class PipelineContourOwner : unsigned char
	{
		None = 0,
		DecodedBundleTransportPublication = 1,
		SingleLaneMicroOpExecution = 2,
		ExplicitPacketExecution = 3,
		ReferenceExecution = 4,
		WriteBackRetireWindow = 5
	};

	enum class PipelineContourVisibilityStage : unsigned char
	{
		None = 0,
		Decode = 1,
		Execute = 2,
		Memory = 3,
		WriteBack = 4,
		DirectRetire = 5
	};

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct PipelineContourCertificate
	class PipelineContourCertificate
	{
	private:
		const PipelineContourKind Kind = PipelineContourKind::None;
		const PipelineContourOwner Owner = PipelineContourOwner::None;
		const PipelineContourVisibilityStage VisibilityStage = PipelineContourVisibilityStage::None;
		const unsigned long long Pc = 0;
		const unsigned char SlotMask = 0;
		const bool IsPublished = false;

		PipelineContourCertificate(PipelineContourKind kind, PipelineContourOwner owner, PipelineContourVisibilityStage visibilityStage, unsigned long long pc, unsigned char slotMask, bool isPublished);

	public:
		PipelineContourKind getKind() const;
		PipelineContourOwner getOwner() const;
		PipelineContourVisibilityStage getVisibilityStage() const;
		unsigned long long getPc() const;
		unsigned char getSlotMask() const;
		bool isPublished() const;

		static PipelineContourCertificate CreateEmpty(PipelineContourKind kind);

		static PipelineContourCertificate CreatePublished(PipelineContourKind kind, PipelineContourOwner owner, PipelineContourVisibilityStage visibilityStage, unsigned long long pc, unsigned char slotMask);

		PipelineContourCertificate() = default;
	};
}
