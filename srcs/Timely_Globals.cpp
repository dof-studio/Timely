// Timely_Globals.cpp
// 
// Version 0.0.1 built 20250706
//
// Global Objects and APIs of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include "Timely_Globals.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {
	
    // <timelyon> If and Only If Timely is Enabled, then construct 
    #if __timelyon__

    // Global Single Thread Protective Timelyc Object (c version must be the 1st registered)
    _Timelyc __GlobalSingleThreadTimelycObject__ = _Timelyc(__timelycgblobjname__, __timelycgblobjhash__);
    // Reserved Names:
    // 
    // "__+-Globalc-+__" is reserved for the global `_Timelyc` Object (coverage mode)
    // that destructed last and does the summarization task in destructor.

    // Global Single Thread Protective Timelyh Object (h version must be after c version)
    _Timelyh __GlobalSingleThreadTimelyhObject__ = _Timelyh(__timelyhgblobjname__, __timelyhgblobjhash__);
    // Reserved Names:
    // 
    // "__+-Globalh-+__" is reserved for the global `_Timelyh` Object (heaplike mode)
    // that destructed second last and does NOT do the summarization task in destructor.

    #endif
    // If and Only If Timely is Enabled, then construct </timelyon>

}
// End namespace libnocppsh
