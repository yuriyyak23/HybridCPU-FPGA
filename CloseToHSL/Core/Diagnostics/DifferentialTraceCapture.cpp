#include "DifferentialTraceCapture.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DifferentialTraceEntry::DifferentialTraceEntry(unsigned long long pc, YAKSys_Hybrid_CPU::Core::DecodeMode decodeMode, unsigned char preparedScalarMask, unsigned char refinedPreparedScalarMask, int advisoryScalarIssueWidth, int refinedAdvisoryScalarIssueWidth, bool shouldProbeClusterPath, bool suggestsFallbackDiagnostics, bool retainsReferenceSequentialPath, unsigned char scalarCandidateMask, unsigned char blockedScalarCandidateMask, unsigned char registerHazardMask, unsigned char orderingHazardMask)
		: PC(pc),
		  DecodeMode(decodeMode),
		  PreparedScalarMask(preparedScalarMask),
		  RefinedPreparedScalarMask(refinedPreparedScalarMask),
		  AdvisoryScalarIssueWidth(advisoryScalarIssueWidth),
		  RefinedAdvisoryScalarIssueWidth(refinedAdvisoryScalarIssueWidth),
		  ShouldProbeClusterPath(shouldProbeClusterPath),
		  SuggestsFallbackDiagnostics(suggestsFallbackDiagnostics),
		  RetainsReferenceSequentialPath(retainsReferenceSequentialPath),
		  ScalarCandidateMask(scalarCandidateMask),
		  BlockedScalarCandidateMask(blockedScalarCandidateMask),
		  RegisterHazardMask(registerHazardMask),
		  OrderingHazardMask(orderingHazardMask)
	{
	}

	unsigned long long DifferentialTraceEntry::getPC() const
	{
		return PC;
	}

	YAKSys_Hybrid_CPU::Core::DecodeMode DifferentialTraceEntry::getDecodeMode() const
	{
		return DecodeMode;
	}

	unsigned char DifferentialTraceEntry::getPreparedScalarMask() const
	{
		return PreparedScalarMask;
	}

	unsigned char DifferentialTraceEntry::getRefinedPreparedScalarMask() const
	{
		return RefinedPreparedScalarMask;
	}

	int DifferentialTraceEntry::getAdvisoryScalarIssueWidth() const
	{
		return AdvisoryScalarIssueWidth;
	}

	int DifferentialTraceEntry::getRefinedAdvisoryScalarIssueWidth() const
	{
		return RefinedAdvisoryScalarIssueWidth;
	}

	bool DifferentialTraceEntry::getShouldProbeClusterPath() const
	{
		return ShouldProbeClusterPath;
	}

	bool DifferentialTraceEntry::getSuggestsFallbackDiagnostics() const
	{
		return SuggestsFallbackDiagnostics;
	}

	bool DifferentialTraceEntry::getRetainsReferenceSequentialPath() const
	{
		return RetainsReferenceSequentialPath;
	}

	unsigned char DifferentialTraceEntry::getScalarCandidateMask() const
	{
		return ScalarCandidateMask;
	}

	unsigned char DifferentialTraceEntry::getBlockedScalarCandidateMask() const
	{
		return BlockedScalarCandidateMask;
	}

	unsigned char DifferentialTraceEntry::getRegisterHazardMask() const
	{
		return RegisterHazardMask;
	}

	unsigned char DifferentialTraceEntry::getOrderingHazardMask() const
	{
		return OrderingHazardMask;
	}

	DifferentialTraceEntry DifferentialTraceEntry::FromAdvisoryChain(RuntimeClusterAdmissionCandidateView candidateView, RuntimeClusterAdmissionDecisionDraft decisionDraft)
	{
		return DifferentialTraceEntry(candidateView.getPC(), candidateView.getDecodeMode(), candidateView.getPreparedScalarMask(), candidateView.getRefinedPreparedScalarMask(), candidateView.getAdvisoryScalarIssueWidth(), candidateView.getRefinedAdvisoryScalarIssueWidth(), decisionDraft.getShouldProbeClusterPath(), candidateView.getSuggestsFallbackDiagnostics(), decisionDraft.getRetainsReferenceSequentialPath(), candidateView.getScalarCandidateMask(), candidateView.getBlockedScalarCandidateMask(), candidateView.getRegisterHazardMask(), candidateView.getOrderingHazardMask());
	}

	int DifferentialTraceCapture::getCount() const
	{
		return _entries.size();
	}

	void DifferentialTraceCapture::AddEntry(DifferentialTraceEntry entry)
	{
		_entries.push_back(entry);
	}

	const std::vector<DifferentialTraceEntry> DifferentialTraceCapture::GetEntries()
	{
		return _entries;
	}

	void DifferentialTraceCapture::Clear()
	{
		_entries.clear();
	}
}
