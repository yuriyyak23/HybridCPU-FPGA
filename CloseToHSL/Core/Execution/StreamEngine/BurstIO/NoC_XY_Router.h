#pragma once
#include <span>

#include "../../BurstIO/IBurstBackend.h"
#include <vector>
#include <memory>
#include "../../../../../tangible_rectangular_vectors.h"

namespace YAKSys_Hybrid_CPU::Execution
{
	/// <summary>
	/// NoC port direction for XY dimension-order routing.
	/// </summary>
	enum class NoCPort : unsigned char
	{
		Local = 0,
		East = 1,
		West = 2,
		North = 3,
		South = 4
	};

	/// <summary>
	/// NoC flit — minimal transfer unit in the 2D-Mesh network (tech.md §2).
	/// Carries burst data between Pods with routing metadata and domain isolation tags.
	/// HLS-compatible: fixed-size struct, no heap allocation.
	/// </summary>
	class NoCFlit
	{
		/// <summary>Destination Pod X coordinate</summary>
	public:
		int DestX = 0;

		/// <summary>Destination Pod Y coordinate</summary>
		int DestY = 0;

		/// <summary>Source Pod X coordinate</summary>
		int SrcX = 0;

		/// <summary>Source Pod Y coordinate</summary>
		int SrcY = 0;

		/// <summary>Device ID for IOMMU translation at destination</summary>
		unsigned long long DeviceId = 0;

		/// <summary>Target memory address at destination Pod</summary>
		unsigned long long Address = 0;

		/// <summary>Payload data (burst segment)</summary>
		std::vector<unsigned char> Payload;

		/// <summary>True = write, False = read request</summary>
		bool IsWrite = false;

		/// <summary>Domain tag for Singularity-style isolation (tech.md §3)</summary>
		unsigned long long DomainTag = 0;

		/// <summary>QoS priority level (from CSR NOC_ROUTE_CFG, 0xB03)</summary>
		unsigned char QosPriority = 0;

		/// <summary>Hop counter for diagnostics / TTL-like protection</summary>
		unsigned char HopCount = 0;

		/// <summary>
		/// Virtual channel ID (0 = high-priority/DMA completion, 1 = bulk data).
		/// Enables HoL-blocking avoidance: if VC0 is stalled, VC1 continues.
		/// </summary>
		unsigned char VirtualChannel = 0;
	};

	/// <summary>
	/// NoC XY Router — inter-Pod burst backend via 2D-Mesh Network-on-Chip (tech.md §2).
	///
	/// Key properties:
	/// - Implements IBurstBackend for transparent integration with existing burst pipeline
	/// - XY dimension-order routing: mathematically deadlock-free (route X first, then Y)
	/// - Only burst transfers between Pods (no inter-pod FSP — FSP is pod-local only)
	/// - Domain tag propagation through flit metadata for IOMMU/security compatibility
	/// - Credit-based flow control to prevent congestion
	///
	/// HLS constraints:
	/// - Fixed-size buffers per port (no dynamic allocation)
	/// - Deterministic routing decision (single-cycle mux in hardware)
	/// - Bounded hop count for timing closure
	/// </summary>
	class NoC_XY_Router : public IBurstBackend
	{
	private:
		const int LocalX;
		const int LocalY;
		int MaxInflightPerDomain = VC_BUFFER_DEPTH * 5;
		bool DomainVcIsolationEnabled = false;
		long long DomainStallCount = 0;
		long long FlitsRouted = 0;
		long long FlitsDelivered = 0;
		long long FlitsDropped = 0;

		/// <summary>Maximum flits per output port buffer (flow control)</summary>
		static constexpr int PORT_BUFFER_DEPTH = 8;

		/// <summary>Maximum hops before TTL expiry (8×8 grid worst case = 14 hops)</summary>
		static constexpr int MAX_HOPS = 16;

		/// <summary>Local Pod X coordinate in 2D-Mesh</summary>
	public:
		int getLocalX() const;

		/// <summary>Local Pod Y coordinate in 2D-Mesh</summary>
		int getLocalY() const;

		/// <summary>Number of virtual channels per physical port.</summary>
	private:
		static constexpr int VC_COUNT = 2;

		/// <summary>Buffer depth per VC per port (HLS: LUTRAM FIFO).</summary>
		static constexpr int VC_BUFFER_DEPTH = 4;

		/// <summary>Maximum tracked security domains for VC isolation (Q1 Review §4).</summary>
		static constexpr int MAX_DOMAINS = 16;

		/// <summary>Legacy credit counters per port (used by default VC0).</summary>
		std::vector<int> _credits = std::vector<int>(5);

		/// <summary>
		/// Per-VC credit counters: [port, vc].
		/// HLS: 5×2 = 10 registers (trivial).
		/// </summary>
// C# TO C++ CONVERTER NOTE: The following call to the 'RectangularVectors' helper class reproduces the rectangular array initialization that is automatic in C#:
// ORIGINAL LINE: private readonly int[,] _vcCredits = new int[5, VC_COUNT];
		std::vector<std::vector<int>> _vcCredits = RectangularVectors::rectangularIntVector(5, VC_COUNT);

		// ── Domain-Isolated Virtual Channels (Q1 Review §4) ─────────

		/// <summary>
		/// Per-domain inflight flit counter for queue bounding.
		/// Prevents one domain from saturating shared NoC resources.
		/// HLS: 16 × 8-bit saturating counters = 128 bits.
		/// </summary>
		std::vector<int> _domainInflightCount = std::vector<int>(MAX_DOMAINS);

		/// <summary>
		/// Maximum inflight flits per domain before local stall.
		/// Default: VC_BUFFER_DEPTH × 5 ports = 20, allowing full utilization
		/// when only one domain is active. Zero disables domain isolation.
		/// </summary>
	public:
		int getMaxInflightPerDomain() const;
		void setMaxInflightPerDomain(int value);

		/// <summary>
		/// Enable domain-to-VC isolation: when true, flit VC is selected
		/// based on DomainTag hash rather than flit.VirtualChannel field.
		/// HLS: single AND-gate on the VC mux select line.
		/// </summary>
		bool getDomainVcIsolationEnabled() const;
		void setDomainVcIsolationEnabled(bool value);

		/// <summary>Statistics: flits rejected due to domain inflight limit.</summary>
		long long getDomainStallCount() const;
	private:
		void setDomainStallCount(long long value);

		/// <summary>Local backend for executing burst operations when flit reaches destination</summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly IBurstBackend? _localBackend;
		const std::shared_ptr<IBurstBackend> _localBackend;

		/// <summary>Neighbor routers connected via directional links</summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private readonly NoC_XY_Router?[] _neighbors = new NoC_XY_Router?[5];
		std::vector<std::shared_ptr<NoC_XY_Router>> _neighbors = std::vector<std::shared_ptr<NoC_XY_Router>>(5);

		/// <summary>Statistics: total flits routed through this node</summary>
	public:
		long long getFlitsRouted() const;
	private:
		void setFlitsRouted(long long value);

		/// <summary>Statistics: flits delivered locally (destination reached)</summary>
	public:
		long long getFlitsDelivered() const;
	private:
		void setFlitsDelivered(long long value);

		/// <summary>Statistics: flits dropped due to TTL expiry or full buffer</summary>
	public:
		long long getFlitsDropped() const;
	private:
		void setFlitsDropped(long long value);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public NoC_XY_Router(int localX, int localY, IBurstBackend? localBackend = null)
		NoC_XY_Router(int localX, int localY, std::shared_ptr<IBurstBackend> localBackend = nullptr);

		/// <summary>
		/// Connect a neighbor router on a specific port direction.
		/// Called during NoC topology construction.
		/// </summary>
		void ConnectNeighbor(NoCPort direction, std::shared_ptr<NoC_XY_Router> neighbor);

		/// <summary>
		/// XY dimension-order routing: route flit towards destination.
		/// Algorithm: route X dimension first, then Y dimension.
		/// This ordering mathematically excludes routing deadlocks (tech.md §2).
		///
		/// HLS: synthesizes as a 2-level priority mux (compare X, then compare Y).
		/// Single-cycle routing decision in hardware.
		/// </summary>
		void RoutePacket(NoCFlit flit);

		/// <summary>
		/// Forward flit to neighbor via output port with per-VC credit-based flow control.
		/// HoL avoidance: if primary VC is blocked, low-priority flits try alternate VC.
		///
		/// Q1 Review §4 enhancement: when DomainVcIsolationEnabled is true, the VC is
		/// selected by hashing DomainTag, and per-domain inflight counters enforce queue
		/// bounding to prevent cross-domain timing interference.
		///
		/// HLS: 2-input mux per port, single-cycle decision.
		/// </summary>
	private:
		void SendToPort(NoCPort port, NoCFlit flit);

		/// <summary>
		/// Map a DomainTag to a Virtual Channel index (0 or 1).
		/// Uses simple hash to distribute domains across VCs.
		/// HLS: XOR-fold of DomainTag → single-bit result.
		/// </summary>
		inline static int MapDomainToVC(unsigned long long domainTag);

		/// <summary>
		/// Map a DomainTag to an inflight counter slot (0–15).
		/// HLS: low-4-bit XOR hash of DomainTag.
		/// </summary>
		inline static int MapDomainToSlot(unsigned long long domainTag);

		/// <summary>
		/// Release a domain inflight slot (called when flit is consumed at destination).
		/// </summary>
	public:
		void ReleaseDomainInflight(unsigned long long domainTag);

		/// <summary>
		/// Return credit to a port on VC0 (backward-compatible overload).
		/// </summary>
		void ReturnCredit(NoCPort port);

		/// <summary>
		/// Return credit to a specific virtual channel on a port.
		/// Called by downstream router after consuming flit.
		/// </summary>
		/// <param name="port">Port direction.</param>
		/// <param name="virtualChannel">Virtual channel ID (0 or 1).</param>
		void ReturnCredit(NoCPort port, int virtualChannel);

		/// <summary>
		/// Accept flit at destination: execute burst via local backend.
		/// Domain tag from flit metadata is propagated for IOMMU security checking.
		/// </summary>
	private:
		void AcceptLocalDMA(NoCFlit flit);

		// ===== IBurstBackend implementation: transparently routes burst ops through NoC =====

		/// <inheritdoc/>
	public:
		bool Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer) override;

		/// <inheritdoc/>
		bool Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer) override;

		/// <summary>
		/// Initiate a remote burst write via NoC routing to a target Pod.
		/// Creates a NoCFlit with routing metadata and injects it into the network.
		/// </summary>
		/// <param name="destPodX">Destination Pod X coordinate</param>
		/// <param name="destPodY">Destination Pod Y coordinate</param>
		/// <param name="deviceId">Device ID at destination</param>
		/// <param name="address">Target address at destination</param>
		/// <param name="data">Payload data</param>
		/// <param name="domainTag">Domain tag for isolation (tech.md §3)</param>
		void SendRemoteBurst(int destPodX, int destPodY, unsigned long long deviceId, unsigned long long address, std::vector<unsigned char> &data, unsigned long long domainTag = 0);

		/// <inheritdoc/>
		void RegisterAcceleratorDevice(unsigned long long deviceId, AcceleratorDMACapabilities capabilities) override;

		/// <inheritdoc/>
		std::shared_ptr<DMATransferToken> InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size) override;
	};
}
