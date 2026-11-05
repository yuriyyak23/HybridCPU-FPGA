#pragma once

#include "AcceleratorShapeCapability.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include "../../../../../tangible_string_helper.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	class AcceleratorOperationCapability final : public std::enable_shared_from_this<AcceleratorOperationCapability>
	{
	private:
		const std::wstring OperationKind;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<string> SupportedDatatypes;
		const std::vector<std::wstring> SupportedDatatypes;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<AcceleratorShapeCapability> SupportedShapes;
		const std::vector<std::shared_ptr<AcceleratorShapeCapability>> SupportedShapes;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorOperationCapability(string operationKind, IReadOnlyList<string> supportedDatatypes, IReadOnlyList<AcceleratorShapeCapability>? supportedShapes = null)
		AcceleratorOperationCapability(const std::wstring &operationKind, const std::vector<std::wstring> &supportedDatatypes, const std::vector<std::shared_ptr<AcceleratorShapeCapability>> &supportedShapes = {});

		std::wstring getOperationKind() const;

		const std::vector<std::wstring> getSupportedDatatypes() const;

		const std::vector<std::shared_ptr<AcceleratorShapeCapability>> getSupportedShapes() const;

		bool SupportsDatatype(const std::wstring &datatype);

		bool SupportsShape(const std::wstring &shapeKind, unsigned long long elementCount, unsigned char rank = 0);
	};

}
