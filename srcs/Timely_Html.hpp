// Timely_Html.hpp
// 
// Version 0.0.1 built 20250706
//
// HTML Exporting Tools of 
//   Timely Library - 
//   A Header Only Light-weight Benchmark and Analysis Toolkit for C++
// 
// CC DOF Studio 2025

#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>

#include "Timely_Define.hpp"
#include "Timely_Hash.hpp"
#include "Timely_Base.hpp"

#pragma once

// Start namespace libnocppsh
namespace libnocppsh :: inline libtimely {
	
	// _Stats_Backend_CSV_To_Html
	// What is this Conventor?
	// It is an utility class that converts csv tables into a beautiful html
	// used by _Prototype_Timely_StatModels to present the Performance Stats Table
	// into an easy-to-use Html that users can browse and inspect in their web broswers.
	class _Stats_Backend_CSV_To_Html : public _Base_Timely {

	protected:
		// (member) Data of CSV Tables
		_VectorType<_StringType> _Member_CSV_Tables;

		// (member) Titles of CSV Tables
		_VectorType<_StringType> _Member_CSV_Table_Titles;

		// (member) CSV Separator (by default ',')
		_CharType _CSV_Char_Separator = ',';

		// (member) CSV Endline (by defaylt '\n')
		_CharType _CSV_Char_Endline = '\n';

		// (member) HTML Page Title String
		_StringType _Html_Page_Title;

		// (member) HTML Page Theme String Representation
		_StringType _Html_Theme; // {"light", "dark"}

	public:
		// Constructor
		explicit _Stats_Backend_CSV_To_Html() noexcept = delete;
		explicit _Stats_Backend_CSV_To_Html(
			const _VectorType<_StringType>& csv_tables,
			const _VectorType<_StringType>& table_titles,
			_CharType separator = ',',
			_CharType endline = '\n',
			const _StringType& html_title = "",
			const _StringType& theme = "light") exceptional:
		_Base_Timely(),
			_Member_CSV_Tables(csv_tables),
			_Member_CSV_Table_Titles(table_titles),
			_CSV_Char_Separator(separator),
			_CSV_Char_Endline(endline),
			_Html_Page_Title(html_title),
			_Html_Theme(theme) {
			// If size does not match, throw exception
			if (_Member_CSV_Tables.size() != _Member_CSV_Table_Titles.size()) {
				throw _Timely_Exception("_Stats_Backend_CSV_To_Html method constructor() is called while number of tables and titles does not match");
			}
			if (_Html_Theme != "light" && _Html_Theme != "dark") {
				// By default, we use light
				_Html_Theme = "light";
			}
		}

		// Destructor
		virtual ~_Stats_Backend_CSV_To_Html() noexcept {}

	private:
		// Create Html Cascade Style Sheet (CSS)
		_StringType __stylecss__() const {
			// light theme
			if (_Html_Theme == "light") {
				return R"(
                body { background-color: white; color: black; font-family: Arial, sans-serif; }
                table { border-collapse: collapse; width: 100%; margin-bottom: 20px; }
                th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                th { background-color: #ADD8E6; color: black; }
                tr:nth-child(even) { background-color: #f2f2f2; }
                ul { list-style-type: none; padding: 0; }
                li { display: inline; margin-right: 10px; }
                a { color: #0000FF; text-decoration: none; }
                a:hover { text-decoration: underline; }
            )";
			}
			else {
				// dark theme
				return R"(
                body { background-color: #333; color: white; font-family: Arial, sans-serif; }
                table { border-collapse: collapse; width: 100%; margin-bottom: 20px; }
                th, td { border: 1px solid #555; padding: 8px; text-align: left; }
                th { background-color: #00008B; color: white; }
                tr:nth-child(even) { background-color: #444; }
                ul { list-style-type: none; padding: 0; }
                li { display: inline; margin-right: 10px; }
                a { color: #1E90FF; text-decoration: none; }
                a:hover { text-decoration: underline; }
            )";
			}
		}

		// Replace Special Characters by HTML Escape Characters
		_StringType __escape_html__(const _StringType& s) const {
			_StringType result;
			result.reserve(s.size());
			for (_CharType c : s) {
				switch (c) {
				case '&': result += "&amp;"; break;
				case '<': result += "&lt;"; break;
				case '>': result += "&gt;"; break;
				case '"': result += "&quot;"; break;
				case '\'': result += "&#39;"; break;
				default: result += c; break;
				}
			}
			return result;
		}

	public:
		// Run and generate an html stored in a string
		_StringType gen() const {
			std::ostringstream html;
			html << "<!DOCTYPE html>\n";
			html << "<html>\n";
			html << "<head>\n";
			html << "<title>" << __escape_html__(_Html_Page_Title) << "</title>\n";
			html << "<style>\n";
			html << __stylecss__();
			html << "</style>\n";
			html << "</head>\n";
			html << "<body>\n";

			// Create Navigation menu
			html << "<ul>\n";
			for (_SizeType i = 0; i < _Member_CSV_Table_Titles.size(); ++i) {
				html << "<li><a href=\"#table" << i << "\">" << __escape_html__(_Member_CSV_Table_Titles[i]) << "</a></li>\n";
			}
			html << "</ul>\n";

			// Write Tables
			for (_SizeType i = 0; i < _Member_CSV_Tables.size(); ++i) {
				html << "<table id=\"table" << i << "\">\n";
				html << "<caption style=\"font-weight: bold; \">" << __escape_html__(_Member_CSV_Table_Titles[i]) << " </caption>\n";
				std::istringstream iss(_Member_CSV_Tables[i]);
				_StringType row;
				_VectorType<_StringType> headers;
				bool first_row = true;
				while (std::getline(iss, row, _CSV_Char_Endline)) {
					if (row.empty()) continue;
					html << "<tr>\n";
					std::istringstream row_stream(row);
					_StringType cell;
					_SizeType col_index = 0;
					_StringType row_name;
					while (std::getline(row_stream, cell, _CSV_Char_Separator)) {
						if (first_row) {
							headers.push_back(cell);

							if (col_index == 0) {
								// Left Alignment
								html << "<th style=\"text-align: left;\">" << __escape_html__(cell) << "</th>\n";
							}
							else {
								// Right Alignment
								html << "<th style=\"text-align: right;\">" << __escape_html__(cell) << "</th>\n";
							}
						}
						else {
							if (col_index == 0) {
								row_name = cell;

								// Left Alignment
								_StringType tooltip = "Row: " + __escape_html__(row_name) + ", Column: " + __escape_html__(headers[col_index]);
								html << "<td title=\"" << tooltip << "\" style=\"text-align: left;\">" << __escape_html__(cell) << "</td>\n";
							}
							else {
								// Right Alignment
								_StringType tooltip = "Row: " + __escape_html__(row_name) + ", Column: " + __escape_html__(headers[col_index]);
								html << "<td title=\"" << tooltip << "\" style=\"text-align: right;\">" << __escape_html__(cell) << "</td>\n";
							}
							
						}
						col_index++;
					}
					html << "</tr>\n";
					first_row = false;
				}
				html << "</table>\n";
			}

			html << "</body>\n";
			html << "</html>\n";
			return html.str();
		}
	};
}
// End namespace libnocppsh
