#pragma once
#include "HybridCpuIseBootBlobContractsV1.h"

#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <optional>
#include <memory>
#include <span>

using namespace HybridCPU::ManagedRuntime;
using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed;

namespace HybridCPU_ISE::NonRTL::Runtime
{

	/// <summary>Single-use, pre-CPU loader binding. Only fully copied, rooted guest arrays are published.</summary>
	class HybridCpuIseBootBlobBindingV1 final : public IHybridCpuManagedBootBlobProviderV1
	{
	private:
		std::optional<std::shared_ptr<HybridCpuManagedBootBlobRegistryV1>> _registry;
		bool _attempted = false;

	public:
		std::shared_ptr<HybridCpuManagedBootBlobProviderResultV1> Resolve(int blobId);

		bool TryBind(std::shared_ptr<HybridCpuIseManagedImageLoadResultV1> load, int blobId, std::span<const unsigned char> bytes, std::shared_ptr<HybridCpuIseManagedImageLoadResultV1> &bound, std::wstring &reason);
	};

}
