/*
 * Instance.cpp
 *
 *  Created on: 24 de fev de 2019
 *      Author: haroldo
 */



#include "Dataset.hpp"

#include <fstream>
#include <vector>
#include <set>

using namespace std;

#include <algorithm>
#include <cctype>
#include <locale>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

enum Datatype { Integer, Float, String };

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}


class CSVRow
{
    public:
        std::string const& operator[](std::size_t index) const
        {
            return m_data[index];
        }
        std::size_t size() const
        {
            return m_data.size();
        }
        void readNextRow(std::istream& str)
        {
            std::string         line;
            std::getline(str, line);

            std::stringstream   lineStream(line);
            std::string         cell;

            m_data.clear();
            while(std::getline(lineStream, cell, ','))
            {
            	trim(cell);
                m_data.push_back(cell);
            }
            // This checks for a trailing comma with no data after it.
            if (!lineStream && cell.empty())
            {
                // If there was a trailing comma then add an empty element.
                m_data.push_back("");
            }
        }
    private:
        std::vector<std::string>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}


Dataset::Dataset(const char *fileName)
{
	ifstream ifile;
	ifile.open(fileName, ios::in);

	string line;

	vector<CSVRow> rows;

    CSVRow row;

    vector<set<string>> difValues;

    while(ifile >> row)
    {
    	if (row.size()>difValues.size())
    		difValues.resize(row.size());
    	for ( size_t i=0 ; (i<row.size()) ; ++i )
    		difValues[i].insert(row[i]);
    	rows.push_back(row);
    }
	ifile.close();

	for ( size_t i=0 ; (i<difValues.size()) ; ++i )
		cout << difValues[i].size() << endl;
}

Dataset::~Dataset()
{
}
