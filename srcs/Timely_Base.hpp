// Timely_Base.hpp
// 
// Version 0.0.1 built 20250706
//
// Base Classes of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include <cstdint>
#include <iomanip>  // For std::setprecision
#include <iostream>

#include "Color4/color4"

#include "Timely_Define.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {

    // Performance Registory Table
    // What's the use?
    // The performance table stores the time consumption to run a specific function
    // in the entire program runtime. By taking *elapsed*/*count*, we can get the average
    // execution time and use *elapsed* to measure the hot-spot-ness of the function.
    // Moreover, we give the minimum and maximum time elapsed for executing one specific 
    // function during the several executions happened.
    struct _Performance_Reg_Table {
        // One function creates only ONE table (shared)
        _SizeType      _thread_id = 0;             // Thread number 
        _HashType      _sig_hash = 0;              // Hash of signature of the function
        _StringType    _sig_string = "";           // String content of the signature of the function
        _SizeType      _invok_count = 0;           // Count of invoke of the function
        _NumberType    _invok_elapsed = 0;         // Cumulative time elapsed in executing the function for n times
        _NumberType    _invok_minelap = INFINITY;  // Minimum time elapsed in one call
        _NumberType    _invok_maxelap = 0;         // Maximum time elapsed in one call

        // Constructor
        explicit _Performance_Reg_Table() noexcept {}
        explicit _Performance_Reg_Table(
            const _SizeType& threadid,
            const _HashType& funchash,
            const _StringType& funcsignature) noexcept :
            _thread_id(threadid), _sig_hash(funchash), _sig_string(funcsignature) {
        }
        explicit _Performance_Reg_Table(
            const _SizeType& threadid,
            const _HashType& funchash,
            _StringType&& funcsignature) noexcept :
            _thread_id(threadid), _sig_hash(funchash), _sig_string(std::move(funcsignature)) {
        }

        // Register one new call 
        void register_call(const _NumberType& new_elapsed) noexcept {
            // Number of invokes plus one
            this->_invok_count++;

            // Cumulative elapsed time added
            this->_invok_elapsed += new_elapsed;

            // Register minimum and maximum
            if (this->_invok_minelap > new_elapsed) {
                this->_invok_minelap = new_elapsed;
            }
            if (this->_invok_maxelap < new_elapsed) {
                this->_invok_maxelap = new_elapsed;
            }
            return;
        }
    };

    // Base Timely Class
    // What is the base class?
    // The base class is the base of any defined class in the Timely project 
    // hierarchy. Kindly note, any Timely class's ownership can NEVER be transferred.
    // That's why we make the constructors and assignment methods as deleted functions.
    class _Base_Timely {
    public:
        _Base_Timely() noexcept {};
        virtual ~_Base_Timely() noexcept {};

        // Prohibited of copying and assigning
        _Base_Timely(const _Base_Timely& lv) noexcept = delete;
        _Base_Timely(_Base_Timely&& rv) noexcept = delete;
        _Base_Timely& operator=(const _Base_Timely& lv) noexcept = delete;
        _Base_Timely& operator=(_Base_Timely&& rv) noexcept = delete;
    };

    // Base Timely Primative Implementation Class
    // A Common Non-Template Class Implementation That Shares Performance Tracking Members.
    // A Multiple thread supported abstract class, with 4 specific members defined:
    // -> A global instance counter, which records the number of instances created across all threads
    // -> A thread-wise instance counter, which records the number of instances created in the specific thread
    // -> A thread-wise performance table, which records the performance of each function in each thread
    // -> An thread-instance-wise timely reference collections, which records the address pointer of the timely class as an ULL integer
    // As Well, a global shared Emergency Member is stored, meaning if emergency exit happens, do not conduct actual exit behaviors
    // A special assumption is made that the [0] thread ALWAYS represents the main thread.
    class _Base_Timely_Primative_Implementation : public _Base_Timely {

    public:
        // (member) Global Project Name (If Any)
        inline static _StringType _Member_Project_Name = "Default Timely Project";

    protected:
        // This type
        using _s_ThisType = _Base_Timely_Primative_Implementation;

        // Base type
        using _s_BaseType = _Base_Timely;

    protected:
        // (member) Shared, Atomic, Cross-Thread Having Instance Counter
        using _s_Global_Thread_Instance_Counter_Type = _AtomicWrapper<_SizeType>;
        inline static _s_Global_Thread_Instance_Counter_Type _Shared_Global_InstCounter;
        // If a thread is started with one new function, this get added with 1
        // If a thread exits the last function, this get minused with 1
        // Note, if this returns to 0, it means the program hits to the end Timely instance

        // (member) Shared, Thread Specific Reference Counter (reserved __timelymaxthread__ instances)
        using _s_Reference_Counter_Type = _VectorType<_SizeType>;
        inline static _s_Reference_Counter_Type _Shared_Reference_Counter { __timelymaxthread__, std::allocator<_SizeType>{} };
        // Note:
        // We MUST initialize like this or the __timelymaxthread__ may be interpreted as an element which making it with size 1

        // (member) Shared, Thread Specific Performance Table for All Threads (reserved __timelymaxthread__ instances)
        using _s_Performance_Table_Type = _VectorType<_MappingType<_HashType, _Performance_Reg_Table>>;
        inline static _s_Performance_Table_Type _Shared_Performance_Table { __timelymaxthread__ };

        // (member) Shared, Thread-Instance Specific Reference Collections for each instances (reserved __timelymaxthread__ instances)
        using _s_Pointer_Collector_Type = _VectorType<_VectorType<_PointerType>>;
        inline static _s_Pointer_Collector_Type _Shared_Pointer_Collector { __timelymaxthread__ };

        // (member) Shared, Global Atomic Emergency Indicator (If set to non-0, meaning Emergency Exit)
        using _s_Global_Emergency_Indicator_Type = _AtomicWrapper<_SizeType>;
        inline static _s_Global_Emergency_Indicator_Type _Shared_Emergency_Indicator = 0;

    public:
        // Constructor
        _Base_Timely_Primative_Implementation() noexcept : _Base_Timely() {};

        // Destructor
        virtual ~_Base_Timely_Primative_Implementation() noexcept {};

        // Set Project Name
        inline static void set_prjname(const _StringType& prjname) noexcept {
            _Base_Timely_Primative_Implementation::_Member_Project_Name = prjname;
            return;
        }
        inline static void set_prjname(_StringType&& prjname) noexcept {
            _Base_Timely_Primative_Implementation::_Member_Project_Name = std::move(prjname);
            return;
        }

        // Get Project Name
        inline static _StringType get_prjname() noexcept {
            return _Base_Timely_Primative_Implementation::_Member_Project_Name;
        }

        // Set to Emergency Status (Non-Reversable)
        inline static void set_emergency() noexcept {
            _Base_Timely_Primative_Implementation::_Shared_Emergency_Indicator.store(1);
            return;
        }

        // Get the Global Emergency Status
        inline static _SizeType get_emergency() noexcept {
            return _Base_Timely_Primative_Implementation::_Shared_Emergency_Indicator.load();
        }
    };

    // Base Timely Implementation Class
    // Just Derived From Base Timely Primative Sharing the Performance Tracking Members
    // With A Template Parameter setting the Thread Number
    // And we will check if the template thread number parameter is less than or equal to
    // the global maximum reserved thread number. If not, then static assert is triggered.
    // Moreover, it defines common (self) members used by one Timely Implementation.
    // Any Timely Implementation MUST be based on this class not the Primative Implementation class.
    template<_ThreadIdType _ThreadId>
    class _Base_Timely_Implementation : public _Base_Timely_Primative_Implementation {

    public:
        // Thread Id Must be Less Or Equal To Maximum Thread Number
        static_assert(_ThreadId >= -1 && _ThreadId < __timelymaxthread__, "Thread Id For Any Timely Related Instance Must NOT Be Greater Than Or Equal To Global Max Thread Reserved.");
        // Value  0 is reserved for Main Thread that is used by Single-Thread and Main-Thread Model
        // Value -1 is reserved for Automatic Thread Deduction mode in Multi-Thread Timely Models
        // Other positive values are reserved for Multi-Thread Timely Models

    protected:
        // This type
        using _s_ThisType = _Base_Timely_Implementation<_ThreadId>;

        // Base type
        using _s_BaseType = _Base_Timely_Primative_Implementation;

    protected:
        // (Member) Runtime Thread Id (Index Id) Reserved
        _ThreadIdType _Runtime_ThreadId;    // Store the Runtime Thread Id

        // (Member) Sig Hash of the invoked function
        _HashType _Sig_Hash;                // Store the actual value if the sig hash
        _CharType* _Sig_String = nullptr;   // Just holds the name pointer, do NOT allocate

        // (Member) Timing Objects
        _BoolType _Is_Started = true;       // Set to false when jumps out the heap frame
        _TimeType _Start_Time;              // The last start time
        _DuraType _Cumulative_Dura;         // The cumulative duration in ms, calculated each time jumps out and finalizes
    
    protected:
        // *(Kernel) Static Implementation Kernel Now
        inline static _TimeType __now_impl__() noexcept{
            // This function uses custom type declarations

            #ifdef _WIN32
                // Special Optimization for Windows System
                const static long long _Freq = _Query_perf_frequency(); // doesn't change after system boot
                const long long _Ctr  = _Query_perf_counter();          // runtime query

                // The compiler recognizes the constants for frequency and time period and uses shifts and
                // multiplies instead of divides to calculate the nanosecond value.
                constexpr long long _TenMHz        = 10'000'000;
                constexpr long long _TwentyFourMHz = 24'000'000;
                if (_Freq == _TenMHz) {
                    // 10 MHz is a very common QPC frequency on modern x86/x64 PCs. Optimizing for
                    // this specific frequency can double the performance of this function by
                    // avoiding the expensive frequency conversion path.
                    constexpr long long _Multiplier = std::nano::den / _TenMHz;
                    return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::nanoseconds(_Ctr * _Multiplier));
                } else if (_Freq == _TwentyFourMHz) {
                    // 24 MHz is a common frequency on ARM/ARM64, including cases where it emulates x86/x64.
                    const long long _Whole = (_Ctr / _TwentyFourMHz) * std::nano::den;
                    const long long _Part  = (_Ctr % _TwentyFourMHz) * std::nano::den / _TwentyFourMHz;
                    return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::nanoseconds(_Whole + _Part));
                }
                else {
                    // Instead of just having "(_Ctr * period::den) / _Freq",
                    // the algorithm below prevents overflow when _Ctr is sufficiently large.
                    // It assumes that _Freq * period::den does not overflow, which is currently true for nano period.
                    // It is not realistic for _Ctr to accumulate to large values from zero with this assumption,
                    // but the initial value of _Ctr could be large.
                    const long long _Whole = (_Ctr / _Freq) * std::nano::den;
                    const long long _Part = (_Ctr % _Freq) * std::nano::den / _Freq;
                    return std::chrono::time_point<std::chrono::steady_clock>(std::chrono::nanoseconds(_Whole + _Part));
                }
            #else
                // Fall back to STL Implementation
                return std::chrono::high_resolution_clock::now();
            #endif
        }

        // *(Kernel) Now
        inline _TimeType __now__() const noexcept {
            return this->__now_impl__(); 
        }

        // *(Kernel) Assign Start Time as NOW
        inline void __start_time_now__() noexcept {
            this->_Start_Time = this->__now__();
            return;
        }

        // *(Kernel) See if triggered Global Emergency Status
        inline _BoolType __is_emergency__() const noexcept {
            return _s_BaseType::get_emergency() != 0;
        }

        // *(Kernel) Output the Function Signiture
        inline _CharType* __get_funcsig__() const noexcept {
            return this->_Sig_String;
        }

    protected:
        // *(Kernel) Thread Number Retriever (Virtual) (Must be Override Each Time)
        inline virtual _SizeType __threadid__() const noexcept = 0;

        // *(Kernel) Thread Number Retriever In Runtime (Virtual) (Must be Override Each Time)
        inline virtual _SizeType __threadid_runtime__() const noexcept = 0;

        // *(Kernel) Is Heap Mode (Virtual) (Must Override)
        inline virtual constexpr _BoolType __is_heap__() const noexcept = 0;

        // *(Kernel) Is Heap Mode Detected In Runtime (Virtual) (Must Override)
        inline virtual _BoolType __is_heap_runtime__() const noexcept = 0;

        // *(Kernel) Is Ordinary Controller (Virtual) (Must Override)
        inline virtual constexpr _BoolType __is_ordinary__() const noexcept = 0;

        // *(Kernel) Register the Instance to Heap (Virtual) (Must Override)
        inline virtual void __register_heap__() noexcept = 0;

        // *(Kernel) Popout the Last Instance from Heap (Virtual) (Must Override)
        inline virtual _PointerType __popout_heap__() noexcept = 0;

        // *(Kernel) Manually Pause Latest Instance if Any and if Latest is heap (Virtual) (Must Override)
        inline virtual void __pause_latest_ifheap__() noexcept = 0;

        // *(Kernel) Manually Resume Latest Instance if Any and if Latest is heap (Virtual) (Must Override)
        inline virtual void __resume_latest_ifheap__() noexcept = 0;

        // *(Kernel) Manually Pause Latest Instance if Any and if Latest is heap - Given Thread Number (Virtual) (Must Override)
        inline virtual void __pause_latest_ifheap__(_ThreadIdType _DynamicTid) noexcept = 0;

        // *(Kernel) Manually Resume Latest Instance if Any and if Latest is heap - Given Thread Number (Virtual) (Must Override)
        inline virtual void __resume_latest_ifheap__(_ThreadIdType _DynamicTid) noexcept = 0;

        // *(Kernel) Register to Performance Table if not existed
        inline virtual void __register_ptable__() const noexcept {
            // By default, if not override __threadid__, then do with the Main Thread
            if (!this->_Shared_Performance_Table[this->__threadid__()].contains(this->_Sig_Hash)) {
                _HashType key = _HashType(this->_Sig_Hash);
                _Performance_Reg_Table table = _Performance_Reg_Table(this->__threadid__(), this->_Sig_Hash, _StringType(this->_Sig_String));
                this->_Shared_Performance_Table[this->__threadid__()][key] = std::move(table);
            }
            return;
        }

        // *(Kernel) Update to Performance Table when exits
        inline virtual void __update_ptable__() const noexcept {
            // By default, if not override __threadid__, then do with the Main Thread

            // Get the cumulative duration in ms
            _NumberType new_elapsed = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(this->_Cumulative_Dura).count()) / 1000'000;
            this->_Shared_Performance_Table[this->__threadid__()][this->_Sig_Hash].register_call(new_elapsed);
            return;
        }

        // *(Kernel) Manually Pause Timing
        inline virtual void __pause__() noexcept {
            // If is working
            if (this->_Is_Started) {

                // Add previously accumulated time
                _TimeType now = this->__now__();
                this->_Cumulative_Dura += now - this->_Start_Time;

                // Stop working
                this->_Is_Started = false;
            }
            return;
        }

        // *(Kernel) Manually Resume Timing
        inline virtual void __resume__() noexcept {
            // If is not working
            if (!this->_Is_Started) {

                // Set start time to now
                this->_Start_Time = this->__now__();

                // Start working
                this->_Is_Started = true;
            }
            return;
        }

        // *(Kernel) Manually Pause All within this thread
        inline virtual void __pause_all__() noexcept {
            // Find out the latset instance
            const auto _size = this->_Shared_Pointer_Collector[this->__threadid__()].size();
            
            // Iterate over each instance, inversely
            auto& _ref = this->_Shared_Pointer_Collector[this->__threadid__()];
            for (_IntegerType i = _size - 1; i >= 0; --i) {
                auto _latest_ptr = reinterpret_cast<_s_ThisType*>(_ref.at(i));

                // Execute __pause__ on that instance
                _latest_ptr->__pause__();
            }
            return;
        }

        // *(Kernel) Manually Resume All within this thread
        inline virtual void __resume_all__() noexcept {
            // Find out the latset instance
            const auto _size = this->_Shared_Pointer_Collector[this->__threadid__()].size();
            
            // Iterate over each instance, inversely
            auto& _ref = this->_Shared_Pointer_Collector[this->__threadid__()];
            for (_IntegerType i = _size - 1; i >= 0; --i) {
                auto _latest_ptr = reinterpret_cast<_s_ThisType*>(_ref.at(i));

                // Execute __resume__ on that instance
                _latest_ptr->__resume__();
            }
            return;
        }

        // *(Kernel) Manually Pause Latest within this thread
        inline virtual void __pause_latest__() noexcept {
            // Find out the latset instance
            const auto _size = this->_Shared_Pointer_Collector[this->__threadid__()].size();
            if (_size > 0) {
                auto _latest_ptr = reinterpret_cast<_s_ThisType*>(this->_Shared_Pointer_Collector[this->__threadid__()].at(_size - 1));

                // Execute __pause__ on that instance
                _latest_ptr->__pause__();
            }
            return;
        }

        // *(Kernel) Manually Resume Latest within this thread
        inline virtual void __resume_latest__() noexcept {
            // Find out the latset instance
            const auto _size = this->_Shared_Pointer_Collector[this->__threadid__()].size();
            if (_size > 0) {
                auto _latest_ptr = reinterpret_cast<_s_ThisType*>(this->_Shared_Pointer_Collector[this->__threadid__()].at(_size - 1));

                // Execute __resume__ on that instance
                _latest_ptr->__resume__();
            }
            return;
        }

        // *(Kernel) Manually Pause Previous Instance if Any
        inline virtual void __pause_prev__() noexcept {
            // Find out the latset instance
            const auto _size = this->_Shared_Pointer_Collector[this->__threadid__()].size();
            
            // If not the 1st one, then pause the previous one
            if (_size > 1) {
                auto _prev_ptr = reinterpret_cast<_s_ThisType*>(this->_Shared_Pointer_Collector[this->__threadid__()].at(_size - 2));
                // Execute __pause__ on that instance
                _prev_ptr->__pause__();
            }
            return;
        }

        // *(Kernel) Manually Resume Previous Instance if Any
        inline virtual void __resume_prev__() noexcept {
            // Find out the latset instance
            const auto _size = this->_Shared_Pointer_Collector[this->__threadid__()].size();

            // If not the 1st one, then resume the previous one
            if (_size > 1) {
                auto _prev_ptr = reinterpret_cast<_s_ThisType*>(this->_Shared_Pointer_Collector[this->__threadid__()].at(_size - 2));
                // Execute __resume__ on that instance
                _prev_ptr->__resume__();
            }
            return;
        }

        // *(Kernel) Manually Perform Error Stop (Uncaught Error) (Must Override)
        inline virtual void __pause_uponerror__() noexcept {
            // If heaplike, then pause the last one
            if (this->__is_heap__()) {
                this->__pause_latest__();
            }
            // If coverage, then pause all
            else {
                this->__pause_all__();
            }
            return;
        }

        // *(Kernel) Manually Perform Error Resume (What?) (Must Override)
        inline virtual void __resume_uponerror__() noexcept {
            // If heaplike, then resume the last one
            if (this->__is_heap__()) {
                this->__resume_latest__();
            }
            // If coverage, then resume all
            else {
                this->__resume_all__();
            }
            return;
        }

        // *(Kernel) Inspect A Snapshot of the current Timely invocation (Non Overridable)
        inline virtual _SnapshotType __snapshot_self__() const noexcept final {
            // Return the name and other information of the invoked function

            // Calculate the name
            auto _funcname = _StringType(this->_Sig_String);
            if (_funcname.size() >= 77) {
                // Regularization to 80 bytes
                _funcname = _funcname.substr(0, 77) + "...";
            }

            _StringsteramType ss;
            ss << _funcname << color4::reset<_CharType> << " @ " << color4::green<_CharType> << this->_Shared_Performance_Table[this->__threadid__()][this->_Sig_Hash]._invok_count + 1 << color4::reset<_CharType> << " times";
            // Why + 1? Since not yet destructed, so number should be 1 more

            return _SnapshotType{ ss.str()};
        }

        // *(Kernel) Stat Pipeline for very last instance (Virtual) (Must Override)
        inline virtual void __stat_pipeline_vl__() const noexcept = 0;

        // *(Kernel) Inspect A Snapshot of Memory Module (Virtual) (Must Override)
        inline virtual _SnapshotType __snapshot__() const noexcept = 0;

        // *(Kernel) Constructor Executor (Virtual)
        inline virtual void __init__() noexcept {

            // If previous is `heap` mode, pause the previous instance if any
            // ** Called in constructors.
            // 
            // this->__pause_prev_ifheap__();

            // Global atomic increment if first instance
            if (this->_Shared_Reference_Counter[this->__threadid__()] == 0) {
                this->_Shared_Global_InstCounter.fetch_add(1);
            }

            // Reference increment
            this->_Shared_Reference_Counter[this->__threadid__()]++;

            // Register to Instance 
            this->_Shared_Pointer_Collector[this->__threadid__()].push_back(reinterpret_cast<_PointerType>(this));

            // Register to an empty Performance Table if not exists
            this->__register_ptable__();

            // If `heap` mode, then register the instance to heap
            this->__register_heap__();

            // Always Call __start_time_now__ to formally start timing for real execution
            this->__start_time_now__();

            return;
        }

        // *(Kernel) Destructor Executor (Virtual)
        inline virtual void __exit__() noexcept {

            // If Emergency Mode, Directly Return (Present in each __exit__)
            if (this->__is_emergency__()) {
                return;
            }

            // Pause
            this->__pause__();

            // If `heap` mode, then pop out the instance from heap
            this->__popout_heap__();

            // Update to the Performance Table
            this->__update_ptable__();

            // Remove from to Instance Collector
            this->_Shared_Pointer_Collector[this->__threadid__()].pop_back();

            // Reference decrement
            this->_Shared_Reference_Counter[this->__threadid__()]--;

            // Global atomic decrement if last instance
            if (this->_Shared_Reference_Counter[this->__threadid__()] == 0) {
                this->_Shared_Global_InstCounter.fetch_sub(1);
            }

            // SingleThread atomic does NOT decrement since it is monotonic
            // ...

            // If the very last instance we do the statistic summarization for all threads
            if (this->_Shared_Global_InstCounter.load() == 0) {
                this->__stat_pipeline_vl__();
            }

            // If previous is `heap` mode, resume the previous instance if any
            this->__resume_latest_ifheap__(); // Why last? Since the most recent one is poped out.

            return;
        }

    public:
        // Exportable Snapshot Self (Final, non-overridable)
        virtual _SnapshotType snapshot_self__() const noexcept final {
            return this->__snapshot_self__();
        }

        // Exportable Snapshot Taken for the entire Callback System / Memory Module if Any
        virtual _SnapshotType snapshot() const noexcept {
            return this->__snapshot__();
        }

        // Is Heap Mode (Virtual) (Wrapper)
        virtual constexpr _BoolType is_heap() const noexcept {
            return this->__is_heap__();
        }

        // Is Heap Mode at Runtime (Virtual) (Wrapper)
        virtual _BoolType is_heap_runtime() const noexcept {
            return this->__is_heap_runtime__();
        }

        // Get Function Sig (Wrapper)
        virtual _CharType* get_funcsig() const noexcept {
            return this->__get_funcsig__();
        }

    public:
        // Manually Pause timing (Virtual)
        virtual void pause() noexcept {
            this->__pause__();
            return;
        }

        // Manually Resume timing (Virtual)
        virtual void resume() noexcept {
            this->__resume__();
            return;
        }

        // Manually Stop the timing hierarchy and Turn ON Emergency (Virtual)
        virtual void stop() noexcept {
            // Pause upon error happening
            this->__pause_uponerror__();
            
            // Set to Emergency Status, avoid exiting
            _s_BaseType::set_emergency();
            return;
        }

        // Constructor
        explicit _Base_Timely_Implementation() noexcept = default;
        _Base_Timely_Implementation(_ThreadIdType rt_threadid, const char* sigstr) noexcept :
            _Base_Timely_Primative_Implementation(),
            _Runtime_ThreadId(rt_threadid),
            _Sig_Hash(_Timely_Hash(sigstr)),
            _Sig_String(const_cast<char*>(sigstr)),
            _Is_Started(true){}; // Start Time Uninitialized
        _Base_Timely_Implementation(_ThreadIdType rt_threadid, const char* sigstr, _HashType sighash) noexcept :
            _Base_Timely_Primative_Implementation(),
            _Runtime_ThreadId(rt_threadid),
            _Sig_Hash(sighash),
            _Sig_String(const_cast<char*>(sigstr)),
            _Is_Started(true){}; // Start Time Uninitialized
        explicit _Base_Timely_Implementation(_ThreadIdType rt_threadid, const _StringType& sigstr) noexcept = delete;
        explicit _Base_Timely_Implementation(_ThreadIdType rt_threadid, const _StringType& sigstr, _HashType sighash) noexcept = delete;

        // Destructor
        virtual ~_Base_Timely_Implementation() noexcept {};

    };

    // Base Timely Memory Model
    // What is this?
    // We stores common methods and use polymophorism to unify APIs.
    // This serves as the base class of Memory Models
    class _Timely_MemoryModel_Base : public _Base_Timely {
    protected:
        // (Kernel) Snapshot (Taking a snapshot of the memory model)
        virtual inline _SnapshotType __snapshot__() const noexcept = 0;

        // (Kernel) Is Heap Runtime Identifier (Must Override)
        virtual inline _BoolType __is_heap_runtime__() const noexcept = 0;

    public:
        // Is Heap at Runtime
        inline _BoolType is_heap_runtime() const noexcept {
            return this->__is_heap_runtime__();
        }

        // Empty-ness of the container (true, but overridable)
        virtual _BoolType empty() const noexcept {
            return true;
        }

        // Size of the container (0, but overridable)
        virtual _SizeType size() const noexcept {
            return 0;
        }

        // Reserve the heap capacity (none, but overridable)
        virtual void reserve(const _SizeType& size) noexcept {
            return;
        }

        // Inspect back (exceptional, but overridable)
        virtual _PointerType inspect_back() const exceptional {
            throw _Timely_Exception("_Timely_MemoryModel_Base method inspect_back() is called while the heap is empty.");
        }

        // Push back one element to the top of the heap (none, but overridable)
        virtual void push_back(const _PointerType& elem) noexcept {
            return;
        }

        // Pop back one element from the top of the heap (exceptional, but overridable)
        virtual _PointerType pop_back() exceptional {
            throw _Timely_Exception("_Timely_MemoryModel_Base method pop_back() is called while the heap is empty.");
        }

    public:

        // Snapshot (Taking a snapshot of the memory model)
        virtual inline _SnapshotType snapshot() const noexcept {
            return this->__snapshot__();
            // Note. To re-implement, please override __snapshot__ method.
        }

        // Constructor
        _Timely_MemoryModel_Base() noexcept : _Base_Timely() {};

        // Destructor
        virtual ~_Timely_MemoryModel_Base() noexcept {};
    };

}
// End namespace libnocppsh
