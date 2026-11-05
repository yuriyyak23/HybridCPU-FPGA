#include "ExecutionFaultContract.h"
#include "../Pipeline/UnsupportedExecutionSurfaceException.h"
#include "InvalidInternalOpException.h"

namespace YAKSys_Hybrid_CPU::Core
{

    std::wstring ExecutionFaultContract::GetMessagePrefix(ExecutionFaultCategory category)
    {
        const wchar_t* name = nullptr;
        switch (category) {
        case ExecutionFaultCategory::UnsupportedExecutionSurface: name = L"UnsupportedExecutionSurface"; break;
        case ExecutionFaultCategory::InvalidInternalOp: name = L"InvalidInternalOp"; break;
        case ExecutionFaultCategory::UnsupportedVectorElementType: name = L"UnsupportedVectorElementType"; break;
        case ExecutionFaultCategory::ReferenceModelValidation: name = L"ReferenceModelValidation"; break;
        case ExecutionFaultCategory::InvalidPipelineLane: name = L"InvalidPipelineLane"; break;
        case ExecutionFaultCategory::InvalidVirtualThread: name = L"InvalidVirtualThread"; break;
        default: return PrefixLead + std::to_wstring(static_cast<unsigned int>(category)) + L"]";
        }
        return PrefixLead + name + L"]";
    }

    std::wstring ExecutionFaultContract::FormatMessage(ExecutionFaultCategory category, const std::wstring &detail)
    {
        if (StringHelper::isEmptyOrWhiteSpace(detail))
            throw std::invalid_argument("Execution fault detail must not be null or empty.");
        return GetMessagePrefix(category) + L" " + detail;
    }
    void ExecutionFaultContract::Stamp(std::exception& exception, ExecutionFaultCategory category)
    {
        if (auto metadata = dynamic_cast<ExecutionFaultMetadata*>(&exception))
            metadata->executionFaultCategory = static_cast<unsigned char>(category);
    }

    ExecutionFaultCategory ExecutionFaultContract::GetCategory(const std::exception& exception)
    {
        ExecutionFaultCategory category;
        if (TryGetCategory(exception, category)) return category;
        throw std::invalid_argument("Exception does not carry a stable execution fault category.");
    }

    bool ExecutionFaultContract::TryGetCategory(const std::exception& exception, ExecutionFaultCategory& category)
    {
        if (TryGetDirectCategory(exception, category)) return true;
        if (auto metadata = dynamic_cast<const ExecutionFaultMetadata*>(&exception); metadata && metadata->innerException) {
            try { std::rethrow_exception(metadata->innerException); }
            catch (const std::exception& inner) { if (TryGetCategory(inner, category)) return true; }
            catch (...) {}
        }
        category = static_cast<ExecutionFaultCategory>(0);
        return false;
    }

    InvalidOperationException ExecutionFaultContract::CreateUnsupportedVectorElementTypeException(const std::wstring& detail)
    {
        InvalidOperationException exception(FormatMessage(ExecutionFaultCategory::UnsupportedVectorElementType, detail));
        Stamp(exception, ExecutionFaultCategory::UnsupportedVectorElementType);
        return exception;
    }

    ArgumentOutOfRangeException ExecutionFaultContract::CreateReferenceModelValidationException(const std::wstring& paramName, std::any actualValue, const std::wstring& detail)
    {
        ArgumentOutOfRangeException exception(paramName, actualValue, FormatMessage(ExecutionFaultCategory::ReferenceModelValidation, detail));
        Stamp(exception, ExecutionFaultCategory::ReferenceModelValidation);
        return exception;
    }

    InvalidOperationException ExecutionFaultContract::CreateWrappedException(ExecutionFaultCategory category, const std::wstring& detail, std::exception_ptr innerException)
    {
        if (!innerException) throw std::invalid_argument("innerException");
        InvalidOperationException exception(FormatMessage(category, detail));
        exception.innerException = std::move(innerException);
        Stamp(exception, category);
        return exception;
    }

    bool ExecutionFaultContract::TryGetDirectCategory(const std::exception& exception, ExecutionFaultCategory& category)
    {
        if (auto metadata = dynamic_cast<const ExecutionFaultMetadata*>(&exception); metadata && metadata->executionFaultCategory) {
            category = static_cast<ExecutionFaultCategory>(*metadata->executionFaultCategory);
            return true;
        }
        const auto message = exception.what();
        if (message) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            try { if (TryParseCategoryFromMessagePrefix(converter.from_bytes(message), category)) return true; }
            catch (const std::range_error&) {}
        }
        category = static_cast<ExecutionFaultCategory>(0);
        return false;
    }
    bool ExecutionFaultContract::TryParseCategoryFromMessagePrefix(const std::wstring &message, ExecutionFaultCategory &category)
    {
        category = static_cast<ExecutionFaultCategory>(0);
        if (message.compare(0, PrefixLead.size(), PrefixLead) != 0) return false;
        const auto end = message.find(L']', PrefixLead.size());
        if (end == std::wstring::npos || end == PrefixLead.size()) return false;
        const auto name = message.substr(PrefixLead.size(), end - PrefixLead.size());
        if (name == L"UnsupportedExecutionSurface") category = ExecutionFaultCategory::UnsupportedExecutionSurface;
        else if (name == L"InvalidInternalOp") category = ExecutionFaultCategory::InvalidInternalOp;
        else if (name == L"UnsupportedVectorElementType") category = ExecutionFaultCategory::UnsupportedVectorElementType;
        else if (name == L"ReferenceModelValidation") category = ExecutionFaultCategory::ReferenceModelValidation;
        else if (name == L"InvalidPipelineLane") category = ExecutionFaultCategory::InvalidPipelineLane;
        else if (name == L"InvalidVirtualThread") category = ExecutionFaultCategory::InvalidVirtualThread;
        else {
            try {
                std::size_t count = 0;
                const auto numeric = std::stoul(name, &count);
                if (count != name.size() || numeric > 255) return false;
                category = static_cast<ExecutionFaultCategory>(numeric);
            } catch (const std::exception&) { return false; }
        }
        return true;
    }
}
