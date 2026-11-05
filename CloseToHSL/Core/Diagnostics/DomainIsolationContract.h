#pragma once

namespace YAKSys_Hybrid_CPU::Core::Diagnostics
{

	/// <summary>
	/// Formal compiler-facing contract for DomainTag-based isolation.
	/// Exposes structural queries only and does not participate in runtime policy decisions.
	/// </summary>
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct DomainIsolationContract
	class DomainIsolationContract
	{
		/// <summary>
		/// Returns <see langword="true"/> when both domains are non-zero and bitwise disjoint.
		/// DomainTag <c>0</c> is treated as kernel/unrestricted and is never considered disjoint.
		/// </summary>
		/// <param name="domainA">First domain tag.</param>
		/// <param name="domainB">Second domain tag.</param>
		/// <returns>
		/// <see langword="true"/> only when <paramref name="domainA"/> and <paramref name="domainB"/>
		/// are both non-zero and <c>(domainA &amp; domainB) == 0</c>.
		/// </returns>
	public:
		static bool AreDomainsDisjoint(unsigned long long domainA, unsigned long long domainB);
	};

}
