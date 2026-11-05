#include "ClusterIssueIntent.h"
#include "../../../tangible_exception_helper.h"

using namespace ::YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Legality
{

	ClusterIssueIntentEntry::ClusterIssueIntentEntry(DecodedBundleSlotDescriptor slotDescriptor) : ClusterIssueIntentEntry(slotDescriptor.getSlotIndex(), slotDescriptor.getVirtualThreadId(), slotDescriptor.getOwnerThreadId(), slotDescriptor.getOpCode(), slotDescriptor.getMicroOp())
	{
	}

	ClusterIssueIntentEntry::ClusterIssueIntentEntry(unsigned char slotIndex, int virtualThreadId, int ownerThreadId, unsigned int opCode, std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> microOp)
		: MicroOp(microOp), SlotIndex(slotIndex), VirtualThreadId(virtualThreadId), OwnerThreadId(ownerThreadId), OpCode(opCode)
	{
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::MicroOp> ClusterIssueIntentEntry::getMicroOp() const
	{
		return MicroOp;
	}

	YAKSys_Hybrid_CPU::Core::DecodedBundleSlotDescriptor ClusterIssueIntentEntry::getSlotDescriptor() const
	{
		DecodedBundleSlotDescriptor slotDescriptor = DecodedBundleSlotDescriptor::Create(getSlotIndex(), getMicroOp());
		return DecodedBundleSlotDescriptor(slotDescriptor.getMicroOp(), slotDescriptor.getSlotIndex(), getVirtualThreadId(), getOwnerThreadId(), getOpCode(), slotDescriptor.getReadRegisters(), slotDescriptor.getWriteRegisters(), slotDescriptor.getWritesRegister(), slotDescriptor.isMemoryOp(), slotDescriptor.isControlFlow(), slotDescriptor.getPlacement(), slotDescriptor.getMemoryBankIntent(), slotDescriptor.isFspInjected(), slotDescriptor.isEmptyOrNop(), slotDescriptor.isVectorOp());
	}

	unsigned char ClusterIssueIntentEntry::getSlotIndex() const
	{
		return SlotIndex;
	}

	int ClusterIssueIntentEntry::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	int ClusterIssueIntentEntry::getOwnerThreadId() const
	{
		return OwnerThreadId;
	}

	unsigned int ClusterIssueIntentEntry::getOpCode() const
	{
		return OpCode;
	}

	ClusterIssueIntentEntry ClusterIssueIntentEntry::FromScalarClusterIssueEntry(ScalarClusterIssueEntry entry)
	{
		return ClusterIssueIntentEntry(entry.getSlotIndex(), entry.getVirtualThreadId(), entry.getOwnerThreadId(), entry.getOpCode(), entry.getMicroOp());
	}

	YAKSys_Hybrid_CPU::Core::ScalarClusterIssueEntry ClusterIssueIntentEntry::ToScalarClusterIssueEntry() const
	{
		return ScalarClusterIssueEntry(getSlotIndex(), getVirtualThreadId(), getOwnerThreadId(), getOpCode(), getMicroOp());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public ClusterIssueIntent(byte candidateMask, byte preparedMask, IReadOnlyList<ClusterIssueIntentEntry>? entries = null)
	ClusterIssueIntent::ClusterIssueIntent(unsigned char candidateMask, unsigned char preparedMask, const std::vector<ClusterIssueIntentEntry> &entries)
		: CandidateMask([&]() {
			ValidateSlotMask(L"candidateMask", candidateMask);
			ValidateSlotMask(L"preparedMask", preparedMask);
			if ((preparedMask & ~candidateMask) != 0)
			{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Prepared mask must be a subset of candidate mask.", nameof(preparedMask));
				throw std::invalid_argument("Prepared mask must be a subset of candidate mask.");
			}
			return candidateMask;
		}()),
		PreparedMask(preparedMask),
		_entries(NormalizeEntries(entries, preparedMask))
	{
	}

	unsigned char ClusterIssueIntent::getCandidateMask() const
	{
		return CandidateMask;
	}

	unsigned char ClusterIssueIntent::getPreparedMask() const
	{
		return PreparedMask;
	}

	const std::vector<ClusterIssueIntentEntry> ClusterIssueIntent::getPreparedEntries() const
	{
		return _entries;
	}

	const std::vector<ClusterIssueIntentEntry> ClusterIssueIntent::getEntries() const
	{
		return getPreparedEntries();
	}

	int ClusterIssueIntent::getCount() const
	{
		return _entries.size();
	}

	YAKSys_Hybrid_CPU::Core::VtScalarCandidateSummary ClusterIssueIntent::BuildVtSummary()
	{
		int vt0 = 0;
		int vt1 = 0;
		int vt2 = 0;
		int vt3 = 0;

		for (int i = 0; i < _entries.size(); i++)
		{
			switch (_entries[i].getVirtualThreadId())
			{
				case 0:
					vt0++;
					break;
				case 1:
					vt1++;
					break;
				case 2:
					vt2++;
					break;
				case 3:
					vt3++;
					break;
			}
		}

		return VtScalarCandidateSummary(vt0, vt1, vt2, vt3);
	}

	std::shared_ptr<ClusterIssueIntent> ClusterIssueIntent::CreateEmpty()
	{
		return std::make_shared<ClusterIssueIntent>(0, 0);
	}

	std::shared_ptr<ClusterIssueIntent> ClusterIssueIntent::FromScalarClusterIssueGroup(std::shared_ptr<ScalarClusterIssueGroup> scalarClusterGroup)
	{
		ArgumentNullException::ThrowIfNull(scalarClusterGroup);

		if (scalarClusterGroup->getCount() == 0)
		{
			return std::make_shared<ClusterIssueIntent>(scalarClusterGroup->getCandidateMask(), scalarClusterGroup->getPreparedMask());
		}

		const auto scalarEntries = scalarClusterGroup->getEntries();
		std::vector<ClusterIssueIntentEntry> entries;
		entries.reserve(scalarEntries.size());
		for (const auto &entry : scalarEntries)
		{
			entries.push_back(ClusterIssueIntentEntry::FromScalarClusterIssueEntry(entry));
		}

		return std::make_shared<ClusterIssueIntent>(scalarClusterGroup->getCandidateMask(), scalarClusterGroup->getPreparedMask(), entries);
	}

	std::shared_ptr<ScalarClusterIssueGroup> ClusterIssueIntent::ToScalarClusterIssueGroup()
	{
		if (_entries.empty())
		{
			return std::make_shared<ScalarClusterIssueGroup>(getCandidateMask(), getPreparedMask(), std::vector<ScalarClusterIssueEntry>());
		}

		std::vector<ScalarClusterIssueEntry> entries;
		entries.reserve(_entries.size());
		for (const auto &entry : _entries)
		{
			entries.push_back(entry.ToScalarClusterIssueEntry());
		}

		return std::make_shared<ScalarClusterIssueGroup>(getCandidateMask(), getPreparedMask(), entries);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ClusterIssueIntentEntry[] NormalizeEntries(IReadOnlyList<ClusterIssueIntentEntry>? entries, byte preparedMask)
	std::vector<ClusterIssueIntentEntry> ClusterIssueIntent::NormalizeEntries(const std::vector<ClusterIssueIntentEntry> &entries, unsigned char preparedMask)
	{
		if (entries.empty() || entries.empty())
		{
			if (preparedMask != 0)
			{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Prepared mask requires matching prepared entries.", nameof(entries));
				throw std::invalid_argument("Prepared mask requires matching prepared entries.");
			}

			return std::vector<ClusterIssueIntentEntry>();
		}

		std::vector<ClusterIssueIntentEntry> normalized;
		normalized.reserve(entries.size());
		unsigned char entryMask = 0;

		for (int i = 0; i < entries.size(); i++)
		{
			ClusterIssueIntentEntry entry = entries[i];
			ValidateSlotIndex(entry.getSlotIndex());

			unsigned char slotBit = static_cast<unsigned char>(1 << entry.getSlotIndex());
			if ((preparedMask & slotBit) == 0)
			{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Prepared entries contain slot {0}, but it is not set in the prepared mask.", entry.SlotIndex), nameof(entries));
				throw std::invalid_argument(std::format("Prepared entries contain slot {}, but it is not set in the prepared mask.", static_cast<unsigned int>(entry.getSlotIndex())));
			}

			if ((entryMask & slotBit) != 0)
			{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Prepared entries contain duplicate slot {0}.", entry.SlotIndex), nameof(entries));
				throw std::invalid_argument(std::format("Prepared entries contain duplicate slot {}.", static_cast<unsigned int>(entry.getSlotIndex())));
			}

			normalized.push_back(entry);
			entryMask |= slotBit;
		}

		if (entryMask != preparedMask)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Prepared entries must exactly match the prepared mask.", nameof(entries));
			throw std::invalid_argument("Prepared entries must exactly match the prepared mask.");
		}

		return normalized;
	}

	void ClusterIssueIntent::ValidateSlotIndex(int slotIndex)
	{
		if (static_cast<unsigned int>(slotIndex) >= BundleMetadata::BundleSlotCount)
		{
			throw ArgumentOutOfRangeException(std::format("slotIndex: Slot index must be in [0, {}].", BundleMetadata::BundleSlotCount - 1));
		}
	}

	void ClusterIssueIntent::ValidateSlotMask(const std::wstring &paramName, unsigned char slotMask)
	{
		if ((slotMask & ~AllSlotMask) != 0)
		{
			throw ArgumentOutOfRangeException(std::format("{}: Slot mask must not set bits outside bundle width {}.", std::string(paramName.begin(), paramName.end()), BundleMetadata::BundleSlotCount));
		}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static ClusterIssueIntent Build(IReadOnlyList<DecodedBundleSlotDescriptor> slots, byte candidateMask)
	std::shared_ptr<ClusterIssueIntent> ClusterIssueIntentBuilder::Build(const std::vector<DecodedBundleSlotDescriptor> &slots, unsigned char candidateMask)
	{
		ArgumentNullException::ThrowIfNull(slots);

		if (slots.size() < BundleMetadata::BundleSlotCount)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException(string.Format("Slot list must contain at least {0} entries.", BundleMetadata.BundleSlotCount), nameof(slots));
			throw std::invalid_argument(std::format("Slot list must contain at least {} entries.", BundleMetadata::BundleSlotCount));
		}

		std::vector<ClusterIssueIntentEntry> entries;
		entries.reserve(ScalarIssueWidthCeiling);
		unsigned char preparedMask = 0;
		int count = 0;
		unsigned char selectedVirtualThreadMask = 0;

		for (int slotIndex = 0; slotIndex < BundleMetadata::BundleSlotCount && count < ScalarIssueWidthCeiling; slotIndex++)
		{
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if ((candidateMask & slotBit) == 0)
			{
				continue;
			}

			unsigned char virtualThreadBit;
			if (!TryGetVirtualThreadBit(slots[slotIndex].getVirtualThreadId(), virtualThreadBit) || (selectedVirtualThreadMask & virtualThreadBit) != 0)
			{
				continue;
			}

			entries.push_back(ClusterIssueIntentEntry(slots[slotIndex]));
			preparedMask |= slotBit;
			selectedVirtualThreadMask |= virtualThreadBit;
			count++;
		}

		for (int slotIndex = 0; slotIndex < BundleMetadata::BundleSlotCount && count < ScalarIssueWidthCeiling; slotIndex++)
		{
			unsigned char slotBit = static_cast<unsigned char>(1 << slotIndex);
			if ((candidateMask & slotBit) == 0 || (preparedMask & slotBit) != 0)
			{
				continue;
			}

			entries.push_back(ClusterIssueIntentEntry(slots[slotIndex]));
			preparedMask |= slotBit;
			count++;
		}

		if (count == 0)
		{
			return std::make_shared<ClusterIssueIntent>(candidateMask, preparedMask);
		}

		return std::make_shared<ClusterIssueIntent>(candidateMask, preparedMask, entries);
	}

	bool ClusterIssueIntentBuilder::TryGetVirtualThreadBit(int virtualThreadId, unsigned char &virtualThreadBit)
	{
		switch (virtualThreadId)
		{
			case 0:
				virtualThreadBit = 1 << 0;
				return true;
			case 1:
				virtualThreadBit = 1 << 1;
				return true;
			case 2:
				virtualThreadBit = 1 << 2;
				return true;
			case 3:
				virtualThreadBit = 1 << 3;
				return true;
			default:
				virtualThreadBit = 0;
				return false;
		}
	}
}
