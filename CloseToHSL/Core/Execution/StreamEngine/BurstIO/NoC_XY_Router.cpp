#include "NoC_XY_Router.h"
#include "../../BurstIO/AcceleratorRuntimeFailClosed.h"

namespace YAKSys_Hybrid_CPU::Execution
{

	int NoC_XY_Router::getLocalX() const
	{
		return LocalX;
	}

	int NoC_XY_Router::getLocalY() const
	{
		return LocalY;
	}

	int NoC_XY_Router::getMaxInflightPerDomain() const
	{
		return MaxInflightPerDomain;
	}

	void NoC_XY_Router::setMaxInflightPerDomain(int value)
	{
		MaxInflightPerDomain = value;
	}

	bool NoC_XY_Router::getDomainVcIsolationEnabled() const
	{
		return DomainVcIsolationEnabled;
	}

	void NoC_XY_Router::setDomainVcIsolationEnabled(bool value)
	{
		DomainVcIsolationEnabled = value;
	}

	long long NoC_XY_Router::getDomainStallCount() const
	{
		return DomainStallCount;
	}

	void NoC_XY_Router::setDomainStallCount(long long value)
	{
		DomainStallCount = value;
	}

	long long NoC_XY_Router::getFlitsRouted() const
	{
		return FlitsRouted;
	}

	void NoC_XY_Router::setFlitsRouted(long long value)
	{
		FlitsRouted = value;
	}

	long long NoC_XY_Router::getFlitsDelivered() const
	{
		return FlitsDelivered;
	}

	void NoC_XY_Router::setFlitsDelivered(long long value)
	{
		FlitsDelivered = value;
	}

	long long NoC_XY_Router::getFlitsDropped() const
	{
		return FlitsDropped;
	}

	void NoC_XY_Router::setFlitsDropped(long long value)
	{
		FlitsDropped = value;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public NoC_XY_Router(int localX, int localY, IBurstBackend? localBackend = null)
	NoC_XY_Router::NoC_XY_Router(int localX, int localY, std::shared_ptr<IBurstBackend> localBackend)
		: LocalX(localX), LocalY(localY), _vcCredits(RectangularVectors::rectangularIntVector(5, VC_COUNT)),
		  _domainInflightCount(MAX_DOMAINS), _localBackend(localBackend)
	{

		// Initialize credits (all ports start with full buffer capacity)
		for (int i = 0; i < _credits.size(); i++)
		{
			_credits[i] = PORT_BUFFER_DEPTH;
		}

		// Initialize per-VC credits
		for (int i = 0; i < 5; i++)
		{
			for (int vc = 0; vc < VC_COUNT; vc++)
			{
				_vcCredits[i][vc] = VC_BUFFER_DEPTH;
			}
		}
	}

	void NoC_XY_Router::ConnectNeighbor(NoCPort direction, std::shared_ptr<NoC_XY_Router> neighbor)
	{
		_neighbors[static_cast<int>(direction)] = neighbor;
	}

	void NoC_XY_Router::RoutePacket(NoCFlit flit)
	{
		// TTL protection against routing loops (should never happen with XY, but defense-in-depth)
		if (flit.HopCount >= MAX_HOPS)
		{
			setFlitsDropped(getFlitsDropped() + 1);
			return;
		}

		flit.HopCount++;
		setFlitsRouted(getFlitsRouted() + 1);

		// XY routing: X dimension first, then Y
		if (flit.DestX != getLocalX())
		{
			// Route along X axis
			NoCPort port = flit.DestX > getLocalX() ? NoCPort::East : NoCPort::West;
			SendToPort(port, flit);
		}
		else if (flit.DestY != getLocalY())
		{
			// Route along Y axis
			NoCPort port = flit.DestY > getLocalY() ? NoCPort::North : NoCPort::South;
			SendToPort(port, flit);
		}
		else
		{
			// Flit has reached destination Pod — deliver locally
			AcceptLocalDMA(flit);
		}
	}

	void NoC_XY_Router::SendToPort(NoCPort port, NoCFlit flit)
	{
		int portIdx = static_cast<int>(port);
		std::shared_ptr<NoC_XY_Router> neighbor = _neighbors[portIdx];

		if (neighbor == nullptr)
		{
			setFlitsDropped(getFlitsDropped() + 1);
			return;
		}

		// Q1 Review §4: Domain inflight queue bounding
		if (getDomainVcIsolationEnabled() && getMaxInflightPerDomain() > 0 && flit.DomainTag != 0)
		{
			int domainSlot = MapDomainToSlot(flit.DomainTag);
			if (_domainInflightCount[domainSlot] >= getMaxInflightPerDomain())
			{
				// Domain has saturated its allocation — stall locally
				setDomainStallCount(getDomainStallCount() + 1);
				setFlitsDropped(getFlitsDropped() + 1);
				return;
			}
			_domainInflightCount[domainSlot]++;
		}

		int vc = flit.VirtualChannel;

		// Q1 Review §4: Domain-isolated VC selection
		if (getDomainVcIsolationEnabled() && flit.DomainTag != 0)
		{
			vc = MapDomainToVC(flit.DomainTag);
			flit.VirtualChannel = static_cast<unsigned char>(vc);
		}

		// Per-VC credit check (does not block other VC)
		if (_vcCredits[portIdx][vc] <= 0)
		{
			// HoL avoidance: try alternate VC for non-priority traffic
			int altVc = 1 - vc;
			if (_vcCredits[portIdx][altVc] > 0 && flit.QosPriority == 0)
			{
				vc = altVc;
				flit.VirtualChannel = static_cast<unsigned char>(altVc);
			}
			else
			{
				setFlitsDropped(getFlitsDropped() + 1);
				return;
			}
		}

		_vcCredits[portIdx][vc]--;
		_credits[portIdx] = _vcCredits[portIdx][0] + _vcCredits[portIdx][1]; // Sync legacy counter
		neighbor->RoutePacket(flit);
	}

	inline int NoC_XY_Router::MapDomainToVC(unsigned long long domainTag)
	{
		// XOR-fold 64 bits to 1 bit for VC selection
		return static_cast<int>((domainTag ^ (domainTag >> 32)) & 1);
	}

	inline int NoC_XY_Router::MapDomainToSlot(unsigned long long domainTag)
	{
		return static_cast<int>((domainTag ^ (domainTag >> 4)) & 0xF);
	}

	void NoC_XY_Router::ReleaseDomainInflight(unsigned long long domainTag)
	{
		if (!getDomainVcIsolationEnabled() || domainTag == 0)
		{
			return;
		}

		int domainSlot = MapDomainToSlot(domainTag);
		if (_domainInflightCount[domainSlot] > 0)
		{
			_domainInflightCount[domainSlot]--;
		}
	}

	void NoC_XY_Router::ReturnCredit(NoCPort port)
	{
		ReturnCredit(port, 0);
	}

	void NoC_XY_Router::ReturnCredit(NoCPort port, int virtualChannel)
	{
		int portIdx = static_cast<int>(port);
		if (_vcCredits[portIdx][virtualChannel] < VC_BUFFER_DEPTH)
		{
			_vcCredits[portIdx][virtualChannel]++;
		}

		_credits[portIdx] = _vcCredits[portIdx][0] + _vcCredits[portIdx][1];
	}

	void NoC_XY_Router::AcceptLocalDMA(NoCFlit flit)
	{
		setFlitsDelivered(getFlitsDelivered() + 1);

		if (_localBackend == nullptr)
		{
			return;
		}

		if (flit.IsWrite && !flit.Payload.empty())
		{
			_localBackend->Write(flit.DeviceId, flit.Address, flit.Payload);
		}
		else if (!flit.IsWrite)
		{
			// For read requests, allocate response buffer
			// In real hardware this would be a registered buffer
			std::vector<unsigned char> buffer(flit.Payload.empty() ? 64 : flit.Payload.size());
			_localBackend->Read(flit.DeviceId, flit.Address, buffer);
		}
	}

	bool NoC_XY_Router::Read(unsigned long long deviceID, unsigned long long address, std::span<unsigned char> buffer)
	{
		// Local read: delegate to local backend if available
		if (_localBackend != nullptr)
		{
			return _localBackend->Read(deviceID, address, buffer);
		}

		return false;
	}

	bool NoC_XY_Router::Write(unsigned long long deviceID, unsigned long long address, std::span<const unsigned char> buffer)
	{
		// Local write: delegate to local backend if available
		if (_localBackend != nullptr)
		{
			return _localBackend->Write(deviceID, address, buffer);
		}

		return false;
	}

	void NoC_XY_Router::SendRemoteBurst(int destPodX, int destPodY, unsigned long long deviceId, unsigned long long address, std::vector<unsigned char> &data, unsigned long long domainTag)
	{
		NoCFlit flit = {.DestX = destPodX, .DestY = destPodY, .SrcX = getLocalX(), .SrcY = getLocalY(), .DeviceId = deviceId, .Address = address, .Payload = data, .IsWrite = true, .DomainTag = domainTag, .QosPriority = 0, .HopCount = 0, .VirtualChannel = 1};

		RoutePacket(flit);
	}

	void NoC_XY_Router::RegisterAcceleratorDevice(unsigned long long deviceId, AcceleratorDMACapabilities capabilities)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: IBurstBackend? localBackend = _localBackend;
		std::shared_ptr<IBurstBackend> localBackend = _localBackend;
		if (localBackend == nullptr)
		{
			AcceleratorRuntimeFailClosed::ThrowRegistrationNotSupported();
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: localBackend!.RegisterAcceleratorDevice(deviceId, capabilities);
		localBackend->RegisterAcceleratorDevice(deviceId, capabilities);
	}

	std::shared_ptr<DMATransferToken> NoC_XY_Router::InitiateAcceleratorDMA(unsigned long long deviceId, unsigned long long srcAddr, unsigned long long dstAddr, int size)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: IBurstBackend? localBackend = _localBackend;
		std::shared_ptr<IBurstBackend> localBackend = _localBackend;
		if (localBackend != nullptr)
		{
			return localBackend->InitiateAcceleratorDMA(deviceId, srcAddr, dstAddr, size);
		}

		return AcceleratorRuntimeFailClosed::ThrowTransferNotSupported();
	}
}
