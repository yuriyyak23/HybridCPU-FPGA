#pragma once

#include "AcceleratorCommandDescriptor.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{

	enum class AcceleratorDescriptorTaxonomyStatus : unsigned char
	{
		CurrentRuntimeContour = 1,
		MetadataOnly = 2,
		Reserved = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorDescriptorTaxonomyKey(AcceleratorClassId AcceleratorClass, AcceleratorDeviceId AcceleratorId, AcceleratorOperationKind Operation, AcceleratorShapeKind Shape);
	struct AcceleratorDescriptorTaxonomyKey final {
		AcceleratorClassId AcceleratorClass;
		AcceleratorDeviceId AcceleratorId;
		AcceleratorOperationKind Operation;
		AcceleratorShapeKind Shape;
		bool operator==(const AcceleratorDescriptorTaxonomyKey&) const = default;
	};

	class AcceleratorDescriptorTaxonomyEntry final : public std::enable_shared_from_this<AcceleratorDescriptorTaxonomyEntry>
	{
	private:
		const std::shared_ptr<AcceleratorDescriptorTaxonomyKey> Key;
		const std::wstring Name;
		const AcceleratorDescriptorTaxonomyStatus Status;
		const std::wstring Boundary;

	public:
		AcceleratorDescriptorTaxonomyEntry(std::shared_ptr<AcceleratorDescriptorTaxonomyKey> key, const std::wstring &name, AcceleratorDescriptorTaxonomyStatus status, const std::wstring &boundary);

		std::shared_ptr<AcceleratorDescriptorTaxonomyKey> getKey() const;

		std::wstring getName() const;

		AcceleratorDescriptorTaxonomyStatus getStatus() const;

		std::wstring getBoundary() const;

		bool isCurrentRuntimeContour() const;

		bool isMetadataOnly() const;

		bool getGrantsDescriptorAcceptanceAuthority() const;

		bool getGrantsCapabilityAuthority() const;

		bool getGrantsTokenAuthority() const;

		bool getGrantsExecutionAuthority() const;

		bool getGrantsCommitAuthority() const;

		bool getGrantsCompilerEmissionAuthority() const;

		bool getGrantsTopologyQueryAuthority() const;

		bool getGrantsQueueOpenAuthority() const;

		bool getGrantsQueueBindAuthority() const;

		bool getGrantsQueueLifecycleAuthority() const;
	};

	class AcceleratorDescriptorTaxonomyCatalog final
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<AcceleratorDescriptorTaxonomyEntry> Entries = Array.AsReadOnly(_entries);

		static inline const std::vector<std::shared_ptr<AcceleratorDescriptorTaxonomyEntry>> _entries = {std::make_shared<AcceleratorDescriptorTaxonomyEntry>(std::make_shared<AcceleratorDescriptorTaxonomyKey>(AcceleratorClassId::Matrix, AcceleratorDeviceId::ReferenceMatMul, AcceleratorOperationKind::MatMul, AcceleratorShapeKind::Matrix2D), L"matrix.reference-matmul.v1", AcceleratorDescriptorTaxonomyStatus::CurrentRuntimeContour, L"Current L7-SDC Matrix/ReferenceMatMul taxonomy entry. Parser acceptance still requires the guard-backed descriptor chain, capability acceptance, token admission, queue/backend execution, and staged commit."), std::make_shared<AcceleratorDescriptorTaxonomyEntry>(std::make_shared<AcceleratorDescriptorTaxonomyKey>(AcceleratorClassId::Tensor, AcceleratorDeviceId::TensorMetadata, AcceleratorOperationKind::TensorContract, AcceleratorShapeKind::TensorND), L"tensor.metadata.v1", AcceleratorDescriptorTaxonomyStatus::MetadataOnly, L"Reserved Lane7 tensor taxonomy entry for VMX-facing descriptor planning. It is metadata-only and grants no descriptor acceptance, token, execution, commit, or compiler-emission authority."), std::make_shared<AcceleratorDescriptorTaxonomyEntry>(std::make_shared<AcceleratorDescriptorTaxonomyKey>(AcceleratorClassId::TopologyQueue, AcceleratorDeviceId::TopologyQueueMetadata, AcceleratorOperationKind::TopologyQueueContract, AcceleratorShapeKind::TopologyQueueMap), L"topology.queue.metadata.v1", AcceleratorDescriptorTaxonomyStatus::MetadataOnly, L"Reserved Lane7 topology/queue taxonomy contract for VMX preparation. It is metadata-only and grants no topology query, queue open, queue bind, descriptor acceptance, token, execution, commit, or compiler-emission authority."), std::make_shared<AcceleratorDescriptorTaxonomyEntry>(std::make_shared<AcceleratorDescriptorTaxonomyKey>(AcceleratorClassId::Fft, AcceleratorDeviceId::FftMetadata, AcceleratorOperationKind::FftContract, AcceleratorShapeKind::Fft1D), L"fft.metadata.v1", AcceleratorDescriptorTaxonomyStatus::MetadataOnly, L"Reserved Lane7 FFT descriptor taxonomy contract for richer signal-processing metadata planning. It is metadata-only and grants no descriptor acceptance, token, execution, backend, commit, or compiler-emission authority."), std::make_shared<AcceleratorDescriptorTaxonomyEntry>(std::make_shared<AcceleratorDescriptorTaxonomyKey>(AcceleratorClassId::CryptoHash, AcceleratorDeviceId::CryptoHashMetadata, AcceleratorOperationKind::CryptoHashContract, AcceleratorShapeKind::CryptoHashBlock), L"crypto.hash.metadata.v1", AcceleratorDescriptorTaxonomyStatus::MetadataOnly, L"Reserved Lane7 Crypto/Hash descriptor taxonomy contract for richer descriptor metadata planning. It is metadata-only and grants no descriptor acceptance, token, execution, backend, commit, or compiler-emission authority."), std::make_shared<AcceleratorDescriptorTaxonomyEntry>(std::make_shared<AcceleratorDescriptorTaxonomyKey>(AcceleratorClassId::SparseGraph, AcceleratorDeviceId::SparseGraphMetadata, AcceleratorOperationKind::SparseGraphContract, AcceleratorShapeKind::SparseGraphCsr), L"sparse.graph.metadata.v1", AcceleratorDescriptorTaxonomyStatus::MetadataOnly, L"Reserved Lane7 Sparse/Graph descriptor taxonomy contract for richer descriptor metadata planning. It is metadata-only and grants no descriptor acceptance, token, execution, backend, commit, or compiler-emission authority.")};

	public:
		static std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> getCurrentMatMul();

		static std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> getTensorMetadata();

		static std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> getTopologyQueueMetadata();

		static std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> getFftMetadata();

		static std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> getCryptoHashMetadata();

		static std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> getSparseGraphMetadata();

		static const std::vector<std::shared_ptr<AcceleratorDescriptorTaxonomyEntry>> getEntries();

		static bool TryGetEntry(std::shared_ptr<AcceleratorDescriptorTaxonomyKey> key, std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> &entry);

		static bool TryGetEntry(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorShapeKind shape, std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> &entry);
	};

}
