//
//  Sudoku.h
//  Sudoku
//
//  Created by Vincent on 10/13/15.
//  Copyright (c) 2015 VJP. All rights reserved.
//

#ifndef Sudoku_Sudoku_h
#define Sudoku_Sudoku_h

#include <map>
#include <set>
#include <array>
#include <vector>

using std::pair;
using std::map;
using std::set;
using std::array;
using std::vector;


using Point = pair<short,short>;
using Grid = map<Point, short>;
using EligibleDigits = map<Point, set<short>>;
using Region = set<Point>;

const array<short,9> columns = { 1,2,3,4,5,6,7,8,9 },
	rows = columns,
	digits = rows;
const vector<Region> regions =  // coordinates for the nine 3x3 regions
{
    {
        {1,1}, {2,1}, {3,1},
        {1,2}, {2,2}, {3,2},
        {1,3}, {2,3}, {3,3}
    },
    {
        {4,1}, {5,1}, {6,1},
        {4,2}, {5,2}, {6,2},
        {4,3}, {5,3}, {6,3}
    },
    {
        {7,1}, {8,1}, {9,1},
        {7,2}, {8,2}, {9,2},
        {7,3}, {8,3}, {9,3}
    },
    {
        {1,4}, {2,4}, {3,4},
        {1,5}, {2,5}, {3,5},
        {1,6}, {2,6}, {3,6}
    },
    {
        {4,4}, {5,4}, {6,4},
        {4,5}, {5,5}, {6,5},
        {4,6}, {5,6}, {6,6}
    },
    {
        {7,4}, {8,4}, {9,4},
        {7,5}, {8,5}, {9,5},
        {7,6}, {8,6}, {9,6}
    },
    {
        {1,7}, {2,7}, {3,7},
        {1,8}, {2,8}, {3,8},
        {1,9}, {2,9}, {3,9}
    },
    {
        {4,7}, {5,7}, {6,7},
        {4,8}, {5,8}, {6,8},
        {4,9}, {5,9}, {6,9}
    },
    {
        {7,7}, {8,7}, {9,7},
        {7,8}, {8,8}, {9,8},
        {7,9}, {8,9}, {9,9}
    }
};



	// Exceptions
struct GotStuck
{
	Point p;

	GotStuck(Point p) : p{p} {}
};	// for throw

struct FoundSolution
{
	Grid s;

	FoundSolution(Grid &s) : s{s}	{}
	operator Grid() const	{	return s;	}
};

#endif
