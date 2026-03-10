// Timely_MemModel.hpp
// 
// Version 0.0.1 built 20250706
//
// Memory Models of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include <cstdint>
#include <iomanip>  // For std::setprecision
#include <iostream>

#include "Timely_Define.hpp"
#include "Timely_Hash.hpp"
#include "Timely_Base.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {

    // Coverage Mode Class (Memory Model)
    // What is coverage: (in the same thread)
    // converage mode, means only when the destructor is called,
    // time counting is then stopped, leaving the function and entering into a 
    // recurrent or a nested function will NOT stop the time counting,
    // which is useful when you hope to calculate the overall time consumed on
    // the function.
    class _Timely_MemoryModel_Coverage : public _Timely_MemoryModel_Base {
    public:
        // (Shared) Is Heap Notation
        const static inline _BoolType _Is_Heap = false;

    protected:
        // (Kernel) Snapshot (Taking a snapshot of the memory model)
        virtual inline _SnapshotType __snapshot__() const noexcept override {
            // Nothing to report since we don't record anything within Coverage mode
            return _SnapshotType{"  Coverage Mode...\n"};
        }

        // (Kernel) Is Heap Runtime Identifier (Must Override)
        virtual inline _BoolType __is_heap_runtime__() const noexcept override {
            return this->_Is_Heap;
        }

    public:
        // Derive all methods as default, no override

    public:
        _Timely_MemoryModel_Coverage() noexcept : _Timely_MemoryModel_Base() {};
        virtual ~_Timely_MemoryModel_Coverage() noexcept {};
    };

    // Heaplike Mode Class (Memory Model)
    // What is heaplike: (in the same thread)
    // heaplike mode, means once leave the function and enter into another function,
    // the program will start to build up a heap (heap frame object), the previous function will be suspended
    // until the elements above the function in the heap are removed, then resume timing until calling a destructor.
    // It is useful in most of the cases.
    class _Timely_MemoryModel_Heaplike : public _Timely_MemoryModel_Base {
    public:
        // (Shared) Is Heap Notation
        const static inline _BoolType _Is_Heap = true;

    protected:
        // (Member) Thread Specific Heap-Stack Framework
        using _s_Heap_Stack_Frames = _HeapType<_PointerType>;
        _s_Heap_Stack_Frames _Heap_Invoke_System = { };

    protected:
        // (Kernel) Snapshot (Taking a snapshot of the memory model) (Override)
        virtual inline _SnapshotType __snapshot__() const noexcept override {
            // Create an empty snapshot
            _SnapshotType _snapshot{};

            // Create some format-related objects
            const static _SnapshotType __indicator = "> ";
            _SnapshotType _movlayers = "  --";

            // We know, Pointers are all instances derived from _Base_Timely_Implementation<...>
            const auto _size = this->_Heap_Invoke_System.size();
            for (_SizeType i = 0; i < _size; ++i) {
                // Get the invok
                const auto& invok = this->_Heap_Invoke_System[i];

                // Convert the pointer to a _Base_Timely_Implementation
                auto _ptr = reinterpret_cast<_Base_Timely_Implementation<0>*>(invok);
                const auto _inner = _ptr->snapshot_self__();

                // Calculate Space moving to alignment
                const auto _space = _StringType((_size - 1 - i) * 2, ' ');

                // Append to snapshot following a format
                _StringsteramType ss;
                if (i != _size - 1) {
                    // First, and others
                    ss << _movlayers << __indicator << _space << color4::dark_gray<_CharType> << _inner << std::endl << color4::reset<_CharType>;
                }
                else {
                    // Last Stack Element - Where the error is
                    ss << _movlayers << __indicator << _space << color4::underline<_CharType> << color4::red<_CharType> << _inner << std::endl << color4::reset<_CharType> << color4::style_reset<_CharType>;
                }
                _snapshot.append(ss.str());

                // Add more dashes for showing deeper layers
                _movlayers.append("--");
            }
            return _snapshot;
        }

        // (Kernel) Is Heap Runtime Identifier (Must Override)
        virtual inline _BoolType __is_heap_runtime__() const noexcept override {
            return this->_Is_Heap;
        }

    public:
        // Empty-ness of the heap
        virtual _BoolType empty() const noexcept override {
            return this->_Heap_Invoke_System.empty();
        }

        // Size of the heap
        virtual _SizeType size() const noexcept override {
            return this->_Heap_Invoke_System.size();
        }

        // Reserve the heap capacity
        virtual void reserve(const _SizeType& size) noexcept override {
            this->_Heap_Invoke_System.reserve(size);
            return;
        }

        // Inspect back
        virtual _PointerType inspect_back() const exceptional override {
            if (!this->_Heap_Invoke_System.empty()) {
                return this->_Heap_Invoke_System.back();
            }
            else {
                throw _Timely_Exception("_Timely_MemoryModel_Heaplike method inspect_back() is called while the heap is empty.");
            }
        }

        // Push back one element to the top of the heap
        virtual void push_back(const _PointerType& elem) noexcept override {
            this->_Heap_Invoke_System.push_back(elem);
            return;
        }

        // Pop back one element from the top of the heap
        virtual _PointerType pop_back() exceptional override {
            if (!this->_Heap_Invoke_System.empty()) {
                _HashType endelem = this->_Heap_Invoke_System.back();
                this->_Heap_Invoke_System.pop_back();
                return endelem;
            }
            else {
                throw _Timely_Exception("_Timely_MemoryModel_Heaplike method pop_back() is called while the heap is empty.");
            }
        }

    public:
        _Timely_MemoryModel_Heaplike() noexcept : _Timely_MemoryModel_Base() {};
        virtual ~_Timely_MemoryModel_Heaplike() noexcept {};
    };

}
// End namespace libnocppsh

