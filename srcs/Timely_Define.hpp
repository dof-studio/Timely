// Timely_Define.hpp
// 
// Version 0.0.1 built 20250706
//
// Macro and Definition of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <deque>
#include <vector>
#include <cstdlib>
#include <exception>
#include <algorithm>
#include <type_traits>
#include <unordered_map>

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {

	// Macro Timely Functionality ON-OFF (Useful)
	#ifndef __timelyon__
	#define __timelyon__                                     1
	#endif

	// Macro Max Thread Support (Useful)
	#ifndef __timelymaxthread__
	#define __timelymaxthread__                              512
	#endif
	// Note, Thread Available will be this number - 1
	// So, by default, you have Main Thread as index 0, and 510 other threads to use

	// Macro Timely Stats Verbosity (Useful)
	#ifndef __timelystatsverb__
	#define __timelystatsverb__                              2
	#endif

	// Macro Timely Stats Max Print Lines Per Thread (Useful)
	#ifndef __timelystatsmlns__
	#define __timelystatsmlns__                              26
	#endif

	// Macro Timely Stats Save to Local File (Useful)
	#ifndef __timelystatssave__
	#define __timelystatssave__                              0
	#endif

	// Macro Timely Stats HTML Style (Useful)
	#ifndef __timelystatstyle__
	#define __timelystatstyle__                             "light"
	#endif

	// Macro Version (DO NOT CHANGE)
	#ifndef __timelyvers__
	#define __timelyvers__                                  "0.0.1"
	#endif

    // Macro Reserved Global Protective Timelyh Object (DO NOT CHANGE)
	#ifndef __timelyhgblobjname__
	#define __timelyhgblobjname__                           "__+-Globalh-+__"
	#endif
	// This MUST exists in [0] thread of Single/Main Thread Timely Objects,
	// and
	// also MUST exists in [0] thread of Multiple Thread Timely Objects.

	// Macro Reserved Global Protective Timelyc Object (DO NOT CHANGE)
	#ifndef __timelycgblobjname__
	#define __timelycgblobjname__                           "__+-Globalc-+__"
	#endif
	// This MUST exists in [0] thread of Single/Main Thread Timely Objects,
	// and
	// also MUST exists in [0] thread of Multiple Thread Timely Objects.

	// Noexcept (may not throw exceptions) (DO NOT CHANGE)
	// noexcept

	// Exceptional (may throw exceptions) (DO NOT CHANGE)
	#ifndef exceptional
	#define exceptional
	#endif

	// Timely Exception
	class _Timely_Exception : public std::exception {
	private:
		// Stores message info 
		std::string _message;
		// Note: _Timely_Exception is the ONLY exception that can be thrown if a method or function
		//       is declared as exceptional in the entire Timely project.

	public:
		_Timely_Exception(const char* msg) noexcept : _message(msg) {}
		_Timely_Exception(const std::string& msg) noexcept : _message(msg) {}
		_Timely_Exception(std::string&& msg) noexcept : _message(std::move(msg)) {}

		const char* what() const noexcept override {
			return this->_message.c_str();
		}
	};

	// Timely Runtime Boolean
	class _Timely_RuntimeBoolean_Type {
	public:
		bool val = false;

	public:
		_Timely_RuntimeBoolean_Type() noexcept = default;
		_Timely_RuntimeBoolean_Type(bool _val) noexcept : val(_val) {}
		_Timely_RuntimeBoolean_Type(const _Timely_RuntimeBoolean_Type& _other) noexcept : val(_other.val) {}
		_Timely_RuntimeBoolean_Type(_Timely_RuntimeBoolean_Type&& _other) noexcept : val(_other.val) {}

		// Assignment
		inline _Timely_RuntimeBoolean_Type& operator=(const _Timely_RuntimeBoolean_Type& _other) noexcept {
			this->val = _other.val;
			return *this;
		}
		inline _Timely_RuntimeBoolean_Type& operator=(_Timely_RuntimeBoolean_Type&& _other) noexcept {
			this->val = _other.val;
			return *this;
		}

		// Comparision 
		inline bool operator==(bool _val) const noexcept {
			return this->val == _val;
		}
		inline bool operator==(const _Timely_RuntimeBoolean_Type& _other) const noexcept {
			return this->val == _other.val;
		}
		inline bool operator==(_Timely_RuntimeBoolean_Type&& _other) const noexcept {
			return this->val == _other.val;
		}
	};

	// Timely Initialization Ordinary (Ordinarily Initialization)
	class _Timely_Init_Ordinary {};

	// Timely Initialization Deferrer (Init Without Calling __init__, And Without __exit__)
	class _Timely_Init_Deferrer {};

	// Bool Type
	typedef                                     bool        _BoolType;

	// IntegerType
	typedef                         signed long long        _IntegerType;

	// ThreadIdType
	typedef                         signed long long        _ThreadIdType;

	// IndexType
	typedef                       unsigned long long        _IndexType;

	// Size Type
	typedef                       unsigned long long        _SizeType;

	// Pointer Type
	typedef                       unsigned long long        _PointerType;

	// Number Type
	typedef                                   double        _NumberType;

	// Char Type
	typedef                                     char        _CharType;

	// String Type
	typedef                              std::string        _StringType;

	// Stringstream Type
	typedef                        std::stringstream        _StringsteramType;

	// Snapshot Type
	typedef                              std::string        _SnapshotType;

	// Vector Type
	template <typename T>
	using                                _VectorType       = std::vector<T>;

	// Pair Type
	template <typename F, typename S>
	using                                  _PairType       = std::pair<F, S>;

	// Mapping Type
	template <typename K, typename V>
	using                               _MappingType       = std::unordered_map<K, V>;

	// Heap Type
	template <typename T>
	using                                  _HeapType       = std::vector<T>;

	// Time Type
	using                                  _TimeType       = std::chrono::high_resolution_clock::time_point;

	// Duration Type
	using                                  _DuraType       = std::chrono::duration<double>;

	// Hash Type
	using                                  _HashType       = unsigned long long;

	// External Thread ID Type
	using                           _ExtThreadIDType       = std::thread::id;

	// Atomic Wrapper
	template <typename T>
	using                             _AtomicWrapper       = std::atomic<T>;

	// Main Thread Thread ID
	const static _ExtThreadIDType _MainThreadIdentifier    = std::this_thread::get_id();

	// External Thread ID Getter
	template <typename PlaceHolder = void>
	inline _ExtThreadIDType            _ThisThreadId       () noexcept {
		return std::this_thread::get_id();
	}

	// STD Anything To String API
	inline _StringType                 _ToStringType       (_NumberType strlike, _SizeType digits = 14) noexcept{
		_StringsteramType ss;
		ss << std::fixed << std::setprecision(digits) << strlike;
		return ss.str();
	}
	template <typename _AnyType>
	inline _StringType                 _ToStringType       (_AnyType strlike) noexcept {
		return std::to_string(strlike);
	}
}
// End namespace libnocppsh
