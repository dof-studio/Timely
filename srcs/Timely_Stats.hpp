// Timely_Stats.hpp
// 
// Version 0.0.1 built 20250706
//
// Statistic Collection Tools of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>

#include "Color4/color4"

#include "Timely_Define.hpp"
#include "Timely_Hash.hpp"
#include "Timely_Base.hpp"
#include "Timely_Html.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {

	// _Prototype_Timely_StatModels
	// What is the stat model?
	// StatModels is a collection of tools that can process a vector of performance tables
	// stored in `_s_Performance_Table_Type` and give core statistics of performance analysis.
	// It can also create and save the performance analysis either in a table format,
	// or in a human-readable internet web format.
	// This instance should only be created at the VERY LAST step of the global timely instance before program exits.
	class _Prototype_Timely_StatModels : public _Base_Timely {

	protected:
		// (Struct) Structure of Statistics - Line
		struct _s_Statistics_Line_Type {
		public:
			// Signiture of Method/Function/Timing Unit
			_StringType _SignatureOfTimingUnit;

			// Hash of Method/Function/Timing Unit
			_HashType _HashOfTimingUnit;

			// Number of Thread (Cardinal Thread Number starting from 0)
			_SizeType _NoOfThread;

			// Number of Calls Counted
			_SizeType _NoOfCount;

			// Total Elapsed Time
			_NumberType _ElapsedTotally;

			// Average Elapsed Time
			_NumberType _ElapsedAverage;

			// Minimum Elapsed Time 
			_NumberType _ElapsedMinimum;

			// Maximum Elapsed Time
			_NumberType _ElapsedMaximum;

			// Percentage of Total Elapsed Time of the Global Runtime (*%) (Need Calculation)
			_NumberType _nc_ElapsedTotalPercentage = 0;

			// Ranking as ranked by total elapsed time (descending) (Need Calculation)
			_SizeType _nc_RankingElapsedTotal = 0;
			// Starting from 0, but 0 is always by the Global Instance

			// Ranking as ranked by averaged elapsed time (descending) (Need Calculation)
			_SizeType _nc_RankingElapsedAverage = 0;
			// Starting from 0, but 0 is always by the Global Instance

		public:
			// Constructor
			explicit _s_Statistics_Line_Type() noexcept = delete;
			explicit _s_Statistics_Line_Type(
				const _SizeType& noOfThread, 
				const _StringType& signatureOfTimingUnit, 
				const _HashType& hashOfTimingUnit,
				const _SizeType& noOfCount, 
				const _NumberType& elapsedTotally, 
				const _NumberType& elapsedAverage, 
				const _NumberType& elapsedMinimum, 
				const _NumberType& elapsedMaximum) noexcept :
				_NoOfThread(noOfThread),
				_SignatureOfTimingUnit(signatureOfTimingUnit),
				_HashOfTimingUnit(hashOfTimingUnit),
				_NoOfCount(noOfCount),
				_ElapsedTotally(elapsedTotally),
				_ElapsedAverage(elapsedAverage),
				_ElapsedMinimum(elapsedMinimum),
				_ElapsedMaximum(elapsedMaximum) {}
			explicit _s_Statistics_Line_Type(
				const _SizeType& noOfThread,
				_StringType&& signatureOfTimingUnit,
				const _HashType& hashOfTimingUnit,
				const _SizeType& noOfCount,
				const _NumberType& elapsedTotally,
				const _NumberType& elapsedAverage,
				const _NumberType& elapsedMinimum,
				const _NumberType& elapsedMaximum) noexcept :
				_NoOfThread(noOfThread),
				_SignatureOfTimingUnit(std::move(signatureOfTimingUnit)),
				_HashOfTimingUnit(hashOfTimingUnit),
				_NoOfCount(noOfCount),
				_ElapsedTotally(elapsedTotally),
				_ElapsedAverage(elapsedAverage),
				_ElapsedMinimum(elapsedMinimum),
				_ElapsedMaximum(elapsedMaximum) {}
		};

		// (Type) CSV Files Type
		using _s_CSV_Files_Type = _VectorType<_StringType>;

		// (Type) Hash-Number Vector Pair
		using _s_Hash_Number_Vector_Pair_Type = _PairType<_VectorType<_HashType>, _VectorType<_NumberType>>;

	protected:
		// (Member) A Reference to Thread Specific Performance Table for All Threads
		using _s_Performance_Table_Type = _VectorType<_MappingType<_HashType, _Performance_Reg_Table>>;
		const _s_Performance_Table_Type& _Reference_Performance_Table;

		// (Member) An Instance of Ranked (Based on Total Elapsed) PerfTable Object
		using _s_Performance_Rank_TotalElapsed_Type = _VectorType<_s_Hash_Number_Vector_Pair_Type>;
		_s_Performance_Rank_TotalElapsed_Type _Member_Ranking_TotalElapsed;
		// The index of each hash shows the ranking

		// (Member) An Instance of Ranked (Based on Average Elapsed) PerfTable Mapping
		using _s_Performance_Rank_AverageElapsed_Type = _VectorType<_s_Hash_Number_Vector_Pair_Type>;
		_s_Performance_Rank_AverageElapsed_Type _Member_Ranking_AverageElapsed;
		// The index of each hash shows the ranking

		// (Member) An Instance of Table Lines Object With Hash-Element Pairing Dict
		using _s_Performance_Stats_Dict_Type = _VectorType<_MappingType<_HashType, _IndexType>>;
		_s_Performance_Stats_Dict_Type _Member_Statistics_HashMap;

		// (Member) An Instance of Table Lines Object Unsorted Stored in Thread Indexed Vectors
		using _s_Performance_Stats_Type = _VectorType<_VectorType<_s_Statistics_Line_Type>>;
		_s_Performance_Stats_Type _Member_Statistics_Table;

		// (Member) An Instance of Table Lines Object Sorted Stored in Thread Indexed Vectors
		_s_Performance_Stats_Type _Member_Statistics_Table_Sorted;

		// (Member) Title Name for This Timely Report (like SingleThread/Multiple and Other Things)
		_StringType _TitleName;

	protected:
		// *(Kernel) Format hex
		inline static _StringType __format_hex__(const _HashType& hash) noexcept {
			_StringsteramType ss;
			ss << std::hex << hash;
			return ss.str();
		}

		// *(Kernel) Format ms
		inline static _StringType __format_ms__(const _NumberType& time) noexcept{
			if (time < 2e-5) {
				return _StringType("< 20ns");
			}
			else if (time < 1e-3) {
				return _ToStringType(time * 1000, 3) + "us";
			}
			else if (time < 1) {
				return _ToStringType(time, 5);
			}
			else if (time < 100000) {
				return _ToStringType(time, 3);
			}
			else {
				// Regard as an integer
				return _ToStringType(_SizeType(time));
			}
		}

		// *(Kernel) Format percentage
		inline static _StringType __format_percent__(const _NumberType& percent) noexcept {
			if (percent < 1e-5) {
				return _StringType("< 1e-5");
			}
			else if (percent < 1e-3) {
				return _ToStringType(percent, 5);
			}
			else if (percent < 100) {
				return _ToStringType(percent, 3);
			}
			else {
				return _ToStringType(_SizeType(std::round(percent)));
			}
		}

		// *(Kernel) Rewrite Performance Table into Statistics Table Without Filling with Calculation Items
		inline _PairType<_s_Performance_Stats_Type, _s_Performance_Stats_Dict_Type> __rewrite_statstable_prim__(const _s_Performance_Table_Type& tables) const noexcept {
			
			// Create a stats table containing effective threads and a mapping contains how hash-index are stored
			_SizeType _effective_threads = 0;
			for (const auto& t : tables) {
				if (t.size() > 0) {
					_effective_threads++;
				}
			}
			_s_Performance_Stats_Type _stats(_effective_threads);
			_s_Performance_Stats_Dict_Type _hdict(_effective_threads);

			// For each table, rewrite essential items without sorting
			_SizeType _count = 0;
			for (const auto& t : tables) {

				// Same Idea: If and only if the table contains more than 0 elements
				if (t.size() > 0) {

					// Reserve elements using size in the performance table
					_stats[_count].reserve(t.size());
					_hdict[_count].reserve(t.size() * 2); // Reserve more to avoid collision

					// Now, _count is not only the index, but also the thread number
					for (const auto& pair : t) {
						// Create a line type for each function within that thread
						_s_Statistics_Line_Type line(_count,
							_StringType(pair.second._sig_string), pair.second._sig_hash,
							pair.second._invok_count, pair.second._invok_elapsed,
							pair.second._invok_elapsed / pair.second._invok_count,
							pair.second._invok_minelap, pair.second._invok_maxelap);

						// Move to the new Stats table and Hdict
						_stats[_count].push_back(std::move(line));
						_hdict[_count][pair.first] = _stats[_count].size() - 1; // which is the index
					}
					_count++;
				}
			}

			return std::make_pair(std::move(_stats), std::move(_hdict));
		}

		// *(Kernel) Get Hash-TotalElapsed Pair from Performance Reference (unsorted) (one thread) (multithread-able)
		inline _s_Hash_Number_Vector_Pair_Type __pair_hash_ttlelp__(const _SizeType& threadid) const exceptional {
			
			// If _Member_Statistics_Table is with 0 size
			if (this->_Member_Statistics_Table.empty()) {
				throw _Timely_Exception("_Prototype_Timely_StatModels method __pair_hash_ttlelp__() is called while the Member Statistics Table is not initialized");
			}
			// If the threadid is out of scope of the first vector
			if (this->_Member_Statistics_Table.size() <= threadid) {
				throw _Timely_Exception("_Prototype_Timely_StatModels method __pair_hash_ttlelp__() is called with threadid = " + std::to_string(threadid) + " while the Member Statistics Table only has " + std::to_string(this->_Member_Statistics_Table.size()) + " tables");
			}

			// Collect and return the pair
			_s_Hash_Number_Vector_Pair_Type hnpair; // hash, totally_elapsed
			const auto& table = this->_Member_Statistics_Table[threadid];
			hnpair.first.reserve(table.size());
			hnpair.second.reserve(table.size());
			for (const auto& line : table) {
				hnpair.first.push_back(line._HashOfTimingUnit);
				hnpair.second.push_back(line._ElapsedTotally);
			}
			return hnpair;
		}

		// *(Kernel) Get Hash-AverageElapsed Pair from Performance Reference (unsorted) (one thread) (multithread-able)
		inline _s_Hash_Number_Vector_Pair_Type __pair_hash_avgelp__(const _SizeType& threadid) const exceptional {

			// If _Member_Statistics_Table is with 0 size
			if (this->_Member_Statistics_Table.empty()) {
				throw _Timely_Exception("_Prototype_Timely_StatModels method __pair_hash_avgelp__() is called while the Member Statistics Table is not initialized");
			}
			// If the threadid is out of scope of the first vector
			if (this->_Member_Statistics_Table.size() <= threadid) {
				throw _Timely_Exception("_Prototype_Timely_StatModels method __pair_hash_avgelp__() is called with threadid = " + std::to_string(threadid) + " while the Member Statistics Table only has " + std::to_string(this->_Member_Statistics_Table.size()) + " tables");
			}

			// Collect and return the pair
			_s_Hash_Number_Vector_Pair_Type hnpair; // hash, average_elapsed
			const auto& table = this->_Member_Statistics_Table[threadid];
			hnpair.first.reserve(table.size());
			hnpair.second.reserve(table.size());
			for (const auto& line : table) {
				hnpair.first.push_back(line._HashOfTimingUnit);
				hnpair.second.push_back(line._ElapsedAverage);
			}
			return hnpair;
		}

		// *(Kernel) Sort Hash-Number Vector Pair Implementation (Inplace, Binary)
		inline void __pair_hash_number_sort_implace_impl__(_VectorType<_HashType>& hashes, _VectorType<_NumberType>& numbers, _SizeType left, _SizeType right) const noexcept {
			
			// Partitions both vectors based on the numbers vector
			static auto _partition = [](_VectorType<_HashType>& hashes, _VectorType<_NumberType>& numbers, _SizeType left, _SizeType right) noexcept {
				_NumberType pivot = numbers[right];
				_SizeType i = left;
				for (_SizeType k = left; k < right; k++) {
					if (numbers[k] < pivot) {
						std::swap(numbers[i], numbers[k]);
						std::swap(hashes[i], hashes[k]);
						++i;
					}
				}
				std::swap(numbers[i], numbers[right]);
				std::swap(hashes[i], hashes[right]);
				return i;
			};

			// Recursively sorts both vectors in-place
			if (left < right) {
				_SizeType mid = _partition(hashes, numbers, left, right);
				if (mid > left) {
					this->__pair_hash_number_sort_implace_impl__(hashes, numbers, left, mid - 1);
				}
				if (mid < right) {
					this->__pair_hash_number_sort_implace_impl__(hashes, numbers, mid + 1, right);
				}
			}
			return;
		}

		// *(Kernel) Sort Hash-Number Vector Pair for One Pair (Inplace)
		inline void __pair_hash_number_sort_inplace__(_s_Hash_Number_Vector_Pair_Type& pair, _BoolType asec = true) const exceptional {
			
			if (pair.first.size() != pair.second.size()) {
				throw _Timely_Exception("_Prototype_Timely_StatModels method __pair_hash_number_sort_inplace__() is called while the input pair has two Different Length Members");
			}
			// Using recursive calls, which may be time consuming if large
			if (!pair.first.empty()) {

				// Sort it
				this->__pair_hash_number_sort_implace_impl__(pair.first, pair.second, 0, pair.first.size() - 1);

				// If descending then reverse it
				if (asec == false) {
					std::reverse(pair.first.begin(), pair.first.end());
					std::reverse(pair.second.begin(), pair.second.end());
				}
			}
			return;
		}

		// *(Kernel) Perform Total Elapsed Percentage Computation (in-place) (all threads)
		inline void __ttl_elapsed_pct_calc__() noexcept {
			// First retrieve the total elapsed time for the global element
			constexpr _HashType _global_hash = _Timely_Hash(__timelycgblobjname__);
			const _NumberType global_elapsed = this->_Reference_Performance_Table.at(0).at(_global_hash)._invok_elapsed;
			const _NumberType global_elapsed_div100 = global_elapsed / 100; // This is to ensure the calculation yields a percentage

			// Then calculate the percentage in *% format for each element
			for (auto& thread : this->_Member_Statistics_Table) {
				for (auto& obj : thread) {
					obj._nc_ElapsedTotalPercentage = obj._ElapsedTotally / global_elapsed_div100;
				}
			}
			return;
		}

		// *(Kernel) Perform Elapsed-Total Ranking Compuation (in-place) (all threads)
		inline void __ttl_elapsed_rnk_calc__() exceptional {
			// We must ensure the stats table is constructed here before calling this

			// Get the Total Ranking Pairs for Each Thread and Sort Inplace
			_s_Performance_Rank_TotalElapsed_Type _ttlelp_pairs;
			_ttlelp_pairs.reserve(this->_Member_Statistics_Table.size());
			for (_SizeType i = 0; i < this->_Member_Statistics_Table.size(); ++i) {
				auto pairs = this->__pair_hash_ttlelp__(i);
				this->__pair_hash_number_sort_inplace__(pairs, false);
				// Exception may NOT be thrown since we ensure the same length
				_ttlelp_pairs.push_back(pairs);
			}

			// Save to Stats table according to the sequence
			for (_SizeType i = 0; i < this->_Member_Statistics_Table.size(); ++i) {
				// i - represents for thread number (index)

				// The stats table reference
				auto& _s_table = this->_Member_Statistics_Table[i];

				// The hashmap table reference
				const auto& _hashmap = this->_Member_Statistics_HashMap[i];

				const auto& _pair = _ttlelp_pairs[i];
				const _SizeType _size = _pair.first.size();
				for (_SizeType j = 0; j < _size; ++j) {
					// j - represents for element number (index)

					// The according hash and value
					const auto& _hash = _pair.first[j];
					const auto& _numb = _pair.second[j];

					// Lookup the hashmap to see the index stored in the stats table
					const auto& _iter = _hashmap.find(_hash);
					if (_iter == _hashmap.end()) {
						throw _Timely_Exception("_Prototype_Timely_StatModels method __ttl_elapsed_rnk_calc__() is called and looking up for hash " + std::to_string(_hash) + " but not found in the stats hashmap table");
					}
					const auto _index = (*_iter).second;

					// Save the ranking to stats table
					_s_table[_index]._nc_RankingElapsedTotal = j;
				}
			}

			// Save to this _Member_Ranking_TotalElapsed
			this->_Member_Ranking_TotalElapsed = std::move(_ttlelp_pairs);
			return;

			// @TODO: two loops can be reduced to once.
		}

		// *(Kernel) Perform Elapsed-Average Ranking Computation (in-place) (all threads)
		inline void __avg_elapsed_rnk_calc__() exceptional {
			// We must ensure the stats table is constructed here before calling this

			// Get the Average Ranking Pairs for Each Thread and Sort Inplace
			_s_Performance_Rank_AverageElapsed_Type _avgelp_pairs;
			_avgelp_pairs.reserve(this->_Member_Statistics_Table.size());
			for (_SizeType i = 0; i < this->_Member_Statistics_Table.size(); ++i) {
				auto pairs = this->__pair_hash_avgelp__(i);
				this->__pair_hash_number_sort_inplace__(pairs, false);
				// Exception may NOT be thrown since we ensure the same length
				_avgelp_pairs.push_back(pairs);
			}

			// Save to Stats table according to the sequence
			for (_SizeType i = 0; i < this->_Member_Statistics_Table.size(); ++i) {
				// i - represents for thread number (index)

				// The stats table reference
				auto& _s_table = this->_Member_Statistics_Table[i];

				// The hashmap table reference
				const auto& _hashmap = this->_Member_Statistics_HashMap[i];

				const auto& _pair = _avgelp_pairs[i];
				const _SizeType _size = _pair.first.size();
				for (_SizeType j = 0; j < _size; ++j) {
					// j - represents for element number (index)

					// The according hash and value
					const auto& _hash = _pair.first[j];
					const auto& _numb = _pair.second[j];

					// Lookup the hashmap to see the index stored in the stats table
					const auto& _iter = _hashmap.find(_hash);
					if (_iter == _hashmap.end()) {
						throw _Timely_Exception("_Prototype_Timely_StatModels method __avg_elapsed_rnk_calc__() is called and looking up for hash " + std::to_string(_hash) + " but not found in the stats hashmap table");
					}
					const auto _index = (*_iter).second;

					// Save the ranking to stats table
					_s_table[_index]._nc_RankingElapsedAverage = j;
				}
			}

			// Save to this _Member_Ranking_AverageElapsed
			this->_Member_Ranking_AverageElapsed = std::move(_avgelp_pairs);
			return;

			// @TODO: two loops can be reduced to once.
		}

		// *(Kernel) Perform All Computation (in-place) (all threads) (API)
		inline void __fill_stat_calculation__() noexcept {
			this->__ttl_elapsed_pct_calc__();
			this->__ttl_elapsed_rnk_calc__();
			this->__avg_elapsed_rnk_calc__();
			return;
		}

		// *(Kernel) Calculate the Sorted Stat Table by Total Elapsed Time (in-place) (all threads) (API)
		inline void __sort_stat_table_byttl__() exceptional {
			// We create a new copied table instead of moved table
			_SizeType _size = this->_Member_Statistics_Table.size();
			_s_Performance_Stats_Type _sorted(_size);

			// For each table, we use the sorted sequence to assign lines
			for (_SizeType i = 0; i < _size; ++i) {
				// We assume everything is done before this step
				// aka: __fill_stat_calculation__() is done before calling this

				const auto& _s_table = this->_Member_Statistics_Table[i];
				const auto& _hashmap = this->_Member_Statistics_HashMap[i];

				// Already sorted (by default: descendingly)
				const auto& _hashes = this->_Member_Ranking_TotalElapsed[i].first;
				const auto& _values = this->_Member_Ranking_TotalElapsed[i].second;

				// Reserve elements for _sorted[i]
				auto& _new_table = _sorted[i];
				_new_table.reserve(_s_table.size());

				// We iterate over the _Member_Ranking_TotalElapsed table and use the ranking
				for (const auto& _hash : _hashes) {
					// Find out the index
					const auto _iter = _hashmap.find(_hash);
					if (_iter == _hashmap.end()) {
						throw _Timely_Exception("_Prototype_Timely_StatModels method __sort_stat_table_byttl__() is called and looking up for hash " + std::to_string(_hash) + " but not found in the stats hashmap table");
					}
					const auto _index = (*_iter).second;

					// Get a reference of the object and append to new table 
					const auto& _obj = _s_table[_index];
					_new_table.push_back(_obj);
				}
			}

			// Move to this->_Member_Statistics_Table_Sorted
			this->_Member_Statistics_Table_Sorted = std::move(_sorted);
			return;
		}

		// *(Kernel) Export A String Represented CSV Vectors from a table (all threads)
		inline _s_CSV_Files_Type __export_csv_vectors__(const _s_Performance_Stats_Type& tables, _CharType separator = ',', _CharType endline = '\n', _BoolType is_format = false) const noexcept {

			const auto _size = tables.size();
			if (_size == 0) {
				return _s_CSV_Files_Type{};
			}
			_s_CSV_Files_Type _csvs;
			_csvs.reserve(_size);

			// Convert seperator and endline to const string object
			const _StringType sep = { separator };
			const _StringType end = { endline };

			// Lambda: Add header into csv (inplace)
			auto _addheader = [&](_StringType& _content) noexcept -> void {
				// 1. Func Signature
				_content.append("FuncSig").append(sep);

				// 2. Func Identifier
				_content.append("FuncId").append(sep);

				// 3. Thread ID
				_content.append("Thread").append(sep);

				// 4. Called Count
				_content.append("Count").append(sep);

				// 5. Average Elapsed
				_content.append("AvgElapsed").append(sep);

				// 6. Totally Elapsed
				_content.append("TtlElapsed").append(sep);

				// 7. Min Elapsed
				_content.append("MinElapsed").append(sep);

				// 8. Max Elapsed
				_content.append("MaxElapsed").append(sep);

				// 9. Average Elapsed Ranking
				_content.append("AvgElaRnk").append(sep);

				// 10. Totally Elapsed Ranking
				_content.append("TtlElaRnk").append(sep);

				// 11. Totally Elapsed Percentage
				_content.append("TtlElaPctg").append(sep);

				// (Last - endl)
				_content.append(end);
			};

			// Lambda: Add one line into csv (inplace)
			auto _addline = [&](_StringType& _content, const _s_Statistics_Line_Type& line) noexcept -> void {
				
				// 1. Func Signature with quotes
				_content.append("\"").append(line._SignatureOfTimingUnit).append("\"").append(sep);

				// 2. Func Identifier
				if (is_format == false) {
					_content.append(_ToStringType(line._HashOfTimingUnit)).append(sep);
				}
				else {
					_content.append(this->__format_hex__(line._HashOfTimingUnit)).append(sep);
				}

				// 3. Thread ID
				_content.append(_ToStringType(line._NoOfThread)).append(sep);

				// 4. Called Count
				_content.append(_ToStringType(line._NoOfCount)).append(sep);

				// 5. Average Elapsed
				if (is_format == false) {
					_content.append(_ToStringType(line._ElapsedAverage)).append(sep);
				}
				else {
					_content.append(this->__format_ms__(line._ElapsedAverage)).append(sep);
				}

				// 6. Totally Elapsed
				if (is_format == false) {
					_content.append(_ToStringType(line._ElapsedTotally)).append(sep);
				}
				else {
					_content.append(this->__format_ms__(line._ElapsedTotally)).append(sep);
				}

				// 7. Min Elapsed
				if (is_format == false) {
					_content.append(_ToStringType(line._ElapsedMinimum)).append(sep);
				}
				else {
					_content.append(this->__format_ms__(line._ElapsedMinimum)).append(sep);
				}

				// 8. Max Elapsed
				if (is_format == false) {
					_content.append(_ToStringType(line._ElapsedMaximum)).append(sep);
				}
				else {
					_content.append(this->__format_ms__(line._ElapsedMaximum)).append(sep);
				}

				// 9. Average Elapsed Ranking
				_content.append(_ToStringType(line._nc_RankingElapsedAverage)).append(sep);

				// 10. Totally Elapsed Ranking
				_content.append(_ToStringType(line._nc_RankingElapsedTotal)).append(sep);

				// 11. Totally Elapsed Percentage
				if (is_format == false) {
					_content.append(_ToStringType(line._nc_ElapsedTotalPercentage)).append(sep);
				}
				else {
					_content.append(this->__format_percent__(line._nc_ElapsedTotalPercentage)).append("%").append(sep);
				}

				// (Last - endl)
				_content.append(end);
			};

			// Create a constant format csv for all threads
			for (_SizeType i = 0; i < _size; ++i) {
				// A reference of the current stat table
				const auto& _table = tables[i];
				const auto _n_size = _table.size();

				_StringType _content;
				// Reserve for a relatively long space table_elem * 257
				_content.reserve(_table.size() * 257);

				// Add header
				_addheader(_content);

				// For each line, add line
				for (_SizeType ln = 0; ln < _n_size; ++ln) {
					_addline(_content, _table[ln]);
				}

				// Append (MOVE) to _csvs
				_csvs.push_back(std::move(_content));
			}

			return _csvs;
		}

		// *(Kernel) Text File Hard Drive Writer
		inline void __write_newtxt__(const _StringType& filepath, const _StringType& data) const exceptional {

			// Open up the filepath
			std::fstream fp(filepath.c_str(), std::ios::in | std::ios::out | std::ios::trunc);
			if (fp.is_open() == false) {
				throw _Timely_Exception("_Prototype_Timely_StatModels method __write_newtxt__() is called but the file pointer was not correctly resolved/opened");
			}

			// Write and close
			try {
				fp.write(data.c_str(), data.size());
				fp.close(); 
			}
			catch (const std::exception& e) {
				// Handles all standard exceptions
				throw _Timely_Exception("_Prototype_Timely_StatModels method __write_newtxt__() is called but an error happened while writing to local disk. Error " + _StringType(e.what()));
			}
			catch (...) {
				// Catches any other types of exceptions (not derived from std::exception)
				throw _Timely_Exception("_Prototype_Timely_StatModels method __write_newtxt__() is called but an unknown error happened while writing to local disk");
			}

			return;
		}

		// *(Kernel) Print Short Version Stats to Standard Output (need Stats Table) (in one shot)
		inline void __print_stats__(const _s_Performance_Stats_Type& tables, _BoolType longer = false) const noexcept {
			const auto _size = tables.size();
			if (_size == 0) {
				return;
			}

			// Lambda: Print Header to STD output device
			auto _printheader = [&]() noexcept -> void {
				// 0. Number
				std::cout << std::left << std::setw(6) << "[No]";

				// 1. Func Signature
				std::cout << std::left << std::setw(46) << "Signature";

				// 3. Thread ID
				std::cout << std::right << std::setw(4) << "Tid";

				// 4. Called Count
				std::cout << std::right << std::setw(12) << "Count";

				// 6. Totally Elapsed
				std::cout << std::right << std::setw(12) << "T.Elapsed";

				if (longer) {
					// 11. Totally Elapsed Percentage
					std::cout << std::right << std::setw(12) << "T.Elapsed%";

					// 5. Average Elapsed
					std::cout << std::right << std::setw(12) << "A.Elapsed";
				}

				// Endline
				std::cout << std::endl;

				// ----
				std::cout << (longer ? _StringType(104ULL, '-') : _StringType(80ULL, '-')) << std::endl;
			};

			// Lambda: Print one line into STD output device
			auto _printline = [&](const _s_Statistics_Line_Type& line, const _IndexType& idx) noexcept -> void {
				
				// Show Case Level
				_SizeType _level = 0;
				if (std::round(line._nc_ElapsedTotalPercentage) >= 100) {
					_level = 3;
				}
				else if (line._nc_ElapsedTotalPercentage > 25) {
					_level = 2;
				}
				else if (line._nc_ElapsedTotalPercentage > 2) {
					_level = 1;
				}

				// Lambda - Set
				auto _l_set = [](_SizeType level) {
					if (level >= 3) {
						std::cout << color4::underline<_CharType> << color4::red<_CharType>;
					}
					else if (level == 2) {
						std::cout << color4::Color<char>(color4::CL_RGB(255, 165, 0));
					}
					else if (level == 1) {
						std::cout << color4::cyan<_CharType>;
					}
				};

				// Lambda - Reset
				auto _l_reset = [](_SizeType level) {
					if (level > 0) {
						if (level >= 3) {
							std::cout << color4::style_reset<_CharType> << color4::reset<_CharType>;
						}
						else {
							std::cout << color4::reset<_CharType>;
						}
					}
				};

				// Set Global Level to print lines
				_l_set(_level);

				// 0. Number
				std::cout << " " << std::left << std::setw(5) << idx;

				// 1. Func Signature
				if (line._SignatureOfTimingUnit.size() > 45) {
					std::cout << std::left << std::setw(46) << line._SignatureOfTimingUnit.substr(0, 42) + "...";
				}
				else {
					std::cout << std::left << std::setw(46) << line._SignatureOfTimingUnit;
				}				

				// 3. Thread ID
				std::cout << std::right << std::setw(4) << line._NoOfThread;

				// 4. Called Count
				std::cout << std::right << std::setw(12) << line._NoOfCount;

				// 6. Totally Elapsed
				std::cout << std::right << std::setw(12) << this->__format_ms__(line._ElapsedTotally);

				if (longer) {
					// 11. Totally Elapsed Percentage
					std::cout << std::right << std::setw(11) << this->__format_percent__(line._nc_ElapsedTotalPercentage) << "%";

					// 5. Average Elapsed
					std::cout << std::right << std::setw(12) << this->__format_ms__(line._ElapsedAverage);
				}

				// Reset if any
				_l_reset(_level);

				// Endline
				std::cout << std::endl;
			};

			// Create a constant format csv for all threads
			for (_SizeType i = 0; i < _size; ++i) {
				// A reference of the current stat table
				const auto& _table = tables[i];
				const auto _n_size = _table.size();

				// Thread Indicator
				const _StringType _thread_indicator = (i == 0 ? ("Main") : (_ToStringType(i)));

				// Print the Title and Thread Number
				std::cout << std::endl << std::endl;
				std::cout << "Timely (*) Runtime Stats [" << color4::green<_CharType> << this->_TitleName << color4::reset<_CharType> << "] " << color4::green<_CharType> << "Thread " << _thread_indicator << color4::reset<_CharType>;
				std::cout << std::endl;

				// Print header
				_printheader();

				// For each line, print line
				for (_SizeType ln = 0; ln < _n_size; ++ln) {
					// Exit if more than max limit lines
					if (ln >= __timelystatsmlns__) {
						std::cout << " (with "  << _n_size - __timelystatsmlns__ << " lines) ..." << std::endl;
						break;
					}

					_printline(_table[ln], ln);
				}
			}

			return;
		}

		// *(Kernel) Save Stats Std Summarization to a CSV File (need CSV Vectors) (in multiple csvs)
		inline void __save_to_csv__(const _s_CSV_Files_Type& csvs) const exceptional {
			const auto _size = csvs.size();
			if (_size == 0) {
				return;
			}

			// Iterate over csv vectors, and save them one by one
			for (_SizeType i = 0; i < _size; ++i) {
				_StringType filename = "Timely Runtime Stats Thread " + (i == 0 ? "Main" : _ToStringType(i)) + ".csv";
				this->__write_newtxt__(filename, csvs[i]);
			}
			return;
		}

		// *(Kernel) Save Stats Std Summarization to an HTML File (need CSV Vectors) (in one html)
		inline void __save_to_html__(const _s_CSV_Files_Type& csvs, const _StringType& style = "light") const exceptional {
			const auto _size = csvs.size();
			if (_size == 0) {
				return;
			}

			// Iterate over csv vectors, and create titles 
			_VectorType<_StringType> _table_titles;
			_table_titles.reserve(_size);
			for (_SizeType i = 0; i < _size; ++i) {
				_table_titles.push_back("Stats Thread " + (i == 0 ? "Main" : _ToStringType(i)));
			}

			// Create a HTML export
			_Stats_Backend_CSV_To_Html _html(csvs, _table_titles, ',', '\n', "Timely (*) Runtime Stats for Project " + this->_TitleName, style);
			_StringType _html_data = _html.gen();

			// Save the html data 
			_StringType filename = "Timely Runtime Stats Exportable.html";
			this->__write_newtxt__(filename, _html_data);

			return;
		}

	public:
		// Constructor
		explicit _Prototype_Timely_StatModels() noexcept = delete;
		explicit _Prototype_Timely_StatModels(const _StringType& title, const _s_Performance_Table_Type& table) noexcept :
			_Base_Timely(),
			_TitleName(title),
			_Reference_Performance_Table(table) {
		}
		explicit _Prototype_Timely_StatModels(_StringType&& title, const _s_Performance_Table_Type& table) noexcept :
			_Base_Timely(),
			_TitleName(std::move(title)),
			_Reference_Performance_Table(table) {
		}

		// Destructor
		virtual ~_Prototype_Timely_StatModels() noexcept {}

		// Generate Statistic Table and Perform Outputing Stuff
		const _s_Performance_Stats_Type& gen(_IntegerType save = 1, _IntegerType verbosity = 1, const _StringType style = "light") exceptional {
			// Param: save, whether to save results to csv and html
			//            for any non-0 values, regarded as true
			//            for 0 value, regarded as false
			// Param: verbosity, whether and how should the stats be printed out
			//            if 0, silent, do NOT print anything
			//            if 1, low-level verbosity, print essential results
			//            if 2, higher-level verbosity, print longer results
			//            otherwise, regarded as 0
			// Param: style, the style of html exportable, Literal["light", "dark"]
			//            otherwise, regarded as "light"

			// 1st step: Generate the base table and hash map
			auto table_n_hash = this->__rewrite_statstable_prim__(this->_Reference_Performance_Table);
			this->_Member_Statistics_Table = std::move(table_n_hash.first);
			this->_Member_Statistics_HashMap = std::move(table_n_hash.second);

			// 2nd step: Calculate Essentials in-place
			this->__fill_stat_calculation__();

			// 3rd step: Calculate the Sorted Table
			this->__sort_stat_table_byttl__();

			// If saving to Local File
			if (save != 0) {
				
				// Export to CSV Vectors
				auto _csvs = this->__export_csv_vectors__(this->_Member_Statistics_Table_Sorted, ',', '\n', false);
				auto _csfs = this->__export_csv_vectors__(this->_Member_Statistics_Table_Sorted, ',', '\n', true);

				// Save to CSV Files
				this->__save_to_csv__(_csvs);

				// Save to HTML Files
				this->__save_to_html__(_csfs, style); // Using the format version
			}

			// [Optional] Print a concise version on the screen
			if (verbosity == 1) {
				this->__print_stats__(this->_Member_Statistics_Table_Sorted, false);
			}
			else if (verbosity == 2) {
				this->__print_stats__(this->_Member_Statistics_Table_Sorted, true);
			}
			else {
				// Do nothing
			}

			return this->_Member_Statistics_Table_Sorted;
		}
	};

}
// End namespace libnocppsh

