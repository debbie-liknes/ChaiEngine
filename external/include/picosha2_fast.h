/*
picosha2_fast.h

A trimmed-down, non-templated SHA-256 implementation derived from PicoSHA2
(https://github.com/okdshin/PicoSHA2, MIT licensed, Copyright (C) 2017 okdshin).

This version is specialized for hashing files/binaries from disk and avoids:
  - std::istreambuf_iterator (per-character iterator, very slow under
    _ITERATOR_DEBUG_LEVEL / checked iterators on MSVC)
  - std::back_insert_iterator into std::vector (per-byte checked push_back)
  - template dispatch over arbitrary iterator categories
  - std::ostream-based hex formatting

Instead it reads the file in large chunks with std::ifstream::read() and
processes raw byte buffers directly, which is dramatically faster in debug
builds with iterator debugging enabled, and slightly faster even in release
builds.

Same license as the original (MIT):

The MIT License (MIT)

Copyright (C) 2017 okdshin
Modifications Copyright (C) 2026

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef PICOSHA2_FAST_H
#define PICOSHA2_FAST_H

#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef PICOSHA2_FAST_FILE_BUFFER_SIZE
#define PICOSHA2_FAST_FILE_BUFFER_SIZE (1 << 20)  // 1 MB read chunks
#endif

namespace picosha2 {

using byte_t = unsigned char;
inline constexpr size_t k_digest_size = 32;

namespace detail {

inline constexpr uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

inline uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

// Processes exactly one 64-byte block, updating state in place.
inline void sha256_transform(uint32_t state[8], const uint8_t block[64]) {
    uint32_t w[64];
    for (int i = 0; i < 16; ++i) {
        w[i] = (uint32_t(block[i * 4]) << 24) |
               (uint32_t(block[i * 4 + 1]) << 16) |
               (uint32_t(block[i * 4 + 2]) << 8) |
               (uint32_t(block[i * 4 + 3]));
    }
    for (int i = 16; i < 64; ++i) {
        uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t e = state[4], f = state[5], g = state[6], h = state[7];

    for (int i = 0; i < 64; ++i) {
        uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = h + S1 + ch + k[i] + w[i];
        uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = S0 + maj;
        h = g; g = f; f = e; e = d + temp1;
        d = c; c = b; b = a; a = temp1 + temp2;
    }

    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

}  // namespace detail

// Incremental SHA-256 hasher operating on raw byte buffers (no iterators).
class hasher {
   public:
    hasher() { reset(); }

    void reset() {
        state_[0] = 0x6a09e667; state_[1] = 0xbb67ae85;
        state_[2] = 0x3c6ef372; state_[3] = 0xa54ff53a;
        state_[4] = 0x510e527f; state_[5] = 0x9b05688c;
        state_[6] = 0x1f83d9ab; state_[7] = 0x5be0cd19;
        buffer_len_ = 0;
        total_len_ = 0;
        finished_ = false;
    }

    // Feed len bytes starting at data. Can be called repeatedly with
    // arbitrarily sized chunks (e.g. straight from ifstream::read).
    void update(const uint8_t* data, size_t len) {
        total_len_ += len;
        raw_update(data, len);
    }

    // Finalizes the hash and returns the 32-byte digest. Do not call
    // update() again after this without calling reset() first.
    std::array<byte_t, k_digest_size> finish() {
        if (finished_) {
            throw std::logic_error("picosha2::hasher::finish() called twice; call reset() first");
        }
        finished_ = true;

        uint64_t bit_len = total_len_ * 8;

        uint8_t pad = 0x80;
        raw_update(&pad, 1);

        static const uint8_t zero = 0;
        while (buffer_len_ != 56) {
            raw_update(&zero, 1);
        }

        uint8_t len_bytes[8];
        for (int i = 0; i < 8; ++i) {
            len_bytes[i] = static_cast<uint8_t>(bit_len >> (56 - 8 * i));
        }
        raw_update(len_bytes, 8);

        std::array<byte_t, k_digest_size> digest{};
        for (int i = 0; i < 8; ++i) {
            digest[i * 4 + 0] = static_cast<byte_t>(state_[i] >> 24);
            digest[i * 4 + 1] = static_cast<byte_t>(state_[i] >> 16);
            digest[i * 4 + 2] = static_cast<byte_t>(state_[i] >> 8);
            digest[i * 4 + 3] = static_cast<byte_t>(state_[i]);
        }
        return digest;
    }

   private:
    // Buffers/transforms bytes WITHOUT touching total_len_ (used both by
    // update() and internally by finish() for padding, where the length
    // must not be recounted).
    void raw_update(const uint8_t* data, size_t len) {
        if (buffer_len_ > 0) {
            size_t fill = 64 - buffer_len_;
            size_t n = (fill < len) ? fill : len;
            std::memcpy(buffer_ + buffer_len_, data, n);
            buffer_len_ += n;
            data += n;
            len -= n;
            if (buffer_len_ == 64) {
                detail::sha256_transform(state_, buffer_);
                buffer_len_ = 0;
            }
        }
        while (len >= 64) {
            detail::sha256_transform(state_, data);
            data += 64;
            len -= 64;
        }
        if (len > 0) {
            std::memcpy(buffer_, data, len);
            buffer_len_ = len;
        }
    }

    uint32_t state_[8];
    uint8_t buffer_[64];
    size_t buffer_len_;
    uint64_t total_len_;
    bool finished_;
};

// Fast hex encoding via lookup table (no ostream formatting overhead).
inline std::string bytes_to_hex_string(const byte_t* data, size_t len) {
    static const char* hex_chars = "0123456789abcdef";
    std::string result(len * 2, '\0');
    for (size_t i = 0; i < len; ++i) {
        result[i * 2] = hex_chars[data[i] >> 4];
        result[i * 2 + 1] = hex_chars[data[i] & 0x0F];
    }
    return result;
}

inline std::string bytes_to_hex_string(const std::array<byte_t, k_digest_size>& digest) {
    return bytes_to_hex_string(digest.data(), digest.size());
}

// Hashes an already-open binary ifstream (from current position to EOF)
// by reading in large chunks, rather than one byte at a time.
inline std::array<byte_t, k_digest_size> hash256(std::ifstream& f) {
    if (!f) {
        throw std::runtime_error("picosha2::hash256: input stream is not open/valid");
    }
    hasher h;
    std::vector<uint8_t> buffer(PICOSHA2_FAST_FILE_BUFFER_SIZE);
    while (f.read(reinterpret_cast<char*>(buffer.data()),
                  static_cast<std::streamsize>(buffer.size())) ||
           f.gcount() > 0) {
        auto n = static_cast<size_t>(f.gcount());
        if (n == 0) break;
        h.update(buffer.data(), n);
    }
    return h.finish();
}

// Convenience: hash a file on disk directly, given its path.
inline std::array<byte_t, k_digest_size> hash256(const std::filesystem::path& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        throw std::runtime_error("picosha2::hash256: failed to open file: " + path.string());
    }
    return hash256(f);
}

// Convenience: hash a file on disk and return the hex digest directly.
inline std::string hash256_hex_string(const std::filesystem::path& path) {
    return bytes_to_hex_string(hash256(path));
}

// Convenience: hash an already-open ifstream and return the hex digest.
inline std::string hash256_hex_string(std::ifstream& f) {
    return bytes_to_hex_string(hash256(f));
}

// Hash an in-memory buffer (e.g. std::string / std::vector<byte_t> content).
inline std::array<byte_t, k_digest_size> hash256_bytes(const void* data, size_t len) {
    hasher h;
    h.update(static_cast<const uint8_t*>(data), len);
    return h.finish();
}

inline std::string hash256_hex_string_bytes(const void* data, size_t len) {
    return bytes_to_hex_string(hash256_bytes(data, len));
}

}  // namespace picosha2

#endif  // PICOSHA2_FAST_H