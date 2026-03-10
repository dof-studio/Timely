// Timely_Impl.hpp
// 
// Version 0.0.1 built 20250706
//
// Core Implementation of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include <cstdint>
#include <iomanip>  // For std::setprecision
#include <iostream>
#include <shared_mutex>

#include "Color4/color4"

#include "Timely_Define.hpp"
#include "Timely_Hash.hpp"
#include "Timely_Base.hpp"
#include "Timely_Stats.hpp"
#include "Timely_MemModel.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {

    // Prototype Single Thread Timely
    // What is the Single Thread Timely?
    // It is a single-thread (unchecked), fully implemented Timely class,
    // which starts to count when a Timely object is created,
    // pauses to count when jumps out of the current function (heap only),
    // resumes to count when jumps in again into the current function (heap only),
    // register the time elapsed in destructor.
    template<_ThreadIdType _ThreadId = 0, class _Timely_Memory_Module = _Timely_MemoryModel_Heaplike, class _Timely_Init_Controller = _Timely_Init_Ordinary>
    class _Prototype_SingleThread_Timely : public _Base_Timely_Implementation<_ThreadId> {

    public:
        // Thread Id Must be Less Or Equal To Maximum Thread Number
        static_assert(_ThreadId >= 0 && _ThreadId < __timelymaxthread__, "Thread Id For Any Timely Related Instance Must NOT Be Greater Than Or Equal To Global Max Thread Reserved. Moreover, if using -1, please consider Multi-Thread Models.");
        // Value  0 is reserved for Main Thread that is used by Single-Thread and Main-Thread Model
        // Value -1 is reserved for Automatic Thread Deduction mode in Multi-Thread Timely Models
        //          but is only supported by Multi-Thread Auto Model
        // Other positive values are reserved for Multi-Thread Timely Models

        // Timely Memory Module can only be: Coverage, Heaplike
        static_assert(std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Coverage> || std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>, "Timely Memory Module MUST be either Coverage or Heaplike.");

        // Timely Init Controller can only be: Ordinary, Deferrer
        static_assert(std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary> || std::is_same_v<_Timely_Init_Controller, _Timely_Init_Deferrer>, "Timely Init Controller MUST be either Ordinary or Deferrer.");

    protected:
        // This type
        using _s_ThisType = _Prototype_SingleThread_Timely<_ThreadId, _Timely_Memory_Module, _Timely_Init_Controller>;
        
        // Base type
        using _s_BaseType = _Base_Timely_Implementation<_ThreadId>;

        // (Member) Shared Memory Module
        using _s_Memory_Module_Type = _Timely_Memory_Module;
        inline static _s_Memory_Module_Type _Shared_Timely_Memory_Module;

    protected:
        // *(Kernel) Thread Number Retriever (Virtual) (Must be Override Each Time)
        inline virtual _SizeType __threadid__() const noexcept override {
            return _ThreadId; // Generic Approach, to use _ThreadId consistently in SingleThread Model
        }

        // *(Kernel) Thread Number Retriever In Runtime (Virtual) (Must be Override Each Time)
        inline virtual _SizeType __threadid_runtime__() const noexcept override {
            return _ThreadId; // Generic Approach, to use _ThreadId consistently in SingleThread Model
        }

        // *(Kernel) Is Heap Mode (Virtual) (Must Override)
        inline virtual constexpr _BoolType __is_heap__() const noexcept override {
            return std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>;
        }

        // *(Kernel) Is Heap Mode Detected In Runtime (Virtual) (Must Override)
        inline virtual _BoolType __is_heap_runtime__() const noexcept override {
            return this->__mem_module__().is_heap_runtime();
        }

        // *(Kernel) Is Ordinary Controller (Virtual) (Must Override)
        inline virtual constexpr _BoolType __is_ordinary__() const noexcept override {
            return std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary>;
        }

        // *(Kernel) Get Reference of Currently Used Memory Module
        inline virtual _Timely_Memory_Module& __mem_module__() noexcept {
            return this->_Shared_Timely_Memory_Module;
        }

        // *(Kernel) Get Const Reference of Currently Used Memory Module
        inline virtual const _Timely_Memory_Module& __mem_module__() const noexcept {
            return this->_Shared_Timely_Memory_Module;
        }

        // *(Kernel) Register the Instance to Heap (Virtual) (Must Override)
        inline virtual void __register_heap__() noexcept override {
            if constexpr (std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>) {
                // .push_back() is always harmless
                this->__mem_module__().push_back(reinterpret_cast<_PointerType>(this));
            }
            return;
        }

        // *(Kernel) Popout the Last Instance from Heap (Virtual) (Must Override)
        inline virtual _PointerType __popout_heap__() noexcept override {
            if constexpr (std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>) {
                return this->__mem_module__().pop_back();
            }
            else {
                return NULL;
            }
        }

        // *(Kernel) Manually Pause Latest Instance if Any and if Latest is heap (Virtual) (Must Override)
        inline virtual void __pause_latest_ifheap__() noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].size();

            // If having at least one, pause
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].at(_size - 1));
                // Execute __pause__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->pause();
                }
            }
            return;
        }

        // *(Kernel) Manually Resume Latest Instance if Any and if Latest is heap (Virtual) (Must Override)
        inline virtual void __resume_latest_ifheap__() noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].size();

            // If having at least one, resume
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].at(_size - 1));
                // Execute __resume__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->resume();
                }
            }
            return;
        }

        // *(Kernel) Manually Pause Latest Instance if Any and if Latest is heap - Given Thread Number (Virtual) (Must Override)
        inline virtual void __pause_latest_ifheap__(_ThreadIdType _DynamicTid) noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_DynamicTid].size();

            // If having at least one, pause
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_DynamicTid].at(_size - 1));
                // Execute __pause__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->pause();
                }
            }
            return;
        }

        // *(Kernel) Manually Resume Latest Instance if Any and if Latest is heap - Given Thread Number (Virtual) (Must Override)
        inline virtual void __resume_latest_ifheap__(_ThreadIdType _DynamicTid) noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_DynamicTid].size();

            // If having at least one, resume
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_DynamicTid].at(_size - 1));
                // Execute __resume__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->resume();
                }
            }
            return;
        }

        // *(Kernel) Stat Pipeline for very last instance (Virtual) (Must Override)
        inline virtual void __stat_pipeline_vl__() const noexcept override {
            // Use stats tool to do everything
            _Prototype_Timely_StatModels _stats(this->get_prjname(), this->_Shared_Performance_Table);
            _stats.gen(__timelystatssave__, __timelystatsverb__, __timelystatstyle__);
            return;
        }

        // *(Kernel) Inspect A Snapshot of Memory Module (Virtual) (Must Override)
        inline virtual _SnapshotType __snapshot__() const noexcept override {
            _StringsteramType ss;
            ss << "  Single Thread Snapshot for Project [" << color4::green<_CharType> << this->get_prjname() << color4::reset<_CharType> << "] Thread (" << color4::green<_CharType> << ((_ThreadId == 0) ? "Main" : _ToStringType(_ThreadId)) << color4::reset<_CharType> << ")" << std::endl;
            const _SnapshotType _SingleThread_Prefix = ss.str();
            return _SingleThread_Prefix + this->__mem_module__().snapshot();
        }

    public:
        // Constructor
        explicit _Prototype_SingleThread_Timely() noexcept = delete;
        _Prototype_SingleThread_Timely(const char* sigstr) noexcept :
            _s_BaseType((this->__pause_latest_ifheap__(), _ThreadId), sigstr)
        {
            // If Non-Deferrer Controller, Call Init/Exit
            if constexpr (std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary>) {
                this->__init__();
            }
            
            return;
        }
        _Prototype_SingleThread_Timely(const char* sigstr, _HashType sighash) noexcept :
            _s_BaseType((this->__pause_latest_ifheap__(), _ThreadId), sigstr, sighash)
        {
            // If Non-Deferrer Controller, Call Init/Exit
            if constexpr (std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary>) {
                this->__init__();
            }

            return;
        }
        explicit _Prototype_SingleThread_Timely(const _StringType& sigstr) noexcept = delete;
        explicit _Prototype_SingleThread_Timely(const _StringType& sigstr, _HashType sighash) noexcept = delete;

        // Destructor
        virtual ~_Prototype_SingleThread_Timely() noexcept {

            // If Non-Deferrer Controller, Call Init/Exit
            if constexpr (std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary>) {
                this->__exit__();
            }
            return;
        };
    };

    // Prototype Multiple Thread Timely
    //
    template<_ThreadIdType _ThreadId, class _Timely_Memory_Module = _Timely_MemoryModel_Heaplike, class _Timely_Init_Controller = _Timely_Init_Ordinary>
    class _Prototype_MultipleThread_Timely : public _Base_Timely_Implementation<_ThreadId> {
    
    public:
        // Thread Id Must be Less Or Equal To Maximum Thread Number
        static_assert(_ThreadId >= -1 && _ThreadId < __timelymaxthread__, "Thread Id For Any Timely Related Instance Must NOT Be Greater Than Or Equal To Global Max Thread Reserved. Moreover, if using -1, please consider Multi-Thread Models.");
        // Value  0 is reserved for Main Thread that is used by Single-Thread and Main-Thread Model
        // Value -1 is reserved for Automatic Thread Deduction mode in Multi-Thread Timely Models
        //          but is only supported by Multi-Thread Auto Model
        // Other positive values are reserved for Multi-Thread Timely Models

        // Timely Memory Module can only be: Coverage, Heaplike
        static_assert(std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Coverage> || std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>, "Timely Memory Module MUST be either Coverage or Heaplike.");

        // *** This version specific *** We require _ThreadId must be 0 or -1
        static_assert(_ThreadId == 0 || _ThreadId == -1, "Multi-Thread Timely Model must be assigned with _ThreadId = 0 or _ThreadId = -1 in this version");

    protected:
        // This type
        using _s_ThisType = _Prototype_MultipleThread_Timely<_ThreadId, _Timely_Memory_Module, _Timely_Init_Controller>;

        // Base type
        using _s_BaseType = _Base_Timely_Implementation<_ThreadId>;

        // (Shared) External Thread Id and Index Version Thread Id (used by Timely) Mapping
        using _s_TidConversionType = _MappingType<_ExtThreadIDType, _ThreadIdType>;
        static inline _s_TidConversionType _Shared_ThreadId_Index_Mapping;
        // Key: External Thread Id getting by _ThisThreadId(), type _ExtThreadIDType
        // Val: Internal Thread Id Index, used by Timely Vector Data Structures, type _ThreadIdType
        // Reserved: Always reserve the Main Thread External Id

        // (Member) Shared Mapping Mutex
        using _s_MultiThread_Mapping_SharedMutex = std::shared_mutex;
        inline static _s_MultiThread_Mapping_SharedMutex _Shared_Mapping_Mutex;

        // (Member) Shared Memory Module
        using _s_MultiThread_Memory_Module_Type = _VectorType<_Timely_Memory_Module>;
        inline static _s_MultiThread_Memory_Module_Type _Shared_MultiThread_Timely_Memory_Module { __timelymaxthread__ };
        // Although we will reserve a place for Main Thread and Pulling Main Thread Object to here
        // is allowed, please note this will be separately handled than the SingleThread implementation.
        // So, please be consistent in terms of using Timely objects in Main Thread.

        // (Member) External Thread Id (May Be Invalid), only for tmp use DO NOT USE THIS
        _ExtThreadIDType _ExternalThreadId;
        // This one will NEVER be used after initialization

    private:
        // *(Kernel) Insert a new thread id mapping (Write)
        inline void __insert_new_index_mapping__(_ExtThreadIDType _ext, _ThreadIdType _int) noexcept {
            // Write access
            this->_Shared_Mapping_Mutex.lock(); // Use lock() / unlock() for write (exclusive) access.

            // Try to reserve a big number
            this->_Shared_ThreadId_Index_Mapping.reserve(__timelymaxthread__ * 2);

            // Register a new mapping 
            this->_Shared_ThreadId_Index_Mapping.insert(std::make_pair(_ext, _int));

            this->_Shared_Mapping_Mutex.unlock();
            return;
        }

        // *(Kernel) Dynamically Maintain Thread Id Index Mapping (Read)
        inline _ThreadIdType __maintain_threadid_index_mapping__() exceptional {
            // Check the Internal/External Mapping table and convert the dynamic id to internal id if needed

            // Returns Dynamic Thread Id

            this->_ExternalThreadId = _ThisThreadId();

            // Read access
            this->_Shared_Mapping_Mutex.lock_shared(); // Use lock_shared() / unlock_shared() for read-only (shared) access.

            // If not specified, check and assign
            if constexpr (_ThreadId == -1) {

                const _ThreadIdType _size = this->_Shared_ThreadId_Index_Mapping.size();

                // If equals to the reserved number, then error
                if (_size >= __timelymaxthread__) {
                    throw _Timely_Exception("_Prototype_MultipleThread_Timely method __maintain_threadid_index_mapping__() is called with while the reserved thread is used up. Try use fewer threads or raise the maximum limit");
                }

                // Try finding the external thread id
                const auto _itr = this->_Shared_ThreadId_Index_Mapping.find(this->_ExternalThreadId);

                // If not found, assign this id to a new thread number
                if (_itr == this->_Shared_ThreadId_Index_Mapping.end()) {
                    // If is main, then 0, otherwise, size + 1
                    this->_Runtime_ThreadId = (this->_ExternalThreadId == _MainThreadIdentifier) ? 0 : _size + 1;
                    
                    // Need exclusive access to register a new one
                    this->_Shared_Mapping_Mutex.unlock_shared();
                    this->__insert_new_index_mapping__(this->_ExternalThreadId, this->_Runtime_ThreadId);                  
                    
                    // No need to unlock_shared
                    return this->_Runtime_ThreadId;
                }

                // If already found, retrieve the existing internal id
                else {
                    this->_Runtime_ThreadId = (*_itr).second;
                }
            }

            // If specified, check if registered the specified thread id
            else {
                // Assign Dynamic Thread Id as _ThreadId
                this->_Runtime_ThreadId = _ThreadId;

                // If not found, assign this id to a new thread number
                const auto _itr = this->_Shared_ThreadId_Index_Mapping.find(this->_ExternalThreadId);

                if (_itr == this->_Shared_ThreadId_Index_Mapping.end()) {
                    // Need exclusive access
                    this->_Shared_Mapping_Mutex.unlock_shared();
                    this->__insert_new_index_mapping__(this->_ExternalThreadId, _ThreadId);
                    // No need to unlock_shared
                    return this->_Runtime_ThreadId;
                }

            }

            // Release shared_unlock
            this->_Shared_Mapping_Mutex.unlock_shared();

            return this->_Runtime_ThreadId;
        }

        // *(Hybrid Kernel) Dynamically Maintain Thread Id Index Mapping (Read) and Previous Latest one If Heap
        inline _ThreadIdType __maintain_threadid_index_mapping_and_pause_latest_ifheap__() exceptional {
            // Why do we have to do this?
            //
            // We have to pause the latest with a given dynamically defined thread id.
            // This step can only be done once the dynamic id is determined.
            // And we hope we can as quickly as we can to pause the previous one
            // to avoid time consumption of construction that may impact normal timing.
            // So instead of doing this in constructors, we do it now.
            
            const _ThreadIdType _DynamicId = _s_ThisType::__maintain_threadid_index_mapping__();
            _s_ThisType::__pause_latest_ifheap__(_DynamicId);
            return _DynamicId;
        }

    protected:
        // *(Kernel) Thread Number Retriever (Virtual) (Must be Override Each Time)
        inline virtual _SizeType __threadid__() const noexcept override {
            return this->_Runtime_ThreadId; // Return the dynamic version
        }

        // *(Kernel) Thread Number Retriever In Runtime (Virtual) (Must be Override Each Time)
        inline virtual _SizeType __threadid_runtime__() const noexcept override {
            return this->_Runtime_ThreadId; // Return the dynamic version
        }

        // *(Kernel) Is Heap Mode (Virtual) (Must Override)
        inline virtual constexpr _BoolType __is_heap__() const noexcept override {
            return std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>;
        }

        // *(Kernel) Is Heap Mode Detected In Runtime (Virtual) (Must Override)
        inline virtual _BoolType __is_heap_runtime__() const noexcept override {
            return this->__mem_module__().is_heap_runtime();
        }

        // *(Kernel) Is Ordinary Controller (Virtual) (Must Override)
        inline virtual constexpr _BoolType __is_ordinary__() const noexcept override {
            return false; // This is by default false, we used deferrer controller
        }

        // *(Kernel) Get Reference of Currently Used Memory Module
        inline virtual _Timely_Memory_Module& __mem_module__() noexcept {
            return this->_Shared_MultiThread_Timely_Memory_Module[this->__threadid__()];
        }

        // *(Kernel) Get Const Reference of Currently Used Memory Module
        inline virtual const _Timely_Memory_Module& __mem_module__() const noexcept {
            return this->_Shared_MultiThread_Timely_Memory_Module[this->__threadid__()];
        }

        // *(Kernel) Register the Instance to Heap (Virtual) (Must Override)
        inline virtual void __register_heap__() noexcept override {
            if constexpr (std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>) {
                // .push_back() is always harmless
                this->__mem_module__().push_back(reinterpret_cast<_PointerType>(this));
            }
            return;
            // SAME AS SINGLE VERSION
        }

        // *(Kernel) Popout the Last Instance from Heap (Virtual) (Must Override)
        inline virtual _PointerType __popout_heap__() noexcept override {
            if constexpr (std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>) {
                return this->__mem_module__().pop_back();
            }
            else {
                return NULL;
            }
            // SAME AS SINGLE VERSION
        }

        // *(Kernel) Manually Pause Latest Instance if Any and if Latest is heap (Virtual) (Must Override)
        inline virtual void __pause_latest_ifheap__() noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].size();

            // If having at least one, pause
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].at(_size - 1));
                // Execute __pause__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->pause();
                }
            }
            return;
            // SAME AS SINGLE VERSION
        }

        // *(Kernel) Manually Resume Latest Instance if Any and if Latest is heap (Virtual) (Must Override)
        inline virtual void __resume_latest_ifheap__() noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].size();

            // If having at least one, resume
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_s_ThisType::__threadid__()].at(_size - 1));
                // Execute __resume__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->resume();
                }
            }
            return;
            // SAME AS SINGLE VERSION
        }

        // *(Kernel) Manually Pause Latest Instance if Any and if Latest is heap - Given Thread Number (Virtual) (Must Override)
        inline virtual void __pause_latest_ifheap__(_ThreadIdType _DynamicTid) noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_DynamicTid].size();

            // If having at least one, pause
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_DynamicTid].at(_size - 1));
                // Execute __pause__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->pause();
                }
            }
            return;
            // SAME AS SINGLE VERSION
        }

        // *(Kernel) Manually Resume Latest Instance if Any and if Latest is heap - Given Thread Number (Virtual) (Must Override)
        inline virtual void __resume_latest_ifheap__(_ThreadIdType _DynamicTid) noexcept override {
            // Find out the latset instance
            const auto _size = _s_BaseType::_Shared_Pointer_Collector[_DynamicTid].size();

            // If having at least one, resume
            if (_size > 0) {
                _s_BaseType* _latest_ptr = reinterpret_cast<_s_BaseType*>(_s_BaseType::_Shared_Pointer_Collector[_DynamicTid].at(_size - 1));
                // Execute __resume__ on that instance if is heap mode
                if (_latest_ptr->is_heap()) {
                    // Polymophorism Active above since using _s_BaseType
                    _latest_ptr->resume();
                }
            }
            return;
            // SAME AS SINGLE VERSION
        }

        // *(Kernel) Stat Pipeline for very last instance (Virtual) (Must Override)
        inline virtual void __stat_pipeline_vl__() const noexcept override {
            // Use stats tool to do everything
            _Prototype_Timely_StatModels _stats(this->get_prjname(), this->_Shared_Performance_Table);
            _stats.gen(__timelystatssave__, __timelystatsverb__, __timelystatstyle__);
            return;
            // SAME AS SINGLE VERSION
        }

        // *(Kernel) Inspect A Snapshot of This Thread's Memory Module (Virtual) (Must Override)
        inline virtual _SnapshotType __snapshot__() const noexcept override {
            // This takes a snapshot for this thread

            _StringsteramType ss;
            ss << "  Multiple Thread Snapshot for Project [" << color4::green<_CharType> << this->get_prjname() << color4::reset<_CharType> << "] Thread (" << color4::green<_CharType> << _ToStringType(this->__threadid__()) << color4::reset<_CharType> << ")" << std::endl;
            const _SnapshotType _SingleThread_Prefix = ss.str();
            return _SingleThread_Prefix + this->__mem_module__().snapshot();
        }

        // *(Kernel) Static, Inspect A Snapshot of Every Memory Module
        static inline _SnapshotType __snapshot_all__() noexcept {
            // This takes a snapshot for all

            _SnapshotType _SnapShot;
            const _SizeType _size = _s_ThisType::_Shared_MultiThread_Timely_Memory_Module.size();
            
            _SizeType _count = 0;
            for (_SizeType i = 0; i < _size; ++i) {
                // Filter those unused, skip
                const auto& mod = _s_ThisType::_Shared_MultiThread_Timely_Memory_Module[i];
                if (std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike> == false || mod.empty()) {
                    continue;
                }

                // Create thread name by standardized thread id
                const _StringType _threadname = ((i == 0) ? "0" : _ToStringType((_count == 0) ? 1 : _count));
                if (i == 0) {
                    _count++;
                }
                else {
                    _count = (_count == 0) ? 2 : _count + 1;
                    // Why doing that?
                    // In case the <0> is not used in Multi-Thread Objects
                }

                // Print the thread snapshot out
                _StringsteramType ss;
                ss << "  Multiple Thread Snapshot for Project [" << color4::green<_CharType> << _s_ThisType::get_prjname() << color4::reset<_CharType> << "] Thread (" << color4::green<_CharType> << _threadname << color4::reset<_CharType> << ")" << std::endl;
                const _SnapshotType _SingleThread_Prefix = ss.str();
                _SnapShot += _SingleThread_Prefix + mod.snapshot();
            }

            return _SnapShot;
        }

    public:
        // Static, Snapshot All Method (calling __snapshot_all__)
        static inline _SnapshotType snapshot_all() noexcept {
            return _s_ThisType::__snapshot_all__();
        }

        // Constructor
        explicit _Prototype_MultipleThread_Timely() noexcept = delete;
        _Prototype_MultipleThread_Timely(const char* sigstr) exceptional :
            _s_BaseType(this->__maintain_threadid_index_mapping_and_pause_latest_ifheap__(), sigstr)
            // _ExternalThreadId(_ThisThreadId()), DONE IN INDEX MAPPING
        {
            // If Non-Deferrer Controller, Call Init/Exit
            if constexpr (std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary>) {
                this->__init__();
            }

            return;
        }
        _Prototype_MultipleThread_Timely(const char* sigstr, _HashType sighash) exceptional :
            _s_BaseType(this->__maintain_threadid_index_mapping_and_pause_latest_ifheap__(), sigstr, sighash)
            // _ExternalThreadId(_ThisThreadId()), DONE IN INDEX MAPPING
        {
            // If Non-Deferrer Controller, Call Init/Exit
            if constexpr (std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary>) {
                this->__init__();
            }

            return;
        }
        explicit _Prototype_MultipleThread_Timely(const _StringType& sigstr) noexcept = delete;
        explicit _Prototype_MultipleThread_Timely(const _StringType& sigstr, _HashType sighash) noexcept = delete;

        // Destructor
        virtual ~_Prototype_MultipleThread_Timely() noexcept {
            // If Non-Deferrer Controller, Call Init/Exit
            if constexpr (std::is_same_v<_Timely_Init_Controller, _Timely_Init_Ordinary>) {
                this->__exit__();
            }
            return;
        };
    };
    // This thread id represents for which is the thread you are at
    // If passed as -1, then automatically deducted
    // If passed as an integer, then decuced as thread id starting from 0

    // Prototype Generic Timely Wrapper (Deadable Class) (Single + Multi)
    template<_ThreadIdType _ThreadId, class _Timely_Memory_Module = _Timely_MemoryModel_Heaplike>
    class _Prototype_Generic_Timely_Wrapper {

    public:
        // Timely Memory Module can only be: Coverage, Heaplike
        static_assert(std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Coverage> || std::is_same_v<_Timely_Memory_Module, _Timely_MemoryModel_Heaplike>, "Timely Memory Module MUST be either Coverage or Heaplike.");

    public:
        // <timelyon> If and Only If Timely is Enabled, then construct 
        #if __timelyon__

    protected:
        // This type
        using _s_ThisType = _Prototype_Generic_Timely_Wrapper;

    public:
        // Timely Internal Object
        using _s_Timely_Internal_Type = std::conditional_t<(_ThreadId != -1),
            // For generic single threads
            _Prototype_SingleThread_Timely<_ThreadId, _Timely_Memory_Module, _Timely_Init_Ordinary>, 
            // For automatic multi-thread collection
            _Prototype_MultipleThread_Timely<_ThreadId, _Timely_Memory_Module, _Timely_Init_Ordinary>>;
        _s_Timely_Internal_Type _internal;
        
        // Manually Pause timing (Wrapper)
        inline void pause() exceptional {
            this->_internal.pause();
            return;
        }

        // Manually Resume timing (Wrapper)
        inline void resume() exceptional {
            this->_internal.resume();
            return;
        }

        // Manually Stop timing (Wrapper)
        inline void stop() exceptional {
            this->_internal.stop();
            return;
        }

        // Constructor
        explicit inline _Prototype_Generic_Timely_Wrapper() noexcept = delete;
        inline _Prototype_Generic_Timely_Wrapper(const char* sigstr) exceptional :
            _internal(sigstr){}
        inline _Prototype_Generic_Timely_Wrapper(const char* sigstr, _HashType sighash) exceptional :
            _internal(sigstr, sighash){}
        explicit inline _Prototype_Generic_Timely_Wrapper(const _StringType& sigstr) noexcept = delete;
        explicit inline _Prototype_Generic_Timely_Wrapper(const _StringType& sigstr, _HashType sighash) noexcept = delete;

        // Destructor
        inline ~_Prototype_Generic_Timely_Wrapper() noexcept = default;

        #endif
        // If and Only If Timely is Enabled, then construct </timelyon>
    };

    /////////////////////////////////////////////////////////////////////////////////

    // Global Alias for Automatic Timely Wrapper
    template <_ThreadIdType _ThreadId = 0, typename _MD = _Timely_MemoryModel_Heaplike>
    using _Timely = _Prototype_Generic_Timely_Wrapper<_ThreadId, _MD>;

    // Global Alias for Single Thread
    using _Timelyh = _Prototype_SingleThread_Timely<0, _Timely_MemoryModel_Heaplike, _Timely_Init_Ordinary>;
    using _Timelyc = _Prototype_SingleThread_Timely<0, _Timely_MemoryModel_Coverage, _Timely_Init_Ordinary>;

    // Global Alias for Multiple Thread
    using _Timelies = _Prototype_MultipleThread_Timely<-1, _Timely_MemoryModel_Heaplike, _Timely_Init_Ordinary>;
    using _Timeliesh = _Prototype_MultipleThread_Timely<-1, _Timely_MemoryModel_Heaplike, _Timely_Init_Ordinary>;
    using _Timeliesc = _Prototype_MultipleThread_Timely<-1, _Timely_MemoryModel_Coverage, _Timely_Init_Ordinary>;

}
// End namespace libnocppsh

