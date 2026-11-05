#include "ShadowSemanticProjection.h"
#include "../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../NonRTL/Arch/SerializationClass.h"
#include "../../Decoder/ExecutionBindingIdentity.h"
#include "../../Pipeline/Metadata/BundleMetadata.h"
#include "../../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include <codecvt>
#include <locale>
#include <sstream>
#include <span>
#include <mutex>
#include <unordered_map>

using namespace HybridCpu::ShadowFrontend;
using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;

namespace
{
	struct BundleMetadataSidecar final
	{
		std::weak_ptr<const Snapshot> Owner;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> Metadata;
	};

	std::mutex &BundleMetadataSidecarGate()
	{
		// The registry may be consulted by Snapshot custom deleters during static
		// teardown across translation units. Keep its synchronization object alive
		// until process exit; individual entries are erased when their Snapshot dies.
		static auto *gate = new std::mutex();
		return *gate;
	}

	std::unordered_map<const Snapshot *, BundleMetadataSidecar> &BundleMetadataSidecars()
	{
		static auto *sidecars = new std::unordered_map<const Snapshot *, BundleMetadataSidecar>();
		return *sidecars;
	}

	void ForgetBundleMetadataSidecar(const Snapshot *snapshot) noexcept
	{
		std::lock_guard lock(BundleMetadataSidecarGate());
		BundleMetadataSidecars().erase(snapshot);
	}

	void RememberBundleMetadataSidecar(
		const std::shared_ptr<Snapshot> &snapshot,
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> metadata)
	{
		if (!snapshot || !metadata) return;
		std::lock_guard lock(BundleMetadataSidecarGate());
		auto &sidecars = BundleMetadataSidecars();
		for (auto it = sidecars.begin(); it != sidecars.end();)
		{
			if (it->second.Owner.expired()) it = sidecars.erase(it);
			else ++it;
		}
		sidecars[snapshot.get()] = BundleMetadataSidecar{snapshot, std::move(metadata)};
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::BundleMetadata> FindBundleMetadataSidecar(
		const std::shared_ptr<const Snapshot> &snapshot)
	{
		if (!snapshot) return nullptr;
		std::lock_guard lock(BundleMetadataSidecarGate());
		auto &sidecars = BundleMetadataSidecars();
		auto found = sidecars.find(snapshot.get());
		if (found == sidecars.end()) return nullptr;
		const auto owner = found->second.Owner.lock();
		if (!owner || owner.get() != snapshot.get())
		{
			sidecars.erase(found);
			return nullptr;
		}
		return found->second.Metadata;
	}

	std::shared_ptr<HybridCpu::ShadowFrontend::Snapshot> MakeSnapshot(
		const std::wstring &kind, const std::vector<unsigned char> &content)
	{
		const auto bytes = std::span<const unsigned char>(content.data(), content.size());
		return std::shared_ptr<HybridCpu::ShadowFrontend::Snapshot>(
			new HybridCpu::ShadowFrontend::Snapshot(
				kind, std::make_shared<HybridCpu::ShadowFrontend::FrozenBytes>(bytes)),
			[](HybridCpu::ShadowFrontend::Snapshot *snapshot)
			{
				ForgetBundleMetadataSidecar(snapshot);
				delete snapshot;
			});
	}

	void AppendJsonString(std::ostringstream &json, const std::wstring &value)
	{
		const std::string utf8 = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(value);
		json << '"';
		for (unsigned char ch : utf8)
		{
			switch (ch)
			{
			case '"': json << "\\\""; break;
			case '\\': json << "\\\\"; break;
			case '\b': json << "\\b"; break;
			case '\f': json << "\\f"; break;
			case '\n': json << "\\n"; break;
			case '\r': json << "\\r"; break;
			case '\t': json << "\\t"; break;
			default:
				if (ch < 0x20) json << "\\u00" << "0123456789abcdef"[ch >> 4] << "0123456789abcdef"[ch & 0x0f];
				else json << static_cast<char>(ch);
			}
		}
		json << '"';
	}

	void AppendBindingJson(std::ostringstream &json,
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::GeneratedStaticBinding> &binding)
	{
		if (!binding)
		{
			json << "null";
			return;
		}
		json << "{\"Opcode\":" << binding->Opcode << ",\"MaterializerId\":{\"Value\":";
		AppendJsonString(json, binding->MaterializerId.Value);
		json << "},\"RuntimeExecutionProviderId\":{\"Value\":";
		AppendJsonString(json, binding->RuntimeExecutionProviderId.Value);
		json << "},\"LatencyModelId\":{\"Value\":";
		AppendJsonString(json, binding->LatencyModelId.Value);
		json << "},\"CatalogVersion\":";
		AppendJsonString(json, binding->CatalogVersion);
		json << ",\"CatalogSha256\":";
		AppendJsonString(json, binding->CatalogSha256);
		json << ",\"DescriptorFingerprint\":";
		AppendJsonString(json, binding->DescriptorFingerprint);
		json << '}';
	}

	std::vector<unsigned char> Utf8Bytes(const std::string &text)
	{
		return {text.begin(), text.end()};
	}

	std::wstring Base64(const std::vector<unsigned char> &bytes)
	{
		static constexpr wchar_t alphabet[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		std::wstring result;
		result.reserve(((bytes.size() + 2) / 3) * 4);
		for (std::size_t i = 0; i < bytes.size(); i += 3)
		{
			const unsigned int a = bytes[i];
			const unsigned int b = i + 1 < bytes.size() ? bytes[i + 1] : 0;
			const unsigned int c = i + 2 < bytes.size() ? bytes[i + 2] : 0;
			const unsigned int triple = (a << 16) | (b << 8) | c;
			result.push_back(alphabet[(triple >> 18) & 0x3f]);
			result.push_back(alphabet[(triple >> 12) & 0x3f]);
			result.push_back(i + 1 < bytes.size() ? alphabet[(triple >> 6) & 0x3f] : L'=');
			result.push_back(i + 2 < bytes.size() ? alphabet[triple & 0x3f] : L'=');
		}
		return result;
	}

	bool SameSnapshot(const std::shared_ptr<const HybridCpu::ShadowFrontend::Snapshot> &left,
		const std::shared_ptr<const HybridCpu::ShadowFrontend::Snapshot> &right)
	{
		return (!left && !right) || (left && right && *left == *right);
	}
}

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<Snapshot> ShadowSemanticProjection::Snap(std::shared_ptr<CanonicalPayloadSnapshot> s)
	{
		if (!s) return nullptr;
		auto snapshot = MakeSnapshot(s->getKind(), s->getContent());
		if (snapshot->Kind == L"BundleMetadata")
		{
			const auto metadata = s->Deserialize<std::shared_ptr<BundleMetadata>>();
			if (metadata && *metadata) RememberBundleMetadataSidecar(snapshot, *metadata);
		}
		return snapshot;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static Snapshot Object(string kind, Object? value)
	std::shared_ptr<Snapshot> ShadowSemanticProjection::Object(const std::wstring &kind, std::any value)
	{
		std::ostringstream json;
		if (kind == L"GeneratedStaticBinding")
		{
			const auto *binding = std::any_cast<std::shared_ptr<GeneratedStaticBinding>>(&value);
			AppendBindingJson(json, binding ? *binding : nullptr);
		}
		else if (kind == L"CanonicalScalarLoadAddressPlan")
		{
			const auto *plan = std::any_cast<std::shared_ptr<CanonicalScalarLoadAddressPlan>>(&value);
			if (!plan || !*plan)
			{
				json << "null";
			}
			else
			{
				json << "{\"GeneratedBinding\":";
				AppendBindingJson(json, (*plan)->getGeneratedBinding());
				json << ",\"DestinationRegisterId\":" << static_cast<unsigned int>((*plan)->getDestinationRegisterId())
					 << ",\"BaseRegisterId\":" << static_cast<unsigned int>((*plan)->getBaseRegisterId())
					 << ",\"SignedDisplacement\":" << (*plan)->getSignedDisplacement()
					 << ",\"AccessSize\":" << static_cast<unsigned int>((*plan)->getAccessSize()) << '}';
			}
		}
		else
		{
			const auto snapshot = CanonicalPayloadSnapshot::FromObject(kind, std::move(value));
			auto projected = MakeSnapshot(kind, snapshot->getContent());
			if (kind == L"BundleMetadata")
			{
				const auto metadata = snapshot->Deserialize<std::shared_ptr<BundleMetadata>>();
				if (metadata && *metadata) RememberBundleMetadataSidecar(projected, *metadata);
			}
			return projected;
		}
		return MakeSnapshot(kind, Utf8Bytes(json.str()));
	}

	std::shared_ptr<CanonicalPayloadSnapshot> ShadowSemanticProjection::Native(std::shared_ptr<const Snapshot> s)
	{
		if (!s || !s->Content) throw std::invalid_argument("snapshot content must not be null");
		const auto bytes = s->Content->Copy();
		const auto content = std::span<const unsigned char>(bytes.data(), bytes.size());
		if (s->Kind == L"BundleMetadata")
		{
			if (auto metadata = FindBundleMetadataSidecar(s))
				return CanonicalPayloadSnapshot::FromBytes(s->Kind, content, std::any(std::move(metadata)));
		}
		return CanonicalPayloadSnapshot::FromBytes(s->Kind, content);
	}

	std::shared_ptr<DecodeResult> ShadowSemanticProjection::Project(std::shared_ptr<CanonicalBundle> bundle)
	{
		if (!bundle) throw std::invalid_argument("bundle must not be null");
		const auto rawBytes = bundle->getSemanticKey().getRawBundleBytes();
		auto raw = std::make_shared<FrozenBytes>(
			std::span<const unsigned char>(rawBytes.data(), rawBytes.size()));
		const auto canonicalSlots = bundle->getSlots();
		std::vector<std::shared_ptr<SemanticSlot>> slots;
		slots.reserve(canonicalSlots.size());
		for (const auto &slot : canonicalSlots)
		{
			if (!slot) throw std::invalid_argument("canonical bundle contains a null slot");
			std::optional<int> instructionClass;
			if (slot->InstructionClass) instructionClass = static_cast<int>(*slot->InstructionClass);
			std::optional<int> serializationClass;
			if (slot->SerializationClass) serializationClass = static_cast<int>(*slot->SerializationClass);
			slots.push_back(std::make_shared<SemanticSlot>(SemanticSlot{
				slot->SlotIndex, slot->IsOccupied, slot->Opcode, instructionClass, serializationClass,
				slot->Rd, slot->Rs1, slot->Rs2, slot->Immediate, slot->CsrAddress,
				slot->AcquireOrdering, slot->ReleaseOrdering, Snap(slot->RawSlot),
				Snap(slot->InstructionPayload), Snap(slot->SlotSideband),
				Object(L"GeneratedStaticBinding", std::any(slot->StaticBinding)),
				Object(L"CanonicalScalarLoadAddressPlan", std::any(slot->ScalarLoadAddressPlan))}));
		}
		return DecodeResult::Success(std::move(raw), slots, Snap(bundle->getBundleSideband()));
	}

	std::shared_ptr<DecodeResult> ShadowSemanticProjection::Illegal(std::shared_ptr<FrozenBytes> raw, std::shared_ptr<DecodeFailure> failure)
	{
		if (!failure) throw std::invalid_argument("failure must not be null");
		return DecodeResult::Illegal(raw, std::make_shared<TypedDecodeFailure>(TypedDecodeFailure{
			static_cast<unsigned short>(failure->Code), failure->SlotIndex, failure->Field,
			failure->RawHash, failure->Message}));
	}

	std::wstring ShadowSemanticProjection::SidebandIdentity(std::shared_ptr<VliwBundleAnnotations> annotations)
	{
		if (!annotations) throw std::invalid_argument("annotations must not be null");
		// Exact normalized content, not hash equality or a caller's string witness.
		std::wstring identity = Base64(Object(L"BundleMetadata", std::any(annotations->getBundleMetadata()))->Content->Copy());
		for (int i = 0; i < 8; i++)
		{
			std::shared_ptr<InstructionSlotMetadata> m;
			std::shared_ptr<InstructionSlotMetadata> metadata = annotations->TryGetInstructionSlotMetadata(i, m) ? m : InstructionSlotMetadata::getDefault();
			identity.push_back(L'.');
			identity += Base64(Object(L"InstructionSlotMetadata", std::any(metadata))->Content->Copy());
		}
		return identity;
	}

	std::shared_ptr<CanonicalBundle> ShadowSemanticProjection::Restore(std::shared_ptr<DecodeResult> facts, std::shared_ptr<BundleRequest> placement, std::shared_ptr<CanonicalDecodeContext> context)
	{
		if (!facts || !placement || !context)
			throw std::invalid_argument("facts, placement and context must not be null");
		if (!facts->isSuccess())
		{
			throw InvalidOperationException(L"Only successful static facts can be materialized.");
		}
		std::vector<VLIW_Instruction> raw = ReadRaw(facts->getRaw());
		std::vector<std::shared_ptr<CanonicalDecodedInstruction>> slots(8);
		for (int i = 0; i < slots.size(); i++)
		{
			std::shared_ptr<SemanticSlot> s = facts->getSlots()[i];
			std::optional<std::shared_ptr<GeneratedStaticBinding>> binding;
			if (s->IsOccupied)
			{
				std::shared_ptr<GeneratedStaticBinding> current;
				if (!GeneratedStaticBinding::TryFromOpcode(s->Opcode, current) || !SameSnapshot(Object(L"GeneratedStaticBinding", std::any(current)), s->StaticBinding))
				{
					throw InvalidOperationException(L"Shadow static binding differs from current generated catalog.");
				}
				// Use the current generated owner; JSON never creates a binding.
				binding = current;
			}
			else if (!SameSnapshot(Object(L"GeneratedStaticBinding", std::any()), s->StaticBinding))
			{
				throw InvalidOperationException(L"Empty Shadow slot contains a static binding.");
			}
			std::optional<Arch::InstructionClass> instructionClass;
			if (s->InstructionClass) instructionClass = static_cast<Arch::InstructionClass>(*s->InstructionClass);
			std::optional<Arch::SerializationClass> serializationClass;
			if (s->SerializationClass) serializationClass = static_cast<Arch::SerializationClass>(*s->SerializationClass);
			std::shared_ptr<CanonicalDecodedInstruction> slot = std::make_shared<CanonicalDecodedInstruction>(
				s->SlotIndex, s->IsOccupied, s->Opcode, instructionClass, serializationClass,
				s->Rd, s->Rs1, s->Rs2, s->Immediate, s->CsrAddress, s->AcquireOrdering,
				s->ReleaseOrdering, Native(s->RawSlot), Native(s->InstructionPayload), Native(s->SlotSideband));
			slot->StaticBinding = binding ? *binding : nullptr;
			std::shared_ptr<CanonicalScalarLoadAddressPlan> plan;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (CanonicalScalarLoadAddressPlan.TryCreate(slot, out CanonicalScalarLoadAddressPlan? plan))
			if (CanonicalScalarLoadAddressPlan::TryCreate(slot, plan))
			{
				slot->ScalarLoadAddressPlan = plan;
			}
			if (!SameSnapshot(Object(L"CanonicalScalarLoadAddressPlan", std::any(slot->ScalarLoadAddressPlan)), s->ScalarLoadAddressPlan))
			{
				throw InvalidOperationException(L"Shadow load plan differs from the canonical derived plan.");
			}
			slots[i] = slot;
		}
		const auto bundleSideband = facts->getBundleSideband();
		if (!bundleSideband || !*bundleSideband)
			throw InvalidOperationException(L"Shadow bundle metadata missing.");
		const auto metadataValue = Native(*bundleSideband)->Deserialize<std::shared_ptr<BundleMetadata>>();
		if (!metadataValue || !*metadataValue)
			throw InvalidOperationException(L"Shadow bundle metadata missing.");
		std::shared_ptr<BundleMetadata> metadata = *metadataValue;
		std::shared_ptr<CanonicalBundle> restored = CanonicalBundle::CreateFromCanonicalSlots(raw, slots, metadata, placement->Pc, placement->TransportSerial, context);
		if (!Project(restored)->SameFacts(facts))
		{
			throw InvalidOperationException(L"Shadow canonical restore lost static facts.");
		}
		return restored;
	}

	std::vector<VLIW_Instruction> ShadowSemanticProjection::ReadRaw(std::shared_ptr<FrozenBytes> bytes)
	{
		if (!bytes) throw std::invalid_argument("bytes must not be null");
		std::vector<unsigned char> raw = bytes->Copy();
		if (raw.size() != 256)
		{
			throw InvalidOperationException(L"Shadow requires a full raw carrier.");
		}
		std::vector<VLIW_Instruction> slots(8);
		const auto readWord = [&raw](std::size_t offset)
		{
			unsigned long long value = 0;
			for (std::size_t byte = 0; byte < 8; ++byte)
				value |= static_cast<unsigned long long>(raw[offset + byte]) << (byte * 8);
			return value;
		};
		// Raw staging deliberately performs no opcode validation or decode.
		for (int i = 0; i < 8; i++)
		{
			const std::size_t offset = static_cast<std::size_t>(i) * 32;
			VLIW_Instruction instruction{};
			instruction.setWord0(readWord(offset));
			instruction.setWord1(readWord(offset + 8));
			instruction.setWord2(readWord(offset + 16));
			instruction.setWord3(readWord(offset + 24));
			slots[i] = instruction;
		}
		return slots;
	}
}
