#include "ProcessorConfig.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::Subsystem;

namespace YAKSys_Hybrid_CPU
{

	int ProcessorConfig::getNumMemoryBanks() const
	{
		return NumMemoryBanks;
	}

	void ProcessorConfig::setNumMemoryBanks(int value)
	{
		NumMemoryBanks = value;
	}

	int ProcessorConfig::getBankWidthBytes() const
	{
		return BankWidthBytes;
	}

	void ProcessorConfig::setBankWidthBytes(int value)
	{
		BankWidthBytes = value;
	}

	int ProcessorConfig::getDmaThresholdBytes() const
	{
		return DmaThresholdBytes;
	}

	void ProcessorConfig::setDmaThresholdBytes(int value)
	{
		DmaThresholdBytes = value;
	}

	unsigned long long ProcessorConfig::getThreadDomainSize() const
	{
		return ThreadDomainSize;
	}

	void ProcessorConfig::setThreadDomainSize(unsigned long long value)
	{
		ThreadDomainSize = value;
	}

	std::optional<std::unordered_map<int, unsigned long long>> ProcessorConfig::getCustomThreadDomainSizes() const
	{
		return CustomThreadDomainSizes;
	}

	void ProcessorConfig::setCustomThreadDomainSizes(const std::optional<std::unordered_map<int, unsigned long long>> &value)
	{
		CustomThreadDomainSizes = value;
	}

	int ProcessorConfig::getAxiBoundary() const
	{
		return AxiBoundary;
	}

	void ProcessorConfig::setAxiBoundary(int value)
	{
		AxiBoundary = value;
	}

	int ProcessorConfig::getMaxBurstLength() const
	{
		return MaxBurstLength;
	}

	void ProcessorConfig::setMaxBurstLength(int value)
	{
		MaxBurstLength = value;
	}

	int ProcessorConfig::getNumMemoryPorts() const
	{
		return NumMemoryPorts;
	}

	void ProcessorConfig::setNumMemoryPorts(int value)
	{
		NumMemoryPorts = value;
	}

	double ProcessorConfig::getBankBandwidthGBps() const
	{
		return BankBandwidthGBps;
	}

	void ProcessorConfig::setBankBandwidthGBps(double value)
	{
		BankBandwidthGBps = value;
	}

	BankArbitrationPolicy ProcessorConfig::getArbitrationPolicy() const
	{
		return ArbitrationPolicy;
	}

	void ProcessorConfig::setArbitrationPolicy(BankArbitrationPolicy value)
	{
		ArbitrationPolicy = value;
	}

	int ProcessorConfig::getNumDmaChannels() const
	{
		return NumDmaChannels;
	}

	void ProcessorConfig::setNumDmaChannels(int value)
	{
		NumDmaChannels = value;
	}

	int ProcessorConfig::getMinDmaBurstLength() const
	{
		return MinDmaBurstLength;
	}

	void ProcessorConfig::setMinDmaBurstLength(int value)
	{
		MinDmaBurstLength = value;
	}

	bool ProcessorConfig::getCycleAccuratePipeline() const
	{
		return CycleAccuratePipeline;
	}

	void ProcessorConfig::setCycleAccuratePipeline(bool value)
	{
		CycleAccuratePipeline = value;
	}

	int ProcessorConfig::getL1CacheLatency() const
	{
		return L1CacheLatency;
	}

	void ProcessorConfig::setL1CacheLatency(int value)
	{
		L1CacheLatency = value;
	}

	int ProcessorConfig::getL2CacheLatency() const
	{
		return L2CacheLatency;
	}

	void ProcessorConfig::setL2CacheLatency(int value)
	{
		L2CacheLatency = value;
	}

	int ProcessorConfig::getDramLatency() const
	{
		return DramLatency;
	}

	void ProcessorConfig::setDramLatency(int value)
	{
		DramLatency = value;
	}

	YAKSys_Hybrid_CPU::FrontendMode ProcessorConfig::getFrontendMode() const
	{
		return FrontendMode;
	}

	void ProcessorConfig::setFrontendMode(YAKSys_Hybrid_CPU::FrontendMode value)
	{
		FrontendMode = value;
	}

	BaselineMode ProcessorConfig::getExecutionMode() const
	{
		return ExecutionMode;
	}

	void ProcessorConfig::setExecutionMode(BaselineMode value)
	{
		ExecutionMode = value;
	}

	bool ProcessorConfig::getVliwStealEnabled() const
	{
		return getExecutionMode() == BaselineMode::FSP_Enabled;
	}

	void ProcessorConfig::setVliwStealEnabled(bool value)
	{
		setExecutionMode(value ? BaselineMode::FSP_Enabled : BaselineMode::InOrder_Baseline);
	}

	bool ProcessorConfig::getPipelinedFspEnabled() const
	{
		return PipelinedFspEnabled;
	}

	void ProcessorConfig::setPipelinedFspEnabled(bool value)
	{
		PipelinedFspEnabled = value;
	}

	int ProcessorConfig::getSpeculationBudgetMax() const
	{
		return SpeculationBudgetMax;
	}

	void ProcessorConfig::setSpeculationBudgetMax(int value)
	{
		SpeculationBudgetMax = value;
	}

	bool ProcessorConfig::getCreditFairnessEnabled() const
	{
		return CreditFairnessEnabled;
	}

	void ProcessorConfig::setCreditFairnessEnabled(bool value)
	{
		CreditFairnessEnabled = value;
	}

	bool ProcessorConfig::getBankPressureTieBreakEnabled() const
	{
		return BankPressureTieBreakEnabled;
	}

	void ProcessorConfig::setBankPressureTieBreakEnabled(bool value)
	{
		BankPressureTieBreakEnabled = value;
	}

	bool ProcessorConfig::getProfilingEnabled() const
	{
		return ProfilingEnabled;
	}

	void ProcessorConfig::setProfilingEnabled(bool value)
	{
		ProfilingEnabled = value;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::ProfilingOptions> ProcessorConfig::getProfilingOptions() const
	{
		return ProfilingOptions;
	}

	void ProcessorConfig::setProfilingOptions(std::shared_ptr<YAKSys_Hybrid_CPU::ProfilingOptions> value)
	{
		ProfilingOptions = value;
	}

	std::shared_ptr<ProcessorConfig> ProcessorConfig::Default()
	{
		return std::make_shared<ProcessorConfig>();
	}

	std::shared_ptr<ProcessorConfig> ProcessorConfig::HighPerformanceFPGA()
	{
		std::shared_ptr<ProcessorConfig> temp_var = std::make_shared<ProcessorConfig>();
		temp_var->setNumMemoryBanks(16);
		temp_var->setBankWidthBytes(128);
		temp_var->setDmaThresholdBytes(512);
		temp_var->setNumDmaChannels(16);
		temp_var->setNumMemoryPorts(4);
		temp_var->setBankBandwidthGBps(51.2);
		temp_var->setArbitrationPolicy(BankArbitrationPolicy::WeightedFair);
		temp_var->setL1CacheLatency(2);
		temp_var->setL2CacheLatency(8);
		temp_var->setDramLatency(80);
		return temp_var;
	}

	std::shared_ptr<ProcessorConfig> ProcessorConfig::Profiling()
	{
		std::shared_ptr<ProcessorConfig> temp_var = std::make_shared<ProcessorConfig>();
		temp_var->setProfilingEnabled(true);
		temp_var->setCycleAccuratePipeline(true);
		temp_var->setProfilingOptions(ProfilingOptions::All());
		return temp_var;
	}

	std::shared_ptr<ProcessorConfig> ProcessorConfig::Testing()
	{
		std::shared_ptr<ProcessorConfig> temp_var = std::make_shared<ProcessorConfig>();
		temp_var->setNumMemoryBanks(1);
		temp_var->setBankWidthBytes(64);
		temp_var->setDmaThresholdBytes(1024);
		temp_var->setProfilingEnabled(false);
		return temp_var;
	}

	bool ProfilingOptions::getCollectMemoryStats() const
	{
		return CollectMemoryStats;
	}

	void ProfilingOptions::setCollectMemoryStats(bool value)
	{
		CollectMemoryStats = value;
	}

	bool ProfilingOptions::getCollectPipelineStats() const
	{
		return CollectPipelineStats;
	}

	void ProfilingOptions::setCollectPipelineStats(bool value)
	{
		CollectPipelineStats = value;
	}

	bool ProfilingOptions::getCollectDMATransfers() const
	{
		return CollectDMATransfers;
	}

	void ProfilingOptions::setCollectDMATransfers(bool value)
	{
		CollectDMATransfers = value;
	}

	bool ProfilingOptions::getCollectBurstTraces() const
	{
		return CollectBurstTraces;
	}

	void ProfilingOptions::setCollectBurstTraces(bool value)
	{
		CollectBurstTraces = value;
	}

	bool ProfilingOptions::getCollectVectorStats() const
	{
		return CollectVectorStats;
	}

	void ProfilingOptions::setCollectVectorStats(bool value)
	{
		CollectVectorStats = value;
	}

	std::shared_ptr<ProfilingOptions> ProfilingOptions::All()
	{
		std::shared_ptr<ProfilingOptions> temp_var = std::make_shared<ProfilingOptions>();
		temp_var->setCollectMemoryStats(true);
		temp_var->setCollectPipelineStats(true);
		temp_var->setCollectDMATransfers(true);
		temp_var->setCollectBurstTraces(true);
		temp_var->setCollectVectorStats(true);
		return temp_var;
	}

	std::shared_ptr<ProfilingOptions> ProfilingOptions::Minimal()
	{
		std::shared_ptr<ProfilingOptions> temp_var = std::make_shared<ProfilingOptions>();
		temp_var->setCollectMemoryStats(true);
		temp_var->setCollectPipelineStats(false);
		temp_var->setCollectDMATransfers(false);
		temp_var->setCollectBurstTraces(false);
		temp_var->setCollectVectorStats(false);
		return temp_var;
	}

	std::shared_ptr<ProfilingOptions> ProfilingOptions::Default()
	{
		std::shared_ptr<ProfilingOptions> temp_var = std::make_shared<ProfilingOptions>();
		temp_var->setCollectMemoryStats(true);
		temp_var->setCollectPipelineStats(true);
		temp_var->setCollectDMATransfers(true);
		temp_var->setCollectBurstTraces(false);
		temp_var->setCollectVectorStats(true);
		return temp_var;
	}
}
