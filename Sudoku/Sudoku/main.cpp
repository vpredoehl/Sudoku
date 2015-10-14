//
//  main.cpp
//  Sudoku
//
//  Created by Vincent on 10/13/15.
//  Copyright (c) 2015 VJP. All rights reserved.
//

#include "Sudoku.h"

#include <iostream>
#include <algorithm>

using namespace std;

Grid givenValues =
{
	{{1,1}, 3}, {{4,1},4}, {{5,1}, 8}, {{6,1}, 5}, {{7,1},7}, {{8,1},2},
	{{1,2},5}, {{4,2},3}, {{5,2},7}, {{7,2},6}, {{8,2},9},
	{{2,3},2}, {{6,3},6},
	{{2,4},8}, {{3,4},4}, {{5,4},6}, {{6,4},3},
	{{4,5},1}, {{6,5},9},
	{{4,6},8}, {{5,6},2}, {{7,6},4}, {{8,6},3},
	{{4,7},6}, {{8,7},4},
	{{2,8},7}, {{3,8},8}, {{5,8},5}, {{6,8}, 1}, {{9,8},3},
	{{2,9},1}, {{3,9},6}, {{4,9},7}, {{5,9},3}, {{6,9},4}, {{9,9},2}
};

	// output convenience
ostream& operator<<(ostream& o, Point v)	{	o << "x: " << v.first << " y: " << v.second;	return o;	}
ostream& operator<<(ostream& o, set<short> v)	{	for(auto e : v) cout << e << ", "; return o;	}


set<short> FindDigitsForPoint(Point p)
{
	cout << "Finding candidates for point " << p << endl;
	short pX = p.first, pY = p.second;

	set<short> u,i;	// union of all givens on row pY

	for(auto x: columns)
		if(pX != x)
		{
			auto xPos = givenValues.find({x,pY});	// find value at this Point
			if(xPos != givenValues.end())	u.insert(xPos->second);
		}
	for(auto y: rows)
		if(pY != y)
		{
			auto yPos = givenValues.find({pX,y});
			if(yPos != givenValues.end())	u.insert(yPos->second);
		}
	cout << "Found: " << u << endl << endl;

	set_difference(digits.begin(), digits.end(), u.begin(), u.end(), inserter(i, i.begin()));

	cout << "Eligible: " << i << endl << endl;

	return i;
}

EligibleDigits FindEligibleDigits(const Grid& g)
{
	EligibleDigits e;
	for(auto y: rows)	for(auto x: columns)
		if(givenValues.find({x,y}) == givenValues.end())
				// not a given value
			e[{x,y}] = FindDigitsForPoint({x,y});
	return e;
}

void PrintGrid(const Grid& g)
{
	for(auto y : rows)
		{
			for(auto x : columns)
			{
				auto v = givenValues.find(make_pair(x, y));

				if(v != givenValues.end())	cout << v->second << ' ';
				else cout << "* ";
			}
			cout << endl;
		}
	cout << endl << endl;
}



int main(int argc, const char * argv[])
{
	PrintGrid(givenValues);

	Grid solution = givenValues;
	EligibleDigits cs = FindEligibleDigits(givenValues);

    return 0;
}
