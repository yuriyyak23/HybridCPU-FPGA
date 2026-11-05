#pragma once

// Minimal late-bound Brotli C API adapter.  It deliberately keeps all Brotli state
// and allocator activity inside the loaded DLL and passes only caller-owned buffers.
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <limits>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>

namespace HybridCpu::ShadowFrontend::BrotliCompat {
namespace detail {

using AllocFunc = void* (__cdecl*)(void*, size_t);
using FreeFunc = void (__cdecl*)(void*, void*);
using DecoderState = void;
using EncoderState = void;
using DecoderCreate = DecoderState* (__cdecl*)(AllocFunc, FreeFunc, void*);
using DecoderDestroy = void (__cdecl*)(DecoderState*);
using DecoderDecompressStream = int (__cdecl*)(DecoderState*, size_t*, const uint8_t**, size_t*, uint8_t**, size_t*);
using EncoderCompress = int (__cdecl*)(int, int, int, size_t, const uint8_t*, size_t*, uint8_t*);
using EncoderMaxCompressedSize = size_t (__cdecl*)(size_t);

struct Api final {
    HMODULE common = nullptr;
    HMODULE decoder = nullptr;
    HMODULE encoder = nullptr;
    DecoderCreate decoderCreate = nullptr;
    DecoderDestroy decoderDestroy = nullptr;
    DecoderDecompressStream decoderDecompressStream = nullptr;
    EncoderCompress encoderCompress = nullptr;
    EncoderMaxCompressedSize encoderMaxCompressedSize = nullptr;

    Api() {
        // First honor trusted Windows DLL search locations (including app-local).
        if (LoadSet(nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)) return;

        // Load PATH candidates as an app-local-style trio. Using absolute paths and
        // DLL_LOAD_DIR makes each dependent DLL resolve beside its Brotli module,
        // rather than relying on the process current directory or an ambient PATH
        // lookup for dependencies.
        wchar_t pathBuffer[32768]{};
        const DWORD pathLength = GetEnvironmentVariableW(L"PATH", pathBuffer,
            static_cast<DWORD>(sizeof(pathBuffer) / sizeof(pathBuffer[0])));
        if (pathLength == 0 || pathLength >= sizeof(pathBuffer) / sizeof(pathBuffer[0])) return;
        const wchar_t* cursor = pathBuffer;
        while (*cursor) {
            const wchar_t* separator = std::wcschr(cursor, L';');
            std::wstring directory(cursor, separator ? static_cast<size_t>(separator - cursor) : std::wcslen(cursor));
            cursor = separator ? separator + 1 : cursor + directory.size();
            if (directory.size() >= 2 && directory.front() == L'"' && directory.back() == L'"')
                directory = directory.substr(1, directory.size() - 2);
            if (directory.empty()) continue;
            std::wstring absoluteDirectory;
            if (!MakeAbsolute(directory, absoluteDirectory)) continue;
            if (LoadSet(absoluteDirectory.c_str(), LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32)) return;
        }
    }

    ~Api() {
        if (encoder) FreeLibrary(encoder);
        if (decoder) FreeLibrary(decoder);
        if (common) FreeLibrary(common);
    }

    Api(const Api&) = delete;
    Api& operator=(const Api&) = delete;

    bool CanEncode() const noexcept { return encoderCompress && encoderMaxCompressedSize; }
    bool CanDecode() const noexcept { return decoderCreate && decoderDestroy && decoderDecompressStream; }

private:
    static bool MakeAbsolute(const std::wstring& path, std::wstring& absolute) noexcept {
        try {
            std::vector<wchar_t> buffer(32768);
            const DWORD length = GetFullPathNameW(path.c_str(), static_cast<DWORD>(buffer.size()), buffer.data(), nullptr);
            if (length == 0 || length >= buffer.size()) return false;
            absolute.assign(buffer.data(), length);
            return true;
        } catch (...) { return false; }
    }

    static HMODULE LoadFromDirectory(const wchar_t* directory, const wchar_t* name, DWORD flags) noexcept {
        if (!directory) return LoadLibraryExW(name, nullptr, flags);
        try {
            std::wstring fullPath(directory);
            if (!fullPath.empty() && fullPath.back() != L'\\' && fullPath.back() != L'/') fullPath.push_back(L'\\');
            fullPath += name;
            return LoadLibraryExW(fullPath.c_str(), nullptr, flags);
        } catch (...) { return nullptr; }
    }

    void ResetModules() noexcept {
        if (encoder) FreeLibrary(encoder);
        if (decoder) FreeLibrary(decoder);
        if (common) FreeLibrary(common);
        encoder = decoder = common = nullptr;
        decoderCreate = nullptr;
        decoderDestroy = nullptr;
        decoderDecompressStream = nullptr;
        encoderCompress = nullptr;
        encoderMaxCompressedSize = nullptr;
    }

    bool LoadSet(const wchar_t* directory, DWORD flags) noexcept {
        common = LoadFromDirectory(directory, L"libbrotlicommon.dll", flags);
        decoder = LoadFromDirectory(directory, L"libbrotlidec.dll", flags);
        encoder = LoadFromDirectory(directory, L"libbrotlienc.dll", flags);
        if (decoder) {
            decoderCreate = reinterpret_cast<DecoderCreate>(GetProcAddress(decoder, "BrotliDecoderCreateInstance"));
            decoderDestroy = reinterpret_cast<DecoderDestroy>(GetProcAddress(decoder, "BrotliDecoderDestroyInstance"));
            decoderDecompressStream = reinterpret_cast<DecoderDecompressStream>(GetProcAddress(decoder, "BrotliDecoderDecompressStream"));
        }
        if (encoder) {
            encoderCompress = reinterpret_cast<EncoderCompress>(GetProcAddress(encoder, "BrotliEncoderCompress"));
            encoderMaxCompressedSize = reinterpret_cast<EncoderMaxCompressedSize>(GetProcAddress(encoder, "BrotliEncoderMaxCompressedSize"));
        }
        if (common && decoder && encoder && CanDecode() && CanEncode()) return true;
        ResetModules();
        return false;
    }
};

inline Api& GetApi() noexcept {
    static Api api;
    return api;
}

} // namespace detail

// BrotliEncoderCompress uses quality 11 (SmallestSize) and the Brotli default
// window size. Output bytes are the raw Brotli stream used by BrotliStream.
inline std::optional<std::vector<unsigned char>> CompressSmallest(std::span<const unsigned char> input) {
    auto& api = detail::GetApi();
    if (!api.CanEncode() || input.size() > static_cast<size_t>((std::numeric_limits<int>::max)()))
        return std::nullopt;
    size_t capacity = api.encoderMaxCompressedSize(input.size());
    if (capacity == 0 || capacity > static_cast<size_t>((std::numeric_limits<int>::max)()))
        return std::nullopt;
    std::vector<unsigned char> output(capacity);
    size_t written = capacity;
    static constexpr int qualitySmallest = 11;
    static constexpr int defaultWindow = 22;
    static constexpr int genericMode = 0;
    if (!api.encoderCompress(qualitySmallest, defaultWindow, genericMode,
            input.size(), reinterpret_cast<const uint8_t*>(input.data()), &written,
            reinterpret_cast<uint8_t*>(output.data())) || written > output.size())
        return std::nullopt;
    output.resize(written);
    return output;
}

// Decodes into a caller-bounded destination. Success requires one complete stream,
// exact output size, and no trailing input. `consumed` and `written` are set to the
// observed counts on every path where the decoder was successfully created.
inline bool DecompressExact(std::span<const unsigned char> input,
    std::span<unsigned char> output, size_t& consumed, size_t& written) {
    consumed = 0;
    written = 0;
    auto& api = detail::GetApi();
    if (!api.CanDecode()) return false;
    auto* state = api.decoderCreate(nullptr, nullptr, nullptr);
    if (!state) return false;

    const uint8_t* nextInput = reinterpret_cast<const uint8_t*>(input.data());
    size_t availableInput = input.size();
    uint8_t* nextOutput = reinterpret_cast<uint8_t*>(output.data());
    size_t availableOutput = output.size();
    size_t totalOutput = 0;
    int status = 2; // BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT
    for (;;) {
        const size_t beforeInput = availableInput;
        const size_t beforeOutput = availableOutput;
        status = api.decoderDecompressStream(state, &availableInput, &nextInput,
            &availableOutput, &nextOutput, &totalOutput);
        if (status == 1) { // BROTLI_DECODER_RESULT_SUCCESS
            consumed = input.size() - availableInput;
            written = totalOutput;
            const bool exact = availableInput == 0 && availableOutput == 0 &&
                totalOutput == output.size();
            api.decoderDestroy(state);
            return exact;
        }
        if (status == 0) { // BROTLI_DECODER_RESULT_ERROR
            consumed = input.size() - availableInput;
            written = totalOutput;
            api.decoderDestroy(state);
            return false;
        }
        // NEEDS_MORE_OUTPUT cannot be satisfied by the bounded destination. NEEDS_MORE_INPUT
        // with no remaining bytes is truncation. Guard against a non-progressing implementation.
        if ((status == 3 && availableOutput == 0) ||
            (status == 2 && availableInput == 0) ||
            (beforeInput == availableInput && beforeOutput == availableOutput)) {
            consumed = input.size() - availableInput;
            written = totalOutput;
            api.decoderDestroy(state);
            return false;
        }
    }
}

} // namespace HybridCpu::ShadowFrontend::BrotliCompat

#else
namespace HybridCpu::ShadowFrontend::BrotliCompat {
inline std::optional<std::vector<unsigned char>> CompressSmallest(std::span<const unsigned char>) {
    return std::nullopt;
}
inline bool DecompressExact(std::span<const unsigned char>, std::span<unsigned char>, size_t& consumed, size_t& written) {
    consumed = 0;
    written = 0;
    return false;
}
} // namespace HybridCpu::ShadowFrontend::BrotliCompat
#endif
