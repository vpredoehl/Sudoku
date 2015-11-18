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

using std::pair;
using std::map;
using std::set;
using std::array;


using Point = pair<short,short>;
using Grid = map<Point, short>;
using EligibleDigits = map<Point, set<short>>;
using Region = set<Point>;
using RegionSet = set<Region>;

const array<short,9> columns = { 1,2,3,4,5,6,7,8,9 },
	rows = columns,
	digits = rows;


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
