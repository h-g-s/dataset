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
#include <cstdlib>

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

enum Datatype str_type(const string &str)
{
    bool hasNum = false;
    int nPoints = 0;
    bool hasAlpha = false;

    for ( size_t i=0 ; (i<str.size()) ; ++i )
    {
        if (str[i] == '.') 
            ++nPoints;
        else
        {
            if (isdigit(str[i]))
                hasNum = true;
            else
                return String;
        }
    }

    if (hasNum)
    {
        switch (nPoints)
        {
            case 0:
            {
                return Integer;
                break;
            }
            case 1:
            {
                return Float;
                break;
            }
            default:
            {
                return String;
                break;
            }
        }
    }
    
    return Empty;
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

Dataset::Dataset(const char *fileName) :
    data(nullptr)
{
    ifstream ifile;
    ifile.open(fileName, ios::in);

    vector<CSVRow> rows;

    CSVRow row;

    // reading header
    ifile >> row;
    this->headers = vector< string >(row.size());
    for (size_t i=0 ; (i<row.size()) ; ++i )
    {
        this->headers[i] = row[i];
        trim(this->headers[i]);
    }
    
    size_t n = this->headers.size();
    vector<set<string>> difValues = vector<set<string>>(n, set<string>());
    int line = 1;

    vector<vector<size_t>> colTypes = vector<vector<size_t>>(n, vector<size_t>(N_DATA_TYPES, 0));

    // for char types
    vector<size_t> colSizes = vector<size_t>(n, 0);

    while(ifile >> row)
    {
        if (row.size()!=n)
        {
            cerr << "error reading line " << line << \
            ", number of columns should be " << n << \
            " but there are " << row.size() << " columns." << endl;
            abort();
        }

        for ( size_t i=0 ; (i<n) ; ++i )
        {
            difValues[i].insert(row[i]);
            ++colTypes[i][str_type(row[i])];
            colSizes[i] = max(colSizes[i], row[i].size());
        }

        ++line;
    }
    ifile.close();

    this->rowSize = 0;
    
    // checking columns that can be safely deleted
    std::vector< bool > deleteColumn = vector< bool >(n, false);
    for ( size_t i=0 ; (i<difValues.size()) ; ++i )
    {
        size_t nDel = 0;
        cout << difValues[i].size() << endl;
        if (difValues[i].size()<=1)
        {
            deleteColumn[i] = true;
            nDel++;
        }
    }
    
    size_t idx = 0;
    for ( size_t i=0 ; (i<n); ++i )
    {
        if (deleteColumn[i])
            continue;
        if (colTypes[i][String])
        {
            this->cTypes[idx] = String;
            this->cSizes[idx] = colSizes[i]+1;
        }
        else
        {
            if (colTypes[i][Float])
            {
                this->cTypes[idx] = Float;
                this->cSizes[idx] = sizeof(double);
            }
            else
            {
                this->cTypes[idx] = Integer;
                this->cSizes[idx] = sizeof(int);
            }
        }

        this->headers[idx] = this->headers[i];
        this->rowSize += this->cSizes[idx];
        ++idx;
    }
    
    n = idx;
    this->headers.resize(n);
    this->cTypes.resize(n);
    this->cSizes.resize(n);

    this->rows = line;
    size_t dataSize = this->rowSize*line;
    this->data = malloc(dataSize);
    if (this->data==nullptr)
    {
        cerr << "No memory for dataset." << endl;
        abort();
    }

}

Dataset::~Dataset()
{
    if (this->data)
        free(this->data);
}

