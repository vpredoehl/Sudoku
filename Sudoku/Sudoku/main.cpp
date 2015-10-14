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

struct GotStuck
{
	Point p;

	GotStuck(Point p) : p{p} {}
};	// for throw


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
ostream& operator<<(ostream& o, EligibleDigits::const_iterator v)	{	cout << "Point: " << v->first << " Value: " << v->second; return o;	}
ostream& operator<<(ostream& o, GotStuck p)	{	cout << p.p; return o;	}
ostream& operator<<(ostream& o, const Grid& g)
{
	o << endl;
	for(auto y : rows)
		{
		for(auto x : columns)
			{
			auto v = g.find({x,y});

			if(v != g.end())	cout << v->second << ' ';
			else cout << "* ";
			}
		cout << endl;
		}
	return o;
}

set<short> FindDigitsForPoint(const Grid& g, Point p)
{
	cout << "Finding candidates for point " << p << endl;
	short pX = p.first, pY = p.second;

	set<short> u,i;	// union of all givens on row pY

	for(auto x: columns)
		if(pX != x)
		{
			auto xPos = g.find({x,pY});	// find value at this Point
			if(xPos != g.end())	u.insert(xPos->second);
		}
	for(auto y: rows)
		if(pY != y)
		{
			auto yPos = g.find({pX,y});
			if(yPos != g.end())	u.insert(yPos->second);
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
		if(g.find({x,y}) == g.end())
			// point has no value in grid
		{
			set<short> s = FindDigitsForPoint(g, {x,y});

			if(s.empty())	throw GotStuck({x,y});
			e[{x,y}] = s;
		}
	return e;
}

bool isSolved(const Grid& g)
{
	for(auto y: rows)	for(auto x: columns)
		if(g.find({x,y}) == g.end())	return false;	// found an empty square
	return true;
}

Grid FindPossibleSolution(const Grid& g)
{
	auto IsSolution = [](EligibleDigits::const_reference v) noexcept -> bool 	{	return v.second.size() == 1;	};	// point is a solution if only one value is possible
	Grid solutionGrid = g, trialSolution;
	EligibleDigits cs;

	do
	{
		try	{	cs = FindEligibleDigits(solutionGrid);	}
		catch(GotStuck p)	{	cout << "Got stuck: " << p << endl;	break;	}

			// look for solutions ( sets of eligible digits with only one value )
		EligibleDigits::iterator aSolution;
		bool foundSolution = false;
		while((aSolution = find_if(cs.begin(), cs.end(), IsSolution)) != cs.end())
		{
			foundSolution = true;
			cout << "Found Solution at " << aSolution << endl;
			solutionGrid[aSolution->first] = *aSolution->second.begin();
			cout << "New partial solution:" << solutionGrid << endl << endl;
			cs.erase(aSolution);
		}
		if(foundSolution)
			continue;

			// have to start trying combinations.
		try	{	cs = FindEligibleDigits(solutionGrid);	}
		catch(GotStuck p)	{	cout << "Got stuck: " << p << endl;	break;	}

		aSolution = min_element(cs.begin(), cs.end(),	// Start with squares with least count of eligible digits
				// compare number of eligible digits
			[](EligibleDigits::const_reference v1, EligibleDigits::const_reference v2) noexcept -> bool  {		return v1.second.size() < v2.second.size();	});

		for(auto s: aSolution->second)
		{
			solutionGrid[aSolution->first] = s;
			cout << "Trying Solution: " << s << " from " << aSolution << solutionGrid << endl;

			trialSolution = FindPossibleSolution(solutionGrid);
			cout << "Trial Solution:" << trialSolution << endl;
			if(isSolved(trialSolution))
			{
				solutionGrid = move(trialSolution);	// move semantics
				break;
			}
		}
	} while(!isSolved(solutionGrid));

	return solutionGrid;	// ** compiler should use move constructor **
}

int main(int argc, const char * argv[])
{
	cout << "given Values: " << givenValues << endl << endl;
	Grid s = FindPossibleSolution(givenValues);

	if(isSolved(s))
		cout << "The solution is: " << s << endl;
	else
		cout << "No solution found!!" << endl;

	return 0;
}
