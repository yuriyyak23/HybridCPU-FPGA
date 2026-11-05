#pragma once

#include <string>
#include <format>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	enum class ValidationSeverity : unsigned char
	{
		/// <summary>No issue — validation passed cleanly.</summary>
		None = 0,

		/// <summary>Informational warning — validation passed but has a note.</summary>
		Warning = 1,

		/// <summary>Error — validation failed.</summary>
		Error = 2
	};
	class BundleMetadata;

	class SlotMetadata;
	/// <summary>
	/// Result of a metadata schema compatibility validation.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ValidationResult
	class ValidationResult
	{
	private:
		const ValidationSeverity Severity;
		const std::wstring Message;

		const bool _isValid;

		/// <summary>Whether the validation passed (possibly with a warning).</summary>
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Metadata IsValid is schema compatibility only; use IsMetadataSchemaCompatible to avoid authority ambiguity.", false)] public bool IsValid
	public:
		bool isValid() const;

		/// <summary>
		/// Metadata schema compatibility only. This does not grant runtime
		/// legality, execution, publication, commit, or retire authority.
		/// </summary>
		bool isMetadataSchemaCompatible() const;

		/// <summary>Severity of the result.</summary>
		ValidationSeverity getSeverity() const;

		/// <summary>Human-readable message, or null if no message.</summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public string? Message {get;}
		std::wstring getMessage() const;

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ValidationResult(bool isValid, ValidationSeverity severity, string? message)
		ValidationResult(bool isValid, ValidationSeverity severity, const std::wstring &message);

		/// <summary>Creates an OK result with no message.</summary>
	public:
		static ValidationResult Ok();

		/// <summary>Creates a warning result — validation passed, but with a note.</summary>
		static ValidationResult Warning(const std::wstring &message);

		/// <summary>Creates an error result — validation failed.</summary>
		static ValidationResult Error(const std::wstring &message);

		/// <inheritdoc/>
		std::wstring ToString() const;

		ValidationResult() : Severity(ValidationSeverity::Error), Message(L""), _isValid(false)
		{
		}
	};

	/// <summary>Severity level for a <see cref="ValidationResult"/>.</summary>

	/// <summary>
	/// Validates metadata schema compatibility between compiler-emitted
	/// and ISE-consumed metadata.
	/// <para>
	/// The ISE operates at schema version <see cref="MetadataSchemaVersion.Current"/>.
	/// Older metadata uses defaults for missing fields; newer metadata has unknown
	/// fields silently ignored.
	/// </para>
	/// </summary>
	class MetadataCompatibilityValidator final
	{
		/// <summary>
		/// Validate schema compatibility of the given <see cref="BundleMetadata"/>.
		/// Returns <see cref="ValidationResult.Ok"/> for exact version match,
		/// a <see cref="ValidationResult.Warning"/> for minor version skew,
		/// or an <see cref="ValidationResult.Error"/> for unsupported schema.
		/// </summary>
	public:
		static ValidationResult Validate(std::shared_ptr<BundleMetadata> metadata);

		/// <summary>
		/// Validate schema compatibility of the given <see cref="SlotMetadata"/>.
		/// </summary>
		static ValidationResult Validate(std::shared_ptr<SlotMetadata> metadata);
	};
}
