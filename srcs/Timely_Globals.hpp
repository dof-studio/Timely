// Timely_Globals.hpp
// 
// Version 0.0.1 built 20250706
//
// Global Objects and APIs of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include "Timely_Impl.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {

    // <timelyon> If and Only If Timely is Enabled, then construct 
    #if __timelyon__

    // Global Single Thread Protective Timelyc Object (c version must be the 1st registered)
    extern _Timelyc __GlobalSingleThreadTimelycObject__;
    // Reserved Names:
    // 
    // "__+-Globalc-+__" is reserved for the global `_Timelyc` Object (coverage mode)
    // that destructed last and does the summarization task in destructor.

    // Global Single Thread Protective Timelyh Object (h version must be after c version)
    extern _Timelyh __GlobalSingleThreadTimelyhObject__;
    // Reserved Names:
    // 
    // "__+-Globalh-+__" is reserved for the global `_Timelyh` Object (heaplike mode)
    // that destructed second last and does NOT do the summarization task in destructor.

    #endif
    // If and Only If Timely is Enabled, then construct </timelyon>

    // Global Set Project Interface (Compatible for Single/Multiple Thread)
    template<typename _TypePlaceHolder = void>
    inline void __GlobalTimelyProjectNameSet__(const _CharType* _new_prjname) noexcept {
        // <timelyon> If and Only If Timely is Enabled, then construct 
        #if __timelyon__
        
        _Base_Timely_Primative_Implementation::set_prjname(_new_prjname);
        
        #endif
        // If and Only If Timely is Enabled, then construct </timelyon>
        return;
    }
    template<typename _TypePlaceHolder = void>
    inline void __GlobalTimelyProjectNameSet__(const _StringType& _new_prjname) noexcept {
        // <timelyon> If and Only If Timely is Enabled, then construct 
        #if __timelyon__
        
        _Base_Timely_Primative_Implementation::set_prjname(_new_prjname);
        
        #endif
        // If and Only If Timely is Enabled, then construct </timelyon>
        return;
    }
    template<typename _TypePlaceHolder = void>
    inline void __GlobalTimelyProjectNameSet__(_StringType&& _new_prjname) noexcept {
        // <timelyon> If and Only If Timely is Enabled, then construct 
        #if __timelyon__
        
        _Base_Timely_Primative_Implementation::set_prjname(std::forward<_StringType>(_new_prjname));
        
        #endif
        // If and Only If Timely is Enabled, then construct </timelyon>
        
        return;
    }

    // Global Stack-Frame Callback System Callback (Compatible for Single/Multiple Thread)
    template<typename _TypePlaceHolder = void>
    inline _StringType __GlobalStackFrameSnapshotCallback__(_BoolType stop_ = false) noexcept {

        // <timelyon> If and Only If Timely is Enabled, then construct 
        #if __timelyon__

        // If Stop, then stop all from global instances
        if (stop_) {
            __GlobalSingleThreadTimelycObject__.stop();
            __GlobalSingleThreadTimelyhObject__.stop();
        }

        // Inspect global object's snapshot
        _SnapshotType _Snapshot = __GlobalSingleThreadTimelyhObject__.snapshot();

        // Inspect common multi-thread object's snapshot
        _Snapshot += _Prototype_MultipleThread_Timely<-1, _Timely_MemoryModel_Heaplike, _Timely_Init_Ordinary>::snapshot_all();

        return _Snapshot;

        #else

        return _StringType{ };
        
        #endif
        // If and Only If Timely is Enabled, then construct </timelyon>
    }

}
// End namespace libnocppsh
