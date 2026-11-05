#include "TraceModel.h"
#include "../Semantic/CompactCodec.h"
#include "../Invalidation/FreshnessState.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <variant>
#include <cctype>
#include <string_view>
#include <cstdint>

namespace HybridCpu::ShadowFrontend
{
	namespace
	{
		struct HexFormatError final {};
		struct JsonNumber final { std::string Text; };
		struct JsonValue final
		{
			using Array = std::vector<JsonValue>;
			using Object = std::vector<std::pair<std::string, JsonValue>>;
			std::variant<std::nullptr_t, bool, JsonNumber, std::string, Array, Object> Value = nullptr;
			JsonValue() = default;
			JsonValue(bool value) : Value(value) {}
			JsonValue(JsonNumber value) : Value(std::move(value)) {}
			JsonValue(std::string value) : Value(std::move(value)) {}
			JsonValue(Array value) : Value(std::move(value)) {}
			JsonValue(Object value) : Value(std::move(value)) {}
		};

		void AppendUtf8(std::string &out, std::uint32_t cp)
		{
			if (cp <= 0x7f) out.push_back(static_cast<char>(cp));
			else if (cp <= 0x7ff)
			{
				out.push_back(static_cast<char>(0xc0 | (cp >> 6)));
				out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
			}
			else if (cp <= 0xffff)
			{
				out.push_back(static_cast<char>(0xe0 | (cp >> 12)));
				out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f)));
				out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
			}
			else
			{
				out.push_back(static_cast<char>(0xf0 | (cp >> 18)));
				out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3f)));
				out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3f)));
				out.push_back(static_cast<char>(0x80 | (cp & 0x3f)));
			}
		}

		std::string ToUtf8(std::wstring_view value)
		{
			std::string out;
			for (size_t i = 0; i < value.size(); ++i)
			{
				std::uint32_t cp = static_cast<std::uint32_t>(value[i]);
				if constexpr (sizeof(wchar_t) == 2)
				{
					if (cp >= 0xd800 && cp <= 0xdbff && i + 1 < value.size())
					{
						const std::uint32_t low = static_cast<std::uint32_t>(value[i + 1]);
						if (low >= 0xdc00 && low <= 0xdfff)
						{
							cp = 0x10000 + ((cp - 0xd800) << 10) + (low - 0xdc00);
							++i;
						}
					}
				}
				if (cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff)) cp = 0xfffd;
				AppendUtf8(out, cp);
			}
			return out;
		}

		std::wstring FromUtf8(std::string_view text)
		{
			std::wstring out;
			for (size_t i = 0; i < text.size();)
			{
				const auto lead = static_cast<unsigned char>(text[i++]);
				std::uint32_t cp;
				unsigned int continuation;
				if (lead <= 0x7f) { cp = lead; continuation = 0; }
				else if (lead >= 0xc2 && lead <= 0xdf) { cp = lead & 0x1f; continuation = 1; }
				else if (lead >= 0xe0 && lead <= 0xef) { cp = lead & 0x0f; continuation = 2; }
				else if (lead >= 0xf0 && lead <= 0xf4) { cp = lead & 0x07; continuation = 3; }
				else throw std::runtime_error("Invalid UTF-8 input.");
				if (i + continuation > text.size()) throw std::runtime_error("Truncated UTF-8 input.");
				for (unsigned int n = 0; n < continuation; ++n)
				{
					const auto byte = static_cast<unsigned char>(text[i++]);
					if ((byte & 0xc0) != 0x80) throw std::runtime_error("Invalid UTF-8 continuation byte.");
					cp = (cp << 6) | (byte & 0x3f);
				}
				if ((continuation == 1 && cp < 0x80) || (continuation == 2 && cp < 0x800) ||
					(continuation == 3 && cp < 0x10000) || cp > 0x10ffff || (cp >= 0xd800 && cp <= 0xdfff))
					throw std::runtime_error("Invalid UTF-8 scalar value.");
				if constexpr (sizeof(wchar_t) == 2)
				{
					if (cp > 0xffff)
					{
						cp -= 0x10000;
						out.push_back(static_cast<wchar_t>(0xd800 + (cp >> 10)));
						out.push_back(static_cast<wchar_t>(0xdc00 + (cp & 0x3ff)));
					}
					else out.push_back(static_cast<wchar_t>(cp));
				}
				else out.push_back(static_cast<wchar_t>(cp));
			}
			return out;
		}

		class JsonParser final
		{
			std::string_view text_;
			size_t pos_ = 0;
			void Space() { while (pos_ < text_.size() && (text_[pos_] == ' ' || text_[pos_] == '\t' || text_[pos_] == '\r' || text_[pos_] == '\n')) ++pos_; }
			char Take() { if (pos_ >= text_.size()) throw std::invalid_argument("Unexpected end of JSON."); return text_[pos_++]; }
			void Expect(char ch) { Space(); if (Take() != ch) throw std::invalid_argument("Invalid JSON token."); }
			unsigned int Hex4()
			{
				unsigned int value = 0;
				for (int i = 0; i < 4; ++i)
				{
					const char ch = Take();
					value <<= 4;
					if (ch >= '0' && ch <= '9') value |= static_cast<unsigned int>(ch - '0');
					else if (ch >= 'a' && ch <= 'f') value |= static_cast<unsigned int>(ch - 'a' + 10);
					else if (ch >= 'A' && ch <= 'F') value |= static_cast<unsigned int>(ch - 'A' + 10);
					else throw std::invalid_argument("Invalid JSON unicode escape.");
				}
				return value;
			}
			std::string String()
			{
				Expect('"');
				std::string out;
				while (true)
				{
					const unsigned char ch = static_cast<unsigned char>(Take());
					if (ch == '"') return out;
					if (ch < 0x20) throw std::invalid_argument("Unescaped control in JSON string.");
					if (ch != '\\') { out.push_back(static_cast<char>(ch)); continue; }
					const char escaped = Take();
					switch (escaped)
					{
					case '"': out.push_back('"'); break; case '\\': out.push_back('\\'); break; case '/': out.push_back('/'); break;
					case 'b': out.push_back('\b'); break; case 'f': out.push_back('\f'); break; case 'n': out.push_back('\n'); break;
					case 'r': out.push_back('\r'); break; case 't': out.push_back('\t'); break;
					case 'u':
					{
						std::uint32_t cp = Hex4();
						if (cp >= 0xd800 && cp <= 0xdbff)
						{
							if (Take() != '\\' || Take() != 'u') throw std::invalid_argument("Invalid JSON surrogate pair.");
							const std::uint32_t low = Hex4();
							if (low < 0xdc00 || low > 0xdfff) throw std::invalid_argument("Invalid JSON surrogate pair.");
							cp = 0x10000 + ((cp - 0xd800) << 10) + (low - 0xdc00);
						}
						else if (cp >= 0xdc00 && cp <= 0xdfff) throw std::invalid_argument("Unpaired JSON low surrogate.");
						AppendUtf8(out, cp);
						break;
					}
					default: throw std::invalid_argument("Invalid JSON escape.");
					}
				}
			}
		public:
			explicit JsonParser(std::string_view text) : text_(text) {}
			JsonValue Parse()
			{
				Space();
				if (pos_ >= text_.size()) throw std::invalid_argument("Empty JSON input.");
				const char ch = text_[pos_];
				if (ch == 'n') { if (text_.substr(pos_, 4) != "null") throw std::invalid_argument("Invalid JSON null."); pos_ += 4; return {}; }
				if (ch == 't' || ch == 'f') { const bool truth = ch == 't'; const auto word = truth ? "true" : "false"; if (text_.substr(pos_, truth ? 4 : 5) != word) throw std::invalid_argument("Invalid JSON boolean."); pos_ += truth ? 4 : 5; return JsonValue{truth}; }
				if (ch == '"') return JsonValue{String()};
				if (ch == '[')
				{
					++pos_; JsonValue::Array values; Space(); if (pos_ < text_.size() && text_[pos_] == ']') { ++pos_; return JsonValue{std::move(values)}; }
					while (true) { values.push_back(Parse()); Space(); const char sep = Take(); if (sep == ']') break; if (sep != ',') throw std::invalid_argument("Invalid JSON array."); }
					return JsonValue{std::move(values)};
				}
				if (ch == '{')
				{
					++pos_; JsonValue::Object object; Space(); if (pos_ < text_.size() && text_[pos_] == '}') { ++pos_; return JsonValue{std::move(object)}; }
					while (true)
					{
						Space(); std::string key = String(); Expect(':'); JsonValue item = Parse();
						auto existing = std::find_if(object.begin(), object.end(), [&](const auto &entry) { return entry.first == key; });
						if (existing == object.end()) object.emplace_back(std::move(key), std::move(item)); else existing->second = std::move(item);
						Space(); const char sep = Take(); if (sep == '}') break; if (sep != ',') throw std::invalid_argument("Invalid JSON object.");
					}
					return JsonValue{std::move(object)};
				}
				const size_t start = pos_;
				if (pos_ < text_.size() && text_[pos_] == '-') ++pos_;
				if (pos_ >= text_.size() || !std::isdigit(static_cast<unsigned char>(text_[pos_]))) throw std::invalid_argument("Invalid JSON number.");
				if (text_[pos_] == '0') ++pos_;
				else while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
				if (pos_ < text_.size() && text_[pos_] == '.')
				{
					++pos_; const size_t fraction = pos_;
					while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
					if (fraction == pos_) throw std::invalid_argument("Invalid JSON fraction.");
				}
				if (pos_ < text_.size() && (text_[pos_] == 'e' || text_[pos_] == 'E'))
				{
					++pos_; if (pos_ < text_.size() && (text_[pos_] == '+' || text_[pos_] == '-')) ++pos_;
					const size_t exponent = pos_;
					while (pos_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) ++pos_;
					if (exponent == pos_) throw std::invalid_argument("Invalid JSON exponent.");
				}
				return JsonValue{JsonNumber{std::string(text_.substr(start, pos_ - start))}};
			}
			void RequireEnd() { Space(); if (pos_ != text_.size()) throw std::invalid_argument("Trailing JSON data."); }
		};

		JsonValue JString(const std::wstring &value) { return JsonValue{ToUtf8(value)}; }
		JsonValue JNumber(long long value) { return JsonValue{JsonNumber{std::to_string(value)}}; }
		JsonValue JUnsigned(unsigned long long value) { return JsonValue{JsonNumber{std::to_string(value)}}; }
		JsonValue JBool(bool value) { return JsonValue{value}; }
		JsonValue JNull() { return JsonValue{}; }
		JsonValue JOptionalString(const std::optional<std::wstring> &value) { return value ? JString(*value) : JNull(); }
		JsonValue JObject(JsonValue::Object fields) { return JsonValue{std::move(fields)}; }
		JsonValue JRequest(const std::shared_ptr<BundleRequest> &value)
		{
			if (!value) return JNull();
			return JObject({{"Pc", JUnsigned(value->Pc)}, {"VirtualThread", JNumber(value->VirtualThread)}, {"TransportSerial", JUnsigned(value->TransportSerial)}, {"Cursor", JNumber(value->Cursor)}});
		}
		JsonValue JContext(const std::shared_ptr<DecodeContext> &value)
		{
			if (!value) return JNull();
			return JObject({{"ManifestVersion", JString(value->ManifestVersion)}, {"ManifestHash", JString(value->ManifestHash)}, {"Extensions", JString(value->Extensions)}, {"DecoderEpoch", JString(value->DecoderEpoch)}, {"DecoderVersion", JString(value->DecoderVersion)}, {"Privilege", JString(value->Privilege)}, {"Domain", JString(value->Domain)}, {"AddressSpace", JString(value->AddressSpace)}, {"VectorConfiguration", JString(value->VectorConfiguration)}, {"ExecutableEpoch", JUnsigned(value->ExecutableEpoch)}, {"CodeEpoch", JUnsigned(value->CodeEpoch)}, {"ReplayEligible", JBool(value->ReplayEligible)}, {"ContextIncarnation", JString(value->ContextIncarnation)}, {"SidebandIdentity", JString(value->SidebandIdentity)}, {"IsBound", JBool(value->isBound())}});
		}
		JsonValue JFreshness(const FreshnessToken &value) { return JObject({{"Incarnation", JUnsigned(value.Incarnation)}, {"Generation", JUnsigned(value.Generation)}}); }
		JsonValue JBinding(const std::shared_ptr<HotBinding> &value)
		{
			if (!value) return JNull();
			return JObject({{"Structural", JString(value->Structural)}, {"Certificate", JString(value->Certificate)}, {"Owner", JString(value->Owner)}, {"Boundary", JString(value->Boundary)}, {"Serializing", JUnsigned(value->Serializing)}, {"Phase", JUnsigned(value->Phase)}, {"Cursor", JNumber(value->Cursor)}});
		}
		JsonValue JLookupTag(const std::shared_ptr<LookupTag> &value)
		{
			if (!value) return JNull();
			return JObject({{"Pc", JUnsigned(value->Pc)}, {"VirtualThread", JNumber(value->VirtualThread)}, {"Context", JContext(value->Context)}, {"Freshness", JFreshness(value->Freshness)}});
		}
		JsonValue JTraceRow(const std::shared_ptr<TraceRow> &row)
		{
			return JObject({{"Sequence", JNumber(row->Sequence)}, {"Workload", JString(row->Workload)}, {"Provenance", JString(row->Provenance)}, {"Request", JRequest(row->Request)}, {"Occupancy", JNumber(row->Occupancy)}, {"Contour", JString(row->Contour)}, {"Event", row->Event ? JNumber(static_cast<int>(*row->Event)) : JNull()}, {"Committed", JBool(row->Committed)}, {"ReplayObserved", JBool(row->ReplayObserved)}});
		}
		JsonValue JReplayObservation(const std::shared_ptr<ReplayObservation> &row)
		{
			JsonValue::Array guards;
			if (row->RejectedGuards) for (auto guard : *row->RejectedGuards) guards.push_back(JNumber(static_cast<int>(guard)));
			JsonValue::Object object{
				{"Version", JNumber(row->Version)}, {"Seed", JNumber(row->Seed)}, {"Sequence", JNumber(row->Sequence)}, {"Provenance", JString(row->Provenance)},
				{"Request", JRequest(row->Request)}, {"Context", JContext(row->Context)}, {"Freshness", JFreshness(row->Freshness)}, {"ReplayGeneration", JUnsigned(row->ReplayGeneration)},
				{"Event", row->Event ? JNumber(static_cast<int>(*row->Event)) : JNull()}, {"CommittedOrder", JUnsigned(row->CommittedOrder)}, {"Committed", JBool(row->Committed)}, {"Clear", JBool(row->Clear)},
				{"RawHex", JString(row->RawHex)}, {"Allowed", JBool(row->Allowed)}, {"FetchFault", JOptionalString(row->FetchFault)}, {"CandidateBinding", JBinding(row->CandidateBinding)},
				{"CurrentBinding", JBinding(row->CurrentBinding)}, {"CandidateRawHex", JOptionalString(row->CandidateRawHex)}, {"RejectedGuards", row->RejectedGuards ? JsonValue{std::move(guards)} : JNull()},
				{"ExpectedEncodedHex", JOptionalString(row->ExpectedEncodedHex)}, {"ExpectedAccessFault", JOptionalString(row->ExpectedAccessFault)}, {"ExpectedOutcome", JNumber(static_cast<int>(row->ExpectedOutcome))},
				{"Capacity", JNumber(row->Capacity)}, {"Ways", JNumber(row->Ways)}, {"ByteBudget", JNumber(row->ByteBudget)}, {"Enabled", JBool(row->Enabled)}, {"Policy", JNumber(static_cast<int>(row->Policy))},
				{"ActiveVts", JNumber(row->ActiveVts)}, {"CandidateTag", JLookupTag(row->CandidateTag)}, {"CandidateReplayGeneration", row->CandidateReplayGeneration ? JUnsigned(*row->CandidateReplayGeneration) : JNull()},
				{"SidebandContour", JString(row->SidebandContour)}, {"ReplayProtocol", JString(row->ReplayProtocol)}};
			return JObject(std::move(object));
		}
		void WriteJsonString(std::string &out, std::string_view text)
		{
			out.push_back('"');
			for (unsigned char ch : text)
			{
				switch (ch)
				{
				case '"': out += "\\\""; break; case '\\': out += "\\\\"; break; case '\b': out += "\\b"; break;
				case '\f': out += "\\f"; break; case '\n': out += "\\n"; break; case '\r': out += "\\r"; break; case '\t': out += "\\t"; break;
				default: if (ch < 0x20) { static constexpr char digits[] = "0123456789ABCDEF"; out += "\\u00"; out.push_back(digits[ch >> 4]); out.push_back(digits[ch & 0xf]); } else out.push_back(static_cast<char>(ch));
				}
			}
			out.push_back('"');
		}
		void WriteJsonValue(std::string &out, const JsonValue &value)
		{
			if (std::holds_alternative<std::nullptr_t>(value.Value)) { out += "null"; return; }
			if (auto boolean = std::get_if<bool>(&value.Value)) { out += *boolean ? "true" : "false"; return; }
			if (auto number = std::get_if<JsonNumber>(&value.Value)) { out += number->Text; return; }
			if (auto string = std::get_if<std::string>(&value.Value)) { WriteJsonString(out, *string); return; }
			if (auto array = std::get_if<JsonValue::Array>(&value.Value))
			{
				out.push_back('['); bool first = true; for (const auto &item : *array) { if (!first) out.push_back(','); first = false; WriteJsonValue(out, item); } out.push_back(']'); return;
			}
			const auto &object = std::get<JsonValue::Object>(value.Value);
			out.push_back('{'); bool first = true; for (const auto &[key, item] : object) { if (!first) out.push_back(','); first = false; WriteJsonString(out, key); out.push_back(':'); WriteJsonValue(out, item); } out.push_back('}');
		}
		std::string SerializeJson(const JsonValue &value) { std::string out; WriteJsonValue(out, value); return out; }
		const JsonValue &Field(const JsonValue &value, const char *name)
		{
			const auto *object = std::get_if<JsonValue::Object>(&value.Value);
			if (!object) throw std::invalid_argument("Expected JSON object.");
			auto found = std::find_if(object->begin(), object->end(), [&](const auto &entry) { return entry.first == name; });
			if (found == object->end()) throw std::invalid_argument("Missing JSON property."); return found->second;
		}
		bool IsNull(const JsonValue &value) { return std::holds_alternative<std::nullptr_t>(value.Value); }
		long long Int(const JsonValue &value)
		{
			const auto *number = std::get_if<JsonNumber>(&value.Value); if (!number) throw std::invalid_argument("Expected JSON integer.");
			size_t used = 0; long long parsed = std::stoll(number->Text, &used); if (used != number->Text.size()) throw std::invalid_argument("Expected JSON integer."); return parsed;
		}
		int Int32(const JsonValue &value)
		{
			const long long parsed = Int(value);
			if (parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max())
				throw std::out_of_range("JSON integer is outside Int32 range.");
			return static_cast<int>(parsed);
		}
		unsigned char UInt8(const JsonValue &value)
		{
			const long long parsed = Int(value);
			if (parsed < 0 || parsed > std::numeric_limits<unsigned char>::max())
				throw std::out_of_range("JSON integer is outside byte range.");
			return static_cast<unsigned char>(parsed);
		}
		unsigned long long UInt(const JsonValue &value)
		{
			const auto *number = std::get_if<JsonNumber>(&value.Value); if (!number) throw std::invalid_argument("Expected JSON unsigned integer.");
			size_t used = 0; unsigned long long parsed = std::stoull(number->Text, &used); if (used != number->Text.size() || (!number->Text.empty() && number->Text[0] == '-')) throw std::invalid_argument("Expected JSON unsigned integer."); return parsed;
		}
		bool Bool(const JsonValue &value) { const auto *result = std::get_if<bool>(&value.Value); if (!result) throw std::invalid_argument("Expected JSON boolean."); return *result; }
		std::wstring WString(const JsonValue &value) { const auto *result = std::get_if<std::string>(&value.Value); if (!result) throw std::invalid_argument("Expected JSON string."); return FromUtf8(*result); }
		std::optional<std::wstring> OptionalString(const JsonValue &value) { return IsNull(value) ? std::nullopt : std::optional<std::wstring>(WString(value)); }
		std::shared_ptr<BundleRequest> ParseRequest(const JsonValue &value)
		{
			if (IsNull(value)) return nullptr;
			auto result = std::make_shared<BundleRequest>(); result->Pc = UInt(Field(value, "Pc")); result->VirtualThread = Int32(Field(value, "VirtualThread"));
			result->TransportSerial = UInt(Field(value, "TransportSerial")); result->Cursor = Int32(Field(value, "Cursor")); return result;
		}
		std::shared_ptr<DecodeContext> ParseContext(const JsonValue &value)
		{
			if (IsNull(value)) return nullptr;
			auto result = std::make_shared<DecodeContext>();
			result->ManifestVersion = WString(Field(value, "ManifestVersion")); result->ManifestHash = WString(Field(value, "ManifestHash")); result->Extensions = WString(Field(value, "Extensions"));
			result->DecoderEpoch = WString(Field(value, "DecoderEpoch")); result->DecoderVersion = WString(Field(value, "DecoderVersion")); result->Privilege = WString(Field(value, "Privilege"));
			result->Domain = WString(Field(value, "Domain")); result->AddressSpace = WString(Field(value, "AddressSpace")); result->VectorConfiguration = WString(Field(value, "VectorConfiguration"));
			result->ExecutableEpoch = UInt(Field(value, "ExecutableEpoch")); result->CodeEpoch = UInt(Field(value, "CodeEpoch")); result->ReplayEligible = Bool(Field(value, "ReplayEligible"));
			result->ContextIncarnation = WString(Field(value, "ContextIncarnation")); result->SidebandIdentity = WString(Field(value, "SidebandIdentity")); return result;
		}
		FreshnessToken ParseFreshness(const JsonValue &value) { return FreshnessToken{UInt(Field(value, "Incarnation")), UInt(Field(value, "Generation"))}; }
		std::shared_ptr<HotBinding> ParseBinding(const JsonValue &value)
		{
			if (IsNull(value)) return nullptr;
			auto result = std::make_shared<HotBinding>(); result->Structural = WString(Field(value, "Structural")); result->Certificate = WString(Field(value, "Certificate")); result->Owner = WString(Field(value, "Owner")); result->Boundary = WString(Field(value, "Boundary"));
			result->Serializing = UInt(Field(value, "Serializing")); result->Phase = UInt(Field(value, "Phase")); result->Cursor = Int32(Field(value, "Cursor")); return result;
		}
		std::shared_ptr<LookupTag> ParseLookupTag(const JsonValue &value)
		{
			if (IsNull(value)) return nullptr;
			auto result = std::make_shared<LookupTag>(); result->Pc = UInt(Field(value, "Pc")); result->VirtualThread = Int32(Field(value, "VirtualThread"));
			result->Context = ParseContext(Field(value, "Context")); result->Freshness = ParseFreshness(Field(value, "Freshness")); return result;
		}
		std::shared_ptr<TraceRow> ParseTraceRow(const JsonValue &value)
		{
			auto row = std::make_shared<TraceRow>(Int32(Field(value, "Sequence")), WString(Field(value, "Workload")), WString(Field(value, "Provenance")), ParseRequest(Field(value, "Request")), Int32(Field(value, "Occupancy")), WString(Field(value, "Contour")), std::nullopt, Bool(Field(value, "Committed")), Bool(Field(value, "ReplayObserved")));
			const auto &event = Field(value, "Event"); if (!IsNull(event)) row->Event = static_cast<FrontendEvent>(Int32(event)); return row;
		}
		std::shared_ptr<ReplayObservation> ParseReplayObservation(const JsonValue &value)
		{
			auto row = std::make_shared<ReplayObservation>();
			row->Version = Int32(Field(value, "Version")); row->Seed = Int32(Field(value, "Seed")); row->Sequence = Int32(Field(value, "Sequence"));
			row->Provenance = WString(Field(value, "Provenance")); row->Request = ParseRequest(Field(value, "Request")); row->Context = ParseContext(Field(value, "Context")); row->Freshness = ParseFreshness(Field(value, "Freshness"));
			row->ReplayGeneration = UInt(Field(value, "ReplayGeneration")); const auto &event = Field(value, "Event"); if (!IsNull(event)) row->Event = static_cast<FrontendEvent>(Int32(event));
			row->CommittedOrder = UInt(Field(value, "CommittedOrder")); row->Committed = Bool(Field(value, "Committed")); row->Clear = Bool(Field(value, "Clear")); row->RawHex = WString(Field(value, "RawHex")); row->Allowed = Bool(Field(value, "Allowed"));
			row->FetchFault = OptionalString(Field(value, "FetchFault")); row->CandidateBinding = ParseBinding(Field(value, "CandidateBinding")); row->CurrentBinding = ParseBinding(Field(value, "CurrentBinding")); row->CandidateRawHex = OptionalString(Field(value, "CandidateRawHex"));
			const auto &guards = Field(value, "RejectedGuards"); if (!IsNull(guards)) { const auto *items = std::get_if<JsonValue::Array>(&guards.Value); if (!items) throw std::invalid_argument("RejectedGuards must be an array or null."); row->RejectedGuards = std::vector<HotGuard>{}; for (const auto &item : *items) row->RejectedGuards->push_back(static_cast<HotGuard>(Int32(item))); }
			row->ExpectedEncodedHex = OptionalString(Field(value, "ExpectedEncodedHex")); row->ExpectedAccessFault = OptionalString(Field(value, "ExpectedAccessFault")); row->ExpectedOutcome = static_cast<IngressOutcome>(Int32(Field(value, "ExpectedOutcome")));
			row->Capacity = Int32(Field(value, "Capacity")); row->Ways = Int32(Field(value, "Ways")); row->ByteBudget = Int32(Field(value, "ByteBudget")); row->Enabled = Bool(Field(value, "Enabled")); row->Policy = static_cast<ReplacementPolicy>(Int32(Field(value, "Policy")));
			row->ActiveVts = UInt8(Field(value, "ActiveVts")); row->CandidateTag = ParseLookupTag(Field(value, "CandidateTag")); const auto &generation = Field(value, "CandidateReplayGeneration"); if (!IsNull(generation)) row->CandidateReplayGeneration = UInt(generation);
			row->SidebandContour = WString(Field(value, "SidebandContour")); row->ReplayProtocol = WString(Field(value, "ReplayProtocol")); return row;
		}
		template<typename Callback>
		void ForEachUtf8Line(const std::wstring &path, size_t maxChars, size_t maxBytes, size_t maxLines, Callback callback)
		{
			std::ifstream input(std::filesystem::path(path), std::ios::binary);
			if (!input) throw std::runtime_error("Unable to open trace file.");
			std::string bytes; bytes.reserve(256); size_t lineCount = 0;
			auto emit = [&](bool terminatedByLf)
			{
				// File.ReadLines-style CRLF handling: the CR is a line terminator
				// component, not part of the JSON payload or its size limits.
				if (terminatedByLf && !bytes.empty() && bytes.back() == '\r') bytes.pop_back();
				if (bytes.size() > maxBytes) throw std::length_error("Trace UTF-8 line bound exceeded.");
				if (lineCount >= maxLines) throw std::length_error("Trace row bound exceeded.");
				if (lineCount == 0 && bytes.size() >= 3 && static_cast<unsigned char>(bytes[0]) == 0xef && static_cast<unsigned char>(bytes[1]) == 0xbb && static_cast<unsigned char>(bytes[2]) == 0xbf)
					bytes.erase(0, 3);
				auto line = FromUtf8(bytes);
				if (line.size() > maxChars) throw std::length_error("Trace line character bound exceeded.");
				callback(std::move(line)); ++lineCount; bytes.clear();
			};
			char ch;
			while (input.get(ch))
			{
				if (ch != '\n')
				{
					if (bytes.size() >= maxChars * 4 + 4) throw std::length_error("Trace line bound exceeded.");
					bytes.push_back(ch); continue;
				}
				emit(true);
			}
			if (input.bad()) throw std::runtime_error("Trace file read failed.");
			if (!bytes.empty()) emit(false);
		}
		std::vector<unsigned char> ParseHex(std::wstring_view text)
		{
			if ((text.size() & 1u) != 0) throw HexFormatError{};
			std::vector<unsigned char> bytes;
			bytes.reserve(text.size() / 2);
			auto digit = [](wchar_t ch) -> unsigned int
			{
				if (ch >= L'0' && ch <= L'9') return static_cast<unsigned int>(ch - L'0');
				if (ch >= L'a' && ch <= L'f') return static_cast<unsigned int>(ch - L'a' + 10);
				if (ch >= L'A' && ch <= L'F') return static_cast<unsigned int>(ch - L'A' + 10);
				throw HexFormatError{};
			};
			for (size_t i = 0; i < text.size(); i += 2)
				bytes.push_back(static_cast<unsigned char>((digit(text[i]) << 4) | digit(text[i + 1])));
			return bytes;
		}

		bool IsDefined(IngressOutcome value)
		{
			switch (value)
			{
			case IngressOutcome::Cold:
			case IngressOutcome::Warm:
			case IngressOutcome::Hot:
			case IngressOutcome::AccessFault:
			case IngressOutcome::RetryExhausted: return true;
			default: return false;
			}
		}

		bool IsDefined(FrontendEvent value)
		{
			switch (value)
			{
			case FrontendEvent::ExecutableWrite:
			case FrontendEvent::FenceI:
			case FrontendEvent::Reset:
			case FrontendEvent::Restore:
			case FrontendEvent::Context:
			case FrontendEvent::Domain:
			case FrontendEvent::Privilege:
			case FrontendEvent::Configuration:
			case FrontendEvent::Sideband:
			case FrontendEvent::Policy:
			case FrontendEvent::Redirect:
			case FrontendEvent::Trap:
			case FrontendEvent::Rollback:
			case FrontendEvent::ReplayPhase:
			case FrontendEvent::Serializing: return true;
			default: return false;
			}
		}

		bool IsDefined(HotGuard value)
		{
			switch (value)
			{
			case HotGuard::Freshness:
			case HotGuard::Integrity:
			case HotGuard::Structural:
			case HotGuard::Certificate:
			case HotGuard::Owner:
			case HotGuard::Context:
			case HotGuard::Domain:
			case HotGuard::Boundary:
			case HotGuard::Serializing:
			case HotGuard::ReplayPhase:
			case HotGuard::Cursor:
			case HotGuard::Legality:
			case HotGuard::Capacity:
			case HotGuard::Hazard:
			case HotGuard::Scoreboard:
			case HotGuard::Backpressure:
			case HotGuard::Speculation:
			case HotGuard::Assist:
			case HotGuard::Fsp:
			case HotGuard::Vdsa: return true;
			default: return false;
			}
		}

		bool IsDefined(ReplacementPolicy value)
		{
			return value == ReplacementPolicy::Lru || value == ReplacementPolicy::QuotaBorrow;
		}
	}

	void ObservationTrace::Validate(std::shared_ptr<ReplayObservation> row)
	{
		row->Request->Validate();
		if (row->Version != Version || row->Sequence < 0 ||
			(row->Provenance != L"synthetic" && row->Provenance != L"recorded-production") ||
			row->Context == nullptr || row->RawHex.size() != 512 ||
			row->Freshness.Incarnation == 0 || !IsDefined(row->ExpectedOutcome) ||
			(row->Event && !IsDefined(*row->Event)) || !row->RejectedGuards ||
			std::any_of(row->RejectedGuards->begin(), row->RejectedGuards->end(), [](HotGuard guard)
			{
				return !IsDefined(guard);
			}))
		{
			throw InvalidDataException(L"Unsupported observation schema/state.");
		}
		if (row->Capacity < row->Ways || row->Ways < 1 || row->Capacity % row->Ways != 0 || row->Capacity > 4096 || row->ByteBudget <= 0 || row->ActiveVts == 0 || row->ActiveVts > 15 || !IsDefined(row->Policy) || row->SidebandContour != L"default-no-annotations" || row->ReplayProtocol != L"absolute-state-prime-once-v1")
		{
			throw InvalidDataException(L"Unsupported replay configuration/sideband protocol.");
		}
		try
		{
			(void)ParseHex(row->RawHex);
			if (row->CandidateRawHex && (row->CandidateRawHex->size() != 512 || ParseHex(*row->CandidateRawHex).size() != 256))
			{
				throw InvalidDataException(L"Invalid candidate carrier.");
			}
			if (row->ExpectedEncodedHex)
			{
				std::shared_ptr<DecodeResult> decoded;
				if (row->ExpectedEncodedHex->size() > CompactCodec::MaxEncodedBytes * 2 ||
					!CompactCodec::TryDecode(ParseHex(*row->ExpectedEncodedHex), decoded))
				{
					throw InvalidDataException(L"Invalid expected facts.");
				}
			}
		}
		catch (const HexFormatError &)
		{
			throw InvalidDataException(L"Invalid hex observation.");
		}
		if ((row->CandidateBinding == nullptr) != !row->CandidateRawHex || (row->CandidateBinding != nullptr && row->CurrentBinding == nullptr))
		{
			throw InvalidDataException(L"Unpaired HOT observation.");
		}
	}

	void ObservationTrace::Write(const std::wstring &path, std::vector<std::shared_ptr<ReplayObservation>> &rows)
	{
		std::ofstream writer(std::filesystem::path(path), std::ios::binary | std::ios::trunc);
		if (!writer) throw InvalidDataException(L"Unable to create observation trace.");
		size_t count = 0;
		for (const auto &row : rows)
		{
			Validate(row);
			const std::string line = SerializeJson(JReplayObservation(row));
			if (++count > static_cast<size_t>(MaxRows) || line.size() > static_cast<size_t>(MaxLineBytes))
				throw InvalidDataException(L"Trace bound exceeded.");
			writer.write(line.data(), static_cast<std::streamsize>(line.size()));
			writer.write("\r\n", 2);
			if (!writer) throw InvalidDataException(L"Observation trace write failed.");
		}
	}

	std::vector<std::shared_ptr<ReplayObservation>> ObservationTrace::Read(const std::wstring &path)
	{
		std::vector<std::shared_ptr<ReplayObservation>> rows;
		try
		{
			ForEachUtf8Line(path, static_cast<size_t>(MaxLineBytes), static_cast<size_t>(MaxLineBytes), static_cast<size_t>(MaxRows), [&](std::wstring text)
			{
				std::shared_ptr<ReplayObservation> row;
				try
				{
					JsonParser parser(ToUtf8(text));
					auto value = parser.Parse(); parser.RequireEnd(); row = ParseReplayObservation(value);
				}
				catch (const std::exception &)
				{
					throw InvalidDataException(L"Invalid observation JSON.");
				}
				if (!row) throw InvalidDataException(L"Null observation.");
				Validate(row);
				rows.push_back(std::move(row));
			});
		}
		catch (const std::length_error &)
		{
			throw InvalidDataException(L"Trace line/row bound exceeded.");
		}
		catch (const InvalidDataException &)
		{
			throw;
		}
		catch (const std::runtime_error &)
		{
			throw InvalidDataException(L"Invalid UTF-8 observation trace.");
		}
		return rows;
	}

	std::vector<std::wstring> TraceModel::getWorkloads()
	{
		return Workloads;
	}

	std::vector<std::wstring> TraceModel::getWeightedNonLoop()
	{
		return WeightedNonLoop;
	}

	std::vector<std::shared_ptr<TraceRow>> TraceModel::Generate(const std::wstring &workload, int count)
	{
		const auto workloads = getWorkloads();
		if (std::find(workloads.begin(), workloads.end(), workload) == workloads.end())
		{
			throw std::invalid_argument("Unknown workload");
		}
		std::vector<std::shared_ptr<TraceRow>> rows;
		if (count > 0) rows.reserve(static_cast<size_t>(count));
		for (int n = 0; n < count; n++)
		{
			int vt = workload.starts_with(L"smt-") || workload == L"interleaved-loops" ? n % 4 : 0;
			int temp_var185;
// C# TO C++ CONVERTER NOTE: The following 'switch' operated on a string and was converted to C++ 'if-else' logic:
//			switch (workload)
// ORIGINAL LINE: case "streaming":
			if (workload == L"streaming")
			{
					temp_var185 = n;
			}
// ORIGINAL LINE: case "small-loop":
			else if (workload == L"small-loop")
			{
					temp_var185 = 0;
			}
// ORIGINAL LINE: case "interleaved-loops":
			else if (workload == L"interleaved-loops")
			{
					temp_var185 = vt;
			}
// ORIGINAL LINE: case "branchy":
			else if (workload == L"branchy")
			{
					temp_var185 = (n * 17 + n / 9) % 24;
			}
// ORIGINAL LINE: case "smt-different-code":
			else if (workload == L"smt-different-code")
			{
					temp_var185 = vt * 64 + (n / 4 % 12);
			}
// ORIGINAL LINE: case "smt-same-code":
			else if (workload == L"smt-same-code")
			{
					temp_var185 = n / 4 % 12;
			}
			else
			{
					temp_var185 = n % 16;
			}
			int bundle = temp_var185;
			int occupancy = workload.starts_with(L"scalar-") ? std::stoi(workload.substr(7)) : 4;
			std::optional<FrontendEvent> ev;
			if (workload == L"smc-jit" && n % 19 == 0) ev = FrontendEvent::ExecutableWrite;
			else if (workload == L"smc-jit" && n % 19 == 1) ev = FrontendEvent::FenceI;
			else if (workload == L"context-churn" && n % 13 == 0) ev = FrontendEvent::Context;
			const std::wstring contour = (workload == L"vector" || workload == L"tile") ? workload : L"scalar";
			const bool replayObserved = workload == L"small-loop" || workload == L"interleaved-loops";
			auto request = std::make_shared<BundleRequest>(BundleRequest{static_cast<unsigned long long>(bundle * 256), vt, static_cast<unsigned long long>(n)});
			rows.push_back(std::make_shared<TraceRow>(n, workload, L"synthetic", request, occupancy, contour, ev, true, replayObserved));
		}
		return rows;
	}

	std::vector<std::shared_ptr<TraceRow>> TraceModel::ReadJsonLines(const std::wstring &path)
	{
		std::vector<std::shared_ptr<TraceRow>> rows;
		try
		{
			ForEachUtf8Line(path, 65536, 65536 * 4, static_cast<size_t>(std::numeric_limits<int>::max()), [&](std::wstring line)
			{
				std::shared_ptr<TraceRow> row;
				try
				{
					JsonParser parser(ToUtf8(line)); auto value = parser.Parse(); parser.RequireEnd(); row = ParseTraceRow(value);
				}
				catch (const std::exception &)
				{
					throw InvalidDataException(L"Invalid trace row JSON.");
				}
				if (!row) throw InvalidDataException(L"Null trace row.");
				if (!row->Request) throw InvalidDataException(L"Trace row has no request.");
				row->Request->Validate();
				if (row->Occupancy < 0 || row->Occupancy > 8 || (row->Provenance != L"synthetic" && row->Provenance != L"recorded-production"))
					throw InvalidDataException(L"Invalid trace provenance/occupancy.");
				rows.push_back(std::move(row));
			});
		}
		catch (const std::length_error &) { throw InvalidDataException(L"Trace row exceeds bound."); }
		catch (const InvalidDataException &) { throw; }
		catch (const std::runtime_error &) { throw InvalidDataException(L"Invalid UTF-8 trace file."); }
		return rows;
	}

	void TraceModel::WriteJsonLines(const std::wstring &path, std::vector<std::shared_ptr<TraceRow>> &trace)
	{
		std::ofstream writer(std::filesystem::path(path), std::ios::binary | std::ios::trunc);
		if (!writer) throw InvalidDataException(L"Unable to create trace file.");
		for (const auto &row : trace)
		{
			const std::string line = SerializeJson(JTraceRow(row));
			writer.write(line.data(), static_cast<std::streamsize>(line.size())); writer.write("\r\n", 2);
			if (!writer) throw InvalidDataException(L"Trace file write failed.");
		}
	}

	std::vector<std::shared_ptr<ModelRun>> ModelHarness::Sweep(std::function<ModelRun(std::vector<std::shared_ptr<TraceRow>>, const std::wstring&, int, ReplacementPolicy, int)> run)
	{
		std::vector<std::shared_ptr<ModelRun>> results;
		for (auto workload : TraceModel::getWorkloads())
		{
			std::vector<std::shared_ptr<TraceRow>> trace = TraceModel::Generate(workload);
			for (auto capacity : std::vector<int> {32, 64, 128})
			{
				for (auto policy : {ReplacementPolicy::Lru, ReplacementPolicy::QuotaBorrow})
				{
					for (auto mode : std::vector<std::wstring> {L"all-cold", L"existing-loop", L"dbc", L"dbc-existing-loop"})
					{
						results.push_back(std::make_shared<ModelRun>(run(trace, mode, capacity, policy, std::numeric_limits<int>::max())));
					}
				}
			}
			for (auto budget : std::vector<int> {8192, 16384, 32768})
			{
				results.push_back(std::make_shared<ModelRun>(run(trace, L"dbc", 128, ReplacementPolicy::QuotaBorrow, budget)));
			}
		}
		return results;
	}
}
