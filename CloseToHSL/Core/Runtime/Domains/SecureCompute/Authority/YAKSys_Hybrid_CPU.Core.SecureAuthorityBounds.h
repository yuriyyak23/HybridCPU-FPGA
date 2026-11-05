#pragma once

#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureAuthorityBounds final : public std::enable_shared_from_this<SecureAuthorityBounds>
	{
	private:
		static inline const std::shared_ptr<SecureAuthorityBounds> None = std::make_shared<SecureAuthorityBounds>();
		const bool AllowsPrivateMemory;
		const bool AllowsSharedMemory;
		const bool AllowsIo;
		const bool AllowsDma;
		const bool AllowsHypercalls;
		const bool AllowsDebug;
		const bool AllowsMigration;
		const bool AllowsCompatibilityProjection;

	public:
		SecureAuthorityBounds();

		SecureAuthorityBounds(bool allowsPrivateMemory, bool allowsSharedMemory, bool allowsIo, bool allowsDma, bool allowsHypercalls, bool allowsDebug, bool allowsMigration, bool allowsCompatibilityProjection);

		static std::shared_ptr<SecureAuthorityBounds> getNone();

		bool getAllowsPrivateMemory() const;

		bool getAllowsSharedMemory() const;

		bool getAllowsIo() const;

		bool getAllowsDma() const;

		bool getAllowsHypercalls() const;

		bool getAllowsDebug() const;

		bool getAllowsMigration() const;

		bool getAllowsCompatibilityProjection() const;

		bool IsSubsetOf(std::shared_ptr<SecureAuthorityBounds> parent);
	};
}
