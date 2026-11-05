#pragma once

#include "../../../CloseToHSL/Memory/Subsystem/MemorySubsystem.Types.h"
#include <unordered_map>
#include <optional>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU { class ProfilingOptions; }

using namespace HybridCPU_ISE::CloseToHSL::Memory::Subsystem;

namespace YAKSys_Hybrid_CPU
{
	/// <summary>
	/// Frontend profile that feeds the canonical ISA v2 backend.
	/// </summary>
	enum class FrontendMode : unsigned char
	{
		/// <summary>
		/// Native HybridCPU VLIW bundle/instruction frontend.
		/// </summary>
		NativeVLIW = 0

		/// <summary>
		/// RISC-V compatibility frontend lowered into the same canonical backend ISA.
		/// </summary>
		/// RvCompat = 1
	};

	/// <summary>
	/// Processor baseline mode for evaluation comparisons (Q1 Review §3).
	/// </summary>
	enum class BaselineMode : unsigned char
	{
		/// <summary>
		/// Full FSP-enabled mode (default production behavior).
		/// VLIW slot stealing is active across all threads.
		/// </summary>
		FSP_Enabled = 0,

		/// <summary>
		/// In-order baseline mode: disables FSP and simulates classical
		/// in-order instruction issue with stall logic (ARM Cortex-A55 style).
		/// Used to establish a physically realistic IPC competitor.
		/// </summary>
		InOrder_Baseline = 1
	};

	/// <summary>
	/// Configuration parameters for HybridCPU processor.
	/// Allows customization of memory subsystem, DMA, and performance modeling.
	/// </summary>
	class ProcessorConfig : public std::enable_shared_from_this<ProcessorConfig>
	{
	private:
		int NumMemoryBanks = 8;
		int BankWidthBytes = 64;
		int DmaThresholdBytes = 1024;
		unsigned long long ThreadDomainSize = 32 * 1024 * 1024;
		std::optional<std::unordered_map<int, unsigned long long>> CustomThreadDomainSizes;
		int AxiBoundary = 4096;
		int MaxBurstLength = 256;
		int NumMemoryPorts = 2;
		double BankBandwidthGBps = 25.6;
		BankArbitrationPolicy ArbitrationPolicy = BankArbitrationPolicy::RoundRobin;
		int NumDmaChannels = 8;
		int MinDmaBurstLength = 256;
		bool CycleAccuratePipeline = false;
		int L1CacheLatency = 4;
		int L2CacheLatency = 12;
		int DramLatency = 100;
		YAKSys_Hybrid_CPU::FrontendMode FrontendMode = FrontendMode::NativeVLIW;
		BaselineMode ExecutionMode = BaselineMode::FSP_Enabled;
		bool PipelinedFspEnabled = false;
		int SpeculationBudgetMax = 4;
		bool CreditFairnessEnabled = true;
		bool BankPressureTieBreakEnabled = true;
		bool ProfilingEnabled = false;
		std::shared_ptr<YAKSys_Hybrid_CPU::ProfilingOptions> ProfilingOptions = nullptr;

//		#region Memory Subsystem Configuration

		/// <summary>
		/// Number of memory banks (default: 8)
		/// </summary>
	public:
		int getNumMemoryBanks() const;
		void setNumMemoryBanks(int value);

		/// <summary>
		/// Width of each memory bank in bytes (default: 64)
		/// </summary>
		int getBankWidthBytes() const;
		void setBankWidthBytes(int value);

		/// <summary>
		/// Threshold for DMA routing in bytes (default: 1024)
		/// </summary>
		int getDmaThresholdBytes() const;
		void setDmaThresholdBytes(int value);

		/// <summary>
		/// Memory domain size per thread in bytes (default: 32MB = 16MB stack + 16MB heap)
		/// Phase 5 extension: Configurable per-thread memory isolation
		/// </summary>
		unsigned long long getThreadDomainSize() const;
		void setThreadDomainSize(unsigned long long value);

		/// <summary>
		/// Custom domain sizes for specific threads (optional)
		/// Key: thread ID (0-15), Value: domain size in bytes
		/// If not specified, ThreadDomainSize is used
		/// Phase 5 extension: Different domain sizes for different thread types
		/// </summary>
		std::optional<std::unordered_map<int, unsigned long long>> getCustomThreadDomainSizes() const;
		void setCustomThreadDomainSizes(const std::optional<std::unordered_map<int, unsigned long long>> &value);

		/// <summary>
		/// AXI4 boundary alignment requirement (default: 4096 = 4KB)
		/// </summary>
		int getAxiBoundary() const;
		void setAxiBoundary(int value);

		/// <summary>
		/// Maximum burst length in beats (default: 256 for AXI4)
		/// </summary>
		int getMaxBurstLength() const;
		void setMaxBurstLength(int value);

		/// <summary>
		/// Number of memory ports (default: 2)
		/// Controls concurrent memory accesses across banks
		/// </summary>
		int getNumMemoryPorts() const;
		void setNumMemoryPorts(int value);

		/// <summary>
		/// Memory bandwidth in GB/s per bank (default: 25.6)
		/// Used for adaptive stall calculation
		/// </summary>
		double getBankBandwidthGBps() const;
		void setBankBandwidthGBps(double value);

		/// <summary>
		/// Bank arbitration policy (default: RoundRobin)
		/// </summary>
		BankArbitrationPolicy getArbitrationPolicy() const;
		void setArbitrationPolicy(BankArbitrationPolicy value);

//		#endregion

//		#region DMA Controller Configuration

		/// <summary>
		/// Number of DMA channels (default: 8)
		/// </summary>
		int getNumDmaChannels() const;
		void setNumDmaChannels(int value);

		/// <summary>
		/// Minimum burst length for DMA usage (default: 256 bytes)
		/// </summary>
		int getMinDmaBurstLength() const;
		void setMinDmaBurstLength(int value);

//		#endregion

//		#region Pipeline Configuration

		/// <summary>
		/// Enable cycle-accurate pipeline simulation (default: false)
		/// When disabled, uses faster functional simulation
		/// </summary>
		bool getCycleAccuratePipeline() const;
		void setCycleAccuratePipeline(bool value);

		/// <summary>
		/// L1 cache hit latency in cycles (default: 4)
		/// </summary>
		int getL1CacheLatency() const;
		void setL1CacheLatency(int value);

		/// <summary>
		/// L2 cache hit latency in cycles (default: 12)
		/// </summary>
		int getL2CacheLatency() const;
		void setL2CacheLatency(int value);

		/// <summary>
		/// DRAM access latency in cycles (default: 100)
		/// </summary>
		int getDramLatency() const;
		void setDramLatency(int value);

//		#endregion

//		#region Execution Mode Configuration

		/// <summary>
		/// Frontend profile used to produce the canonical ISA v2 instruction stream.
		/// </summary>
		YAKSys_Hybrid_CPU::FrontendMode getFrontendMode() const;
		void setFrontendMode(YAKSys_Hybrid_CPU::FrontendMode value);

		/// <summary>
		/// Processor baseline mode for evaluation comparisons (Q1 Review §3).
		/// Controls whether FSP slot stealing is active or disabled for IPC baseline measurement.
		/// </summary>
		BaselineMode getExecutionMode() const;
		void setExecutionMode(BaselineMode value);

		/// <summary>
		/// Enable VLIW slot stealing via FSP (Fine-Grained Slot Pilfering).
		/// Convenience property: returns true when ExecutionMode == FSP_Enabled.
		/// Set to false to simulate an in-order baseline for evaluation.
		/// </summary>
		bool getVliwStealEnabled() const;
		void setVliwStealEnabled(bool value);

		/// <summary>
		/// Enable 2-stage pipelined FSP arbitration for HLS timing closure.
		/// When true, MicroOpScheduler uses SCHED1/SCHED2 pipeline stages
		/// instead of single-cycle combinational path (reduces LUT depth for synthesis).
		/// Default: false (single-cycle, backwards compatible).
		/// </summary>
		bool getPipelinedFspEnabled() const;
		void setPipelinedFspEnabled(bool value);

		/// <summary>
		/// Phase 2C: Maximum concurrent speculative operations allowed by the scheduler.
		/// When the in-flight speculative count reaches this cap, new speculative injections are blocked.
		/// HLS: 4-bit comparator threshold.
		/// Default: 4 (SCOREBOARD_SLOTS / 2).
		/// </summary>
		int getSpeculationBudgetMax() const;
		void setSpeculationBudgetMax(int value);

		/// <summary>
		/// Phase 2A: Enable credit-based deterministic fairness ranking in the scheduler.
		/// When true, VTs with higher accumulated credit are ranked first for injection.
		/// Default: true.
		/// </summary>
		bool getCreditFairnessEnabled() const;
		void setCreditFairnessEnabled(bool value);

		/// <summary>
		/// Phase 2B: Enable bank-pressure tie-breaking for memory-op candidates.
		/// When true, candidates targeting less-congested banks are preferred.
		/// Default: true.
		/// </summary>
		bool getBankPressureTieBreakEnabled() const;
		void setBankPressureTieBreakEnabled(bool value);

//		#endregion

//		#region Profiling Configuration

		/// <summary>
		/// Enable performance profiling (default: false)
		/// </summary>
		bool getProfilingEnabled() const;
		void setProfilingEnabled(bool value);

		/// <summary>
		/// Profiling options (null = use defaults when profiling is enabled)
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ProfilingOptions? ProfilingOptions {get;set;} = null;
		std::shared_ptr<YAKSys_Hybrid_CPU::ProfilingOptions> getProfilingOptions() const;
		void setProfilingOptions(std::shared_ptr<YAKSys_Hybrid_CPU::ProfilingOptions> value);

//		#endregion

//		#region Factory Methods

		/// <summary>
		/// Create default configuration for emulation mode
		/// </summary>
		static std::shared_ptr<ProcessorConfig> Default();

		/// <summary>
		/// Create configuration for high-performance FPGA simulation
		/// </summary>
		static std::shared_ptr<ProcessorConfig> HighPerformanceFPGA();

		/// <summary>
		/// Create configuration for detailed profiling
		/// </summary>
		static std::shared_ptr<ProcessorConfig> Profiling();

		/// <summary>
		/// Create configuration for testing (deterministic, minimal conflicts)
		/// </summary>
		static std::shared_ptr<ProcessorConfig> Testing();

//		#endregion
	};

	/// <summary>
	/// Profiling options for performance monitoring
	/// </summary>
	class ProfilingOptions : public std::enable_shared_from_this<ProfilingOptions>
	{
	private:
		bool CollectMemoryStats = true;
		bool CollectPipelineStats = true;
		bool CollectDMATransfers = true;
		bool CollectBurstTraces = false;
		bool CollectVectorStats = true;

		/// <summary>
		/// Collect memory subsystem statistics
		/// </summary>
	public:
		bool getCollectMemoryStats() const;
		void setCollectMemoryStats(bool value);

		/// <summary>
		/// Collect pipeline statistics (IPC, stalls, hazards)
		/// </summary>
		bool getCollectPipelineStats() const;
		void setCollectPipelineStats(bool value);

		/// <summary>
		/// Collect DMA transfer statistics
		/// </summary>
		bool getCollectDMATransfers() const;
		void setCollectDMATransfers(bool value);

		/// <summary>
		/// Collect detailed burst traces (memory intensive)
		/// </summary>
		bool getCollectBurstTraces() const;
		void setCollectBurstTraces(bool value);

		/// <summary>
		/// Collect vector ALU operation statistics
		/// </summary>
		bool getCollectVectorStats() const;
		void setCollectVectorStats(bool value);

		/// <summary>
		/// Create profiling options with all features enabled
		/// </summary>
		static std::shared_ptr<ProfilingOptions> All();

		/// <summary>
		/// Create minimal profiling options (memory only)
		/// </summary>
		static std::shared_ptr<ProfilingOptions> Minimal();

		/// <summary>
		/// Create default profiling options
		/// </summary>
		static std::shared_ptr<ProfilingOptions> Default();
	};
}
