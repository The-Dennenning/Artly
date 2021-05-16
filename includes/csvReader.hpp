/********************************************************************************************
	CS410 - Advanced C++ Final Project

	CSV Reader Implementation

		Original Design by Steven Libby @ slibby.pdx.edu
		Modified and Commented by Chris Denning

	This file implements functionality to read a csv file provided by the user 
		splits csv by newline into lines
		splits lines by comma into items 
		returns vector of vector of strings --> collection of lines of items

********************************************************************************************/

#include<functional>
#include<algorithm>
#include<iterator>
#include<vector>
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<streambuf>

//Split
//	Takes begin and end iterator,
//	Takes Predicate p to test when to split
//	Takes Function f to apply when predicate is true
template<typename FWD, typename PRED, typename F>
void split(FWD begin, FWD end, PRED p, F f)
{
    FWD current = begin;
    FWD next = begin;
    while(next != end)
    {
        next = find_if(current, end, p);
        f(current, next);
        if(next != end)
        {
            ++next;
            current = next;
        }
    }
}

//Reads csv found at fileName
//Returns 2D vector of strings
//
//SegFault if CSV is not well formatted
//SegFault if CSV is not found
std::vector<std::vector<std::string>> readCSV(std::string fileName)
{
	std::ifstream data(fileName);

	//Throw exception if file isn't found
	if (!data.good())
		throw std::invalid_argument( "User Provided File Does Not Exist" );

	std::stringstream buffer;
	buffer << data.rdbuf();
	std::string input = buffer.str();

    std::vector<std::vector<std::string>> csv;

    split(input.begin(), input.end(), 
    [](char c) {return c == '\n';},
    [&csv](auto line_begin, auto line_end)
    {
        std::vector<std::string> row;
        bool quoted = false;
        split(line_begin, line_end,
        [&quoted](char c)
        {
            if(c == '\"') quoted = !quoted;
            return !quoted && c == ',';
        },
        [&row](auto cell_begin, auto cell_end)
        {
            row.push_back(std::string(cell_begin, cell_end));
        });
        csv.push_back(row);
    });
    
    data.close();

    return csv;
}

void writeCSV(std::vector<std::vector<std::string>> data, std::string fileName)
{
    std::ofstream out(fileName);

    for (int i = 0; i < data.size(); i++)
    {
        for (int j = 0; j < data[i].size(); j++)
        {
            if (j + 1 != data[i].size())
                out << data[i][j] << ",";
            else
                out << data[i][j] << std::endl;
        }
    }

    out.close();
}