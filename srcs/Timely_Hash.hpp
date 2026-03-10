// Timely_Hash.hpp
// 
// Version 0.0.1 built 20250706
//
// Hash Library of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include "Timely_Define.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {

    // Interface Quick Compile Time Hash for C-string
    inline constexpr _HashType _Timely_Hash(const _CharType* str) noexcept {

        constexpr _HashType seed1 = 0xC6A4A7935BD1E995ULL; // FNV-inspired prime
        constexpr _HashType seed2 = 0x9E3779B97F4A7C15ULL; // Golden ratio prime
        constexpr _HashType seed3 = 0xCE5E8A2D1B4F7C09ULL; // Another large prime
        _HashType hash = seed1;
        _SizeType length = 0;

        // FNV-1a-like pass with enhanced mixing
        for (_SizeType i = 0; str[i] != '\0'; ++i) {
            hash ^= static_cast<uint8_t>(str[i]);
            hash *= 1099511628211ULL;           // FNV prime
            hash = (hash << 13) | (hash >> 51); // Rotate left by 13
            length++;
        }

        // Incorporate length and second seed with non-linear mixing
        hash ^= length ^ seed2;
        hash *= 0xA5A5A5A5A5A5A5A5ULL; // Alternating bit pattern
        hash ^= (hash >> 23) ^ (hash << 17); // Dual-direction shift and XOR

        // Multi-step bit diffusion
        hash += seed3;
        hash ^= hash >> 29;
        hash *= 0xFF51AFD7ED558CCDULL; // MurmurHash3 constant
        hash ^= hash >> 31;
        hash *= 0xC4CEB9FE1A85EC53ULL; // MurmurHash3 constant
        hash ^= hash >> 27;

        // Extra complexity with modular arithmetic and rotations
        hash = (hash ^ (hash >> 37)) * 0xDB4F0A2C6E9D5B71ULL;
        hash = (hash << 19) | (hash >> 45); // Rotate left by 19
        hash ^= (hash >> 41) + length;
        hash *= 0xB2D3E4F5A6C7B891ULL; // Custom large prime

        // Thorough mixing inspired by cryptographic principles
        hash ^= hash >> 33;
        hash *= 0xE9B5DBA58189DBB5ULL;
        hash ^= hash >> 25;
        hash *= 0xF1C9923D5E7F8A4BULL;
        hash ^= hash >> 47;
        hash *= 0xA1B2C3D4E5F60789ULL;
        hash ^= hash >> 53;

        return hash;
    }

    // Kernel Quick Compile Time Hash for Iterative C-string
    inline constexpr _HashType __In_Timely_Hash_Recursive(_HashType state, _SizeType length, const _CharType* str) noexcept {

        constexpr _HashType seed1 = 0xC6A4A7935BD1E995ULL; // FNV-inspired prime
        constexpr _HashType seed2 = 0x9E3779B97F4A7C15ULL; // Golden ratio prime
        constexpr _HashType seed3 = 0xCE5E8A2D1B4F7C09ULL; // Another large prime
        _HashType hash = (length != 0) ? state : seed1;
        // length == 0 meaning the 1st round

        // FNV-1a-like pass with enhanced mixing
        for (_SizeType i = 0; str[i] != '\0'; ++i) {
            hash ^= static_cast<uint8_t>(str[i]);
            hash *= 1099511628211ULL;           // FNV prime
            hash = (hash << 13) | (hash >> 51); // Rotate left by 13
            length++;
        }

        // Terminated

        // Incorporate length and second seed with non-linear mixing
        hash ^= length ^ seed2;
        hash *= 0xA5A5A5A5A5A5A5A5ULL; // Alternating bit pattern
        hash ^= (hash >> 23) ^ (hash << 17); // Dual-direction shift and XOR

        // Multi-step bit diffusion
        hash += seed3;
        hash ^= hash >> 29;
        hash *= 0xFF51AFD7ED558CCDULL; // MurmurHash3 constant
        hash ^= hash >> 31;
        hash *= 0xC4CEB9FE1A85EC53ULL; // MurmurHash3 constant
        hash ^= hash >> 27;

        // Extra complexity with modular arithmetic and rotations
        hash = (hash ^ (hash >> 37)) * 0xDB4F0A2C6E9D5B71ULL;
        hash = (hash << 19) | (hash >> 45); // Rotate left by 19
        hash ^= (hash >> 41) + length;
        hash *= 0xB2D3E4F5A6C7B891ULL; // Custom large prime

        // Thorough mixing inspired by cryptographic principles
        hash ^= hash >> 33;
        hash *= 0xE9B5DBA58189DBB5ULL;
        hash ^= hash >> 25;
        hash *= 0xF1C9923D5E7F8A4BULL;
        hash ^= hash >> 47;
        hash *= 0xA1B2C3D4E5F60789ULL;
        hash ^= hash >> 53;

        return hash;
    }
    template<typename _Ty, typename... _Rest>
    requires std::is_same_v<_Ty, const _CharType*>
    inline constexpr _HashType __In_Timely_Hash_Recursive(_HashType state, _SizeType length, _Ty str, _Rest... rest) noexcept {
        
        constexpr _HashType seed1 = 0xC6A4A7935BD1E995ULL; // FNV-inspired prime
        constexpr _HashType seed2 = 0x9E3779B97F4A7C15ULL; // Golden ratio prime
        constexpr _HashType seed3 = 0xCE5E8A2D1B4F7C09ULL; // Another large prime
        _HashType hash = (length != 0) ? state : seed1;
        _SizeType len = (length != 0) ? length : 0;
        // length == 0 meaning the 1st round

        // FNV-1a-like pass with enhanced mixing
        for (_SizeType i = 0; str[i] != '\0'; ++i) {
            hash ^= static_cast<uint8_t>(str[i]);
            hash *= 1099511628211ULL;           // FNV prime
            hash = (hash << 13) | (hash >> 51); // Rotate left by 13
            len++;
        }

        return __In_Timely_Hash_Recursive(hash, len, rest...);
    }
    
    // Interface Quick Compile Time Hash for Iterative C-string
    template<typename _Ty, typename... _Rest>
    requires std::is_same_v<_Ty, const _CharType*>
    inline constexpr _HashType _Timely_Hash(_Ty str, _Rest... rest) noexcept {
        return __In_Timely_Hash_Recursive(0, 0, str, rest...);
    }

    // Interface Quick Compile Time Hash for C++ string - deleted
    inline constexpr _HashType _Timely_Hash(const _StringType& str) noexcept = delete;
    template<typename _Ty, typename... _Rest>
    requires std::is_same_v<_Ty, _StringType>
    inline constexpr _HashType _Timely_Hash(_Ty str, _Rest... rest) noexcept = delete;
    // We only support C-string that can be hash at compile time.
    // So, from the given version on, we deleted the C++ string version.

    // Macro Reserved Global Protective Timelyh Object Hash
    constexpr _HashType __timelyhgblobjhash__ = _Timely_Hash(__timelyhgblobjname__);

    // Macro Reserved Global Protective Timelyc Object Hash
    constexpr _HashType __timelycgblobjhash__ = _Timely_Hash(__timelycgblobjname__);
}
// End namespace libnocppsh
