#include "CompactCodec.h"

#include "BrotliCompat.h"
#include "../../CloseToHSL/Core/Architecture/Common/BinaryIOCompat.h"
#include <algorithm>
#include <limits>
using System::IO::BinaryReader;
using System::IO::BinaryWriter;
using System::IO::MemoryStream;
namespace HybridCpu::ShadowFrontend
{
	namespace
	{
		long long TextSize(const std::wstring &value) { return 4LL + static_cast<long long>(System::IO::ToUtf8(value).size()); }
		long long SnapshotSize(const std::shared_ptr<const Snapshot> &value)
		{
			if (!value || !value->Content) throw std::invalid_argument("Incomplete snapshot in decode facts.");
			return TextSize(value->Kind) + 4LL + value->Content->getLength();
		}
		long long BodySize(const std::shared_ptr<DecodeResult> &value)
		{
			if (!value || !value->getRaw()) throw std::invalid_argument("Missing raw decode bytes.");
			long long size = 1LL + 4 + value->getRaw()->getLength();
			const auto failure = value->getFailure();
			if (failure && *failure)
				return size + 6 + TextSize((*failure)->Field) + TextSize((*failure)->RawHash) + TextSize((*failure)->Message);
			const auto sideband = value->getBundleSideband();
			if (!sideband || !*sideband) throw std::invalid_argument("Successful decode has no bundle sideband.");
			size += SnapshotSize(*sideband) + 1;
			for (const auto &slot : value->getSlots())
			{
				if (!slot) throw std::invalid_argument("Decode contains a null slot.");
				size += 21 + (slot->InstructionClass ? 4 : 0) + (slot->SerializationClass ? 4 : 0) + (slot->CsrAddress ? 4 : 0) +
					SnapshotSize(slot->RawSlot) + SnapshotSize(slot->InstructionPayload) + SnapshotSize(slot->SlotSideband) + SnapshotSize(slot->StaticBinding) + SnapshotSize(slot->ScalarLoadAddressPlan);
			}
			return size;
		}
		bool Boolean(const std::shared_ptr<BinaryReader> &reader)
		{
			const auto value = reader->ReadByte();
			if (value > 1) throw std::runtime_error("Invalid codec boolean.");
			return value != 0;
		}
		template<typename T> void Optional(const std::shared_ptr<BinaryWriter> &writer, const std::optional<T> &value)
		{
			writer->Write(value.has_value());
			if (value) writer->Write(static_cast<int>(*value));
		}
		std::optional<int> ReadOptional(const std::shared_ptr<BinaryReader> &reader) { return Boolean(reader) ? std::optional<int>(reader->ReadInt32()) : std::nullopt; }
		void Blob(const std::shared_ptr<BinaryWriter> &writer, const std::shared_ptr<const FrozenBytes> &bytes)
		{
			if (!bytes) throw std::invalid_argument("Null codec blob.");
			writer->Write(bytes->getLength()); writer->Write(bytes->getSpan());
		}
		std::shared_ptr<FrozenBytes> ReadBlob(const std::shared_ptr<BinaryReader> &reader)
		{
			const int length = reader->ReadInt32();
			if (length < 0 || length > CompactCodec::MaxDecodedBytes || static_cast<long long>(length) > reader->getRemaining()) throw std::runtime_error("Invalid codec blob length.");
			auto bytes = reader->ReadBytesExact(length, CompactCodec::MaxDecodedBytes);
			return std::make_shared<FrozenBytes>(std::span<const unsigned char>(bytes));
		}
		void Text(const std::shared_ptr<BinaryWriter> &writer, const std::wstring &text)
		{
			const std::string utf8 = System::IO::ToUtf8(text);
			if (utf8.size() > static_cast<size_t>(CompactCodec::MaxDecodedBytes)) throw std::length_error("Codec text exceeds bound.");
			std::vector<unsigned char> bytes(utf8.begin(), utf8.end());
			Blob(writer, std::make_shared<FrozenBytes>(std::span<const unsigned char>(bytes)));
		}
		std::wstring ReadText(const std::shared_ptr<BinaryReader> &reader) { return System::IO::FromUtf8(ReadBlob(reader)->getSpan()); }
		void Snap(const std::shared_ptr<BinaryWriter> &writer, const std::shared_ptr<const Snapshot> &snapshot)
		{
			if (!snapshot) throw std::invalid_argument("Null codec snapshot.");
			Text(writer, snapshot->Kind); Blob(writer, snapshot->Content);
		}
		std::shared_ptr<Snapshot> ReadSnap(const std::shared_ptr<BinaryReader> &reader) { return std::make_shared<Snapshot>(ReadText(reader), ReadBlob(reader)); }
	}

	std::optional<std::vector<unsigned char>> CompactCodec::Encode(std::shared_ptr<DecodeResult> result)
	{
		if (!result) return std::nullopt;
		const auto failure = result->getFailure();
		if (result->getSchema() != 1 || (failure && *failure && (*failure)->Code > 8))
		{
			return std::nullopt;
		}
		// Bound BEFORE allocating the encoding buffer, including opaque extensions.
		try
		{
			if (BodySize(result) > MaxDecodedBytes)
			{
				return std::nullopt;
			}
		}
		catch (const std::runtime_error &)
		{
			return std::nullopt;
		} // Expected unsupported text contour, not a programming exception.
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using MemoryStream body = new MemoryStream();
		{
			auto body = std::make_shared<MemoryStream>();
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (BinaryWriter w = new BinaryWriter(body, Encoding.UTF8, true))
			{
				auto w = std::make_shared<BinaryWriter>(body);
				w->Write(result->isSuccess());
				Blob(w, result->getRaw());
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
				if (failure && *failure)
				{
					w->Write((*failure)->Code);
					w->Write((*failure)->SlotIndex);
					Text(w, (*failure)->Field);
					Text(w, (*failure)->RawHash);
					Text(w, (*failure)->Message);
				}
				else
				{
		// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
		// ORIGINAL LINE: Snap(w, result.BundleSideband!);
					const auto sideband = result->getBundleSideband();
					if (!sideband || !*sideband) return std::nullopt;
					Snap(w, *sideband);
					unsigned char mask = 0;
					for (auto s : result->getSlots())
					{
						if (s->IsOccupied)
						{
							mask |= static_cast<unsigned char>(1 << s->SlotIndex);
						}
					}
					w->Write(mask);
					for (auto s : result->getSlots())
					{
						w->Write(static_cast<unsigned char>(s->SlotIndex));
						w->Write(s->Opcode);
						Optional(w, s->InstructionClass);
						Optional(w, s->SerializationClass);
						w->Write(s->Rd);
						w->Write(s->Rs1);
						w->Write(s->Rs2);
						w->Write(s->Immediate);
						Optional(w, s->CsrAddress);
						w->Write(s->AcquireOrdering);
						w->Write(s->ReleaseOrdering);
						Snap(w, s->RawSlot);
						Snap(w, s->InstructionPayload);
						Snap(w, s->SlotSideband);
						Snap(w, s->StaticBinding);
						Snap(w, s->ScalarLoadAddressPlan);
					}
				}
			}
			if (body->getLength() > MaxDecodedBytes)
			{
				return std::nullopt;
			}
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using MemoryStream output = new MemoryStream();
			{
				auto output = std::make_shared<MemoryStream>();
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (BinaryWriter h = new BinaryWriter(output, Encoding.UTF8, true))
				{
					auto h = std::make_shared<BinaryWriter>(output);
					h->Write(Magic);
					h->Write(static_cast<unsigned short>(1));
					h->Write(static_cast<unsigned short>(0));
					h->Write(static_cast<int>(body->getLength()));
				}
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (BrotliStream compressor = new BrotliStream(output, CompressionLevel.SmallestSize, true))
				auto bodyBytes = body->ToArray();
				auto compressed = BrotliCompat::CompressSmallest(std::span<const unsigned char>(bodyBytes));
				if (!compressed) return std::nullopt;
				auto compressedWriter = std::make_shared<BinaryWriter>(output);
				compressedWriter->Write(*compressed);
				if (output->getLength() > MaxEncodedBytes) return std::nullopt;
				return output->ToArray();
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryDecode(ReadOnlySpan<byte> encoded, out DecodeResult? result)
	bool CompactCodec::TryDecode(std::span<const unsigned char> encoded, std::shared_ptr<DecodeResult> &result)
	{
		result = nullptr;
		if (encoded.size() < 12 || encoded.size() > static_cast<size_t>(MaxEncodedBytes))
		{
			return false;
		}
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using MemoryStream input = new MemoryStream(encoded.ToArray(), false);
		{
			auto input = std::make_shared<MemoryStream>(std::vector<unsigned char>(encoded.begin(), encoded.end()));
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using BinaryReader reader = new BinaryReader(input);
			{
				auto reader = std::make_shared<BinaryReader>(input);
				if (reader->ReadUInt32() != Magic || reader->ReadUInt16() != 1 || reader->ReadUInt16() != 0)
				{
					return false;
				}
				int size = reader->ReadInt32();
				if (size < 261 || size > MaxDecodedBytes)
				{
					return false;
				}
				// BrotliDecoder exposes exact consumed/written counts: reject trailing streams,
				// truncation and expansion bombs without unbounded allocation.
				std::vector<unsigned char> body(size);
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using BrotliDecoder decoder = new BrotliDecoder();
				{
					size_t used = 0, written = 0;
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# range operator:
					if (!BrotliCompat::DecompressExact(encoded.subspan(12), std::span<unsigned char>(body), used, written) || used != encoded.size() - 12 || written != static_cast<size_t>(size))
					{
						return false;
					}
					try
					{
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using MemoryStream stream = new MemoryStream(body, false);
						{
							auto stream = std::make_shared<MemoryStream>(std::move(body));
// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using BinaryReader r = new BinaryReader(stream, new UTF8Encoding(false, true));
							{
								auto r = std::make_shared<BinaryReader>(stream, true);
								bool success = Boolean(r);
								std::shared_ptr<FrozenBytes> raw = ReadBlob(r);
								if (raw->getLength() != 256)
								{
									return false;
								}
								if (!success)
								{
									unsigned short code = r->ReadUInt16();
									int slot = r->ReadInt32();
									if (code > 8 || slot < -1 || slot > 7)
									{
										return false;
									}
									result = DecodeResult::Illegal(raw, std::make_shared<TypedDecodeFailure>(TypedDecodeFailure{code, slot, ReadText(r), ReadText(r), ReadText(r)}));
								}
								else
								{
									std::shared_ptr<Snapshot> sideband = ReadSnap(r);
									unsigned char mask = r->ReadByte();
									std::vector<std::shared_ptr<SemanticSlot>> slots(8);
									for (int i = 0; i < 8; i++)
									{
										if (r->ReadByte() != i)
										{
											return false;
										}
										unsigned int opcode = r->ReadUInt32();
										std::optional<int> cls = ReadOptional(r), ser = ReadOptional(r);
										unsigned char rd = r->ReadByte(), rs1 = r->ReadByte(), rs2 = r->ReadByte();
										long long imm = r->ReadInt64();
										std::optional<int> csr = ReadOptional(r);
									if (csr && (*csr < 0 || *csr > (std::numeric_limits<unsigned short>::max)()))
										{
											return false;
										}
										bool aq = Boolean(r), rl = Boolean(r);
										slots[i] = std::make_shared<SemanticSlot>(SemanticSlot{i, (mask & (1 << i)) != 0, opcode, cls, ser, rd, rs1, rs2, imm, csr ? std::optional<unsigned short>(static_cast<unsigned short>(*csr)) : std::nullopt, aq, rl, ReadSnap(r), ReadSnap(r), ReadSnap(r), ReadSnap(r), ReadSnap(r)});
										if (!slots[i]->RawSlot || !slots[i]->RawSlot->Content || slots[i]->RawSlot->Content->getLength() != 32 || !std::equal(slots[i]->RawSlot->Content->getSpan().begin(), slots[i]->RawSlot->Content->getSpan().end(), raw->getSpan().begin() + i * 32))
										{
											return false;
										}
									}
									result = DecodeResult::Success(raw, slots, sideband);
								}
								if (stream->getPosition() != stream->getLength())
								{
									result = nullptr;
									return false;
								}
								return true;
							}
						}
					}
					catch (const std::exception &)
					{
						result = nullptr;
						return false;
					}
				}
			}
		}
	}

}
