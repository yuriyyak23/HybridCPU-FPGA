#include "AcceleratorOperationCapability.h"
#define NOMINMAX
#include <Windows.h>
#include <algorithm>
#include <limits>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities {
namespace {
bool OrdinalIgnoreCaseEqual(const std::wstring& a, const std::wstring& b) {
    const auto max = static_cast<std::size_t>(std::numeric_limits<int>::max());
    return a.size() <= max && b.size() <= max && CompareStringOrdinal(a.data(), static_cast<int>(a.size()), b.data(), static_cast<int>(b.size()), TRUE) == CSTR_EQUAL;
}
std::vector<std::wstring> NormalizeDatatypes(const std::vector<std::wstring>& values) {
    std::vector<std::wstring> result;
    for (const auto& value : values) {
        if (StringHelper::isEmptyOrWhiteSpace(value)) continue;
        const bool present = std::any_of(result.begin(), result.end(), [&](const auto& existing) { return OrdinalIgnoreCaseEqual(existing, value); });
        if (!present) result.push_back(value);
    }
    return result;
}
}
AcceleratorOperationCapability::AcceleratorOperationCapability(const std::wstring& operationKind, const std::vector<std::wstring>& supportedDatatypes, const std::vector<std::shared_ptr<AcceleratorShapeCapability>>& supportedShapes)
    : OperationKind(operationKind), SupportedDatatypes(NormalizeDatatypes(supportedDatatypes)), SupportedShapes(supportedShapes) {
    if (StringHelper::isEmptyOrWhiteSpace(operationKind)) throw std::invalid_argument("Operation kind is required.");
}
std::wstring AcceleratorOperationCapability::getOperationKind() const { return OperationKind; }
const std::vector<std::wstring> AcceleratorOperationCapability::getSupportedDatatypes() const { return SupportedDatatypes; }
const std::vector<std::shared_ptr<AcceleratorShapeCapability>> AcceleratorOperationCapability::getSupportedShapes() const { return SupportedShapes; }
bool AcceleratorOperationCapability::SupportsDatatype(const std::wstring& datatype) {
    if (StringHelper::isEmptyOrWhiteSpace(datatype)) return false;
    return std::any_of(SupportedDatatypes.begin(), SupportedDatatypes.end(), [&](const auto& value) { return OrdinalIgnoreCaseEqual(value, datatype); });
}
bool AcceleratorOperationCapability::SupportsShape(const std::wstring& shapeKind, unsigned long long elementCount, unsigned char rank) {
    return std::any_of(SupportedShapes.begin(), SupportedShapes.end(), [&](const auto& shape) { return shape->SupportsShape(shapeKind, elementCount, rank); });
}
}
