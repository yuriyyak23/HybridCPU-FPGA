#include "MetadataCompatibilityValidator.h"
#include "../../../../NonRTL/Arch/MetadataSchemaVersion.h"
#include "BundleMetadata.h"
#include "SlotMetadata.h"

namespace YAKSys_Hybrid_CPU::Core
{

	bool ValidationResult::isValid() const
	{
		return _isValid;
	}

	bool ValidationResult::isMetadataSchemaCompatible() const
	{
		return getSeverity() != ValidationSeverity::Error;
	}

	ValidationSeverity ValidationResult::getSeverity() const
	{
		return Severity;
	}

	std::wstring ValidationResult::getMessage() const
	{
		return Message;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ValidationResult(bool isValid, ValidationSeverity severity, string? message)
	ValidationResult::ValidationResult(bool isValid, ValidationSeverity severity, const std::wstring &message)
		: Severity(severity), Message(message), _isValid(isValid)
	{
	}

	ValidationResult ValidationResult::Ok()
	{
		return ValidationResult(true, ValidationSeverity::None, L"");
	}

	ValidationResult ValidationResult::Warning(const std::wstring &message)
	{
		return ValidationResult(true, ValidationSeverity::Warning, message);
	}

	ValidationResult ValidationResult::Error(const std::wstring &message)
	{
		return ValidationResult(false, ValidationSeverity::Error, message);
	}

	std::wstring ValidationResult::ToString() const
	{
		if (getSeverity() == ValidationSeverity::None)
			return L"Ok";
		const wchar_t *severityText = L"Error";
		switch (getSeverity())
		{
		case ValidationSeverity::Warning:
			severityText = L"Warning";
			break;
		case ValidationSeverity::Error:
			severityText = L"Error";
			break;
		case ValidationSeverity::None:
			severityText = L"None";
			break;
		}
		return std::wstring(severityText) + L": " + getMessage();
	}

	ValidationResult MetadataCompatibilityValidator::Validate(std::shared_ptr<BundleMetadata> metadata)
	{
		unsigned char version = metadata->SchemaVersion;

		if (version == MetadataSchemaVersion::Current)
		{
			return ValidationResult::Ok();
		}

		if (version > MetadataSchemaVersion::Current)
		{
			return ValidationResult::Warning(std::format(L"BundleMetadata schema v{} is newer than ISE v{} — unknown fields will be ignored", version, MetadataSchemaVersion::Current));
		}

		// version < Current
		return ValidationResult::Warning(std::format(L"BundleMetadata schema v{} is older than ISE v{} — missing fields will use defaults", version, MetadataSchemaVersion::Current));
	}

	ValidationResult MetadataCompatibilityValidator::Validate(std::shared_ptr<SlotMetadata> metadata)
	{
		unsigned char version = metadata->SchemaVersion;

		if (version == MetadataSchemaVersion::Current)
		{
			return ValidationResult::Ok();
		}

		if (version > MetadataSchemaVersion::Current)
		{
			return ValidationResult::Warning(std::format(L"SlotMetadata schema v{} is newer than ISE v{} — unknown fields will be ignored", version, MetadataSchemaVersion::Current));
		}

		return ValidationResult::Warning(std::format(L"SlotMetadata schema v{} is older than ISE v{} — missing fields will use defaults", version, MetadataSchemaVersion::Current));
	}
}
