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
#include <vector>
using namespace std;

vector<Grid> found;

//#define verbose	// show solution steps

Grid givenValues =	// {{x,y}, value}
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
ostream& operator<<(ostream& o, Point v)	{	o << "{" << v.first << "," << v.second << "}";	return o;	}
ostream& operator<<(ostream& o, set<short> v)	{	for(auto e : v) cout << e << ", "; return o;	}
ostream& operator<<(ostream& o, EligibleDigits::const_iterator v)	{	cout << v->first << " Value: " << v->second; return o;	}
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
	set_difference(digits.begin(), digits.end(), u.begin(), u.end(), inserter(i, i.begin()));
	#ifdef verbose
		cout << "Possible digits at " << p << " are " << i << endl << endl;
	#endif
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

		// check integrity of rows/columns ( each digit used exactly once )
	for(auto y: rows)
	{
		vector<short> d;

		for(auto x: columns)
		{
			Grid::const_iterator p;
			if((p = g.find({x,y})) != g.end())	d.push_back(p->second);
		}
		sort(d.begin(), d.end());
		d.erase(unique(d.begin(), d.end()), d.end());
		if(d.size() != digits.size())
			return false;	// digit used more than once
	}

	for(auto x: columns)
	{
		vector<short> d;

		for(auto y: rows)
		{
			Grid::const_iterator p;
			if((p = g.find({x,y})) != g.end()) d.push_back(p->second);
		}
		sort(d.begin(), d.end());
		d.erase(unique(d.begin(), d.end()), d.end());
		if(d.size() != digits.size())
			return false;	// digit used more than once
	}

		// make sure given values are in the right place
	return includes(g.begin(), g.end(), givenValues.begin(), givenValues.end());
}

Grid FindPossibleSolution(const Grid& g)
{
	auto IsSolution = [](EligibleDigits::const_reference v) noexcept -> bool 	{	return v.second.size() == 1;	};	// point is a solution if only one value is possible
	Grid solutionGrid = g, trialSolution;
	EligibleDigits cs;

	do
	{
		try	{	cs = FindEligibleDigits(solutionGrid);	}
		catch(GotStuck p)
		{
			#ifdef verbose
				cout << "Got stuck: " << p << endl;
			#endif
			break;
		}

			// look for solutions ( sets of eligible digits with only one value )
		EligibleDigits::iterator aSolution;
		if(!cs.empty() && (aSolution = find_if(cs.begin(), cs.end(), IsSolution)) != cs.end())
		{
			#ifdef verbose
				cout << "Unique solution at " << aSolution << solutionGrid << endl;
			#endif
			solutionGrid[aSolution->first] = *aSolution->second.begin();
			cs.erase(aSolution);
            continue;
		}

			// have to start trying combinations.
		try	{	cs = FindEligibleDigits(solutionGrid);	}
		catch(GotStuck p)
		{
			#ifdef verbose
				cout << "Got stuck: " << p << endl;
			#endif
			break;
		}

		aSolution = min_element(cs.begin(), cs.end(),	// Start with squares with least count of eligible digits
				// compare number of eligible digits
			[](EligibleDigits::const_reference v1, EligibleDigits::const_reference v2) noexcept -> bool  {		return v1.second.size() < v2.second.size();	});

		if(aSolution != cs.end())
		{
			for(auto s: aSolution->second)
			{
				solutionGrid[aSolution->first] = s;
				#ifdef verbose
					cout << "Trying Solution: " << s << " from " << aSolution << solutionGrid << endl;
				#endif
				try
				{
					trialSolution = FindPossibleSolution(solutionGrid);
					if(isSolved(trialSolution))	throw FoundSolution(trialSolution);
				}
				catch(FoundSolution s)
				{
					#ifdef verbose
						cout << "Found Solution: " << s << endl;
					#endif
					found.push_back(s);
				}
			}
			cs.erase(aSolution);
		}
	} while(!cs.empty());	// Go through all the combinations

	return solutionGrid;	// ** compiler should use move constructor **
}


int main(int argc, const char * argv[])
{
	cout << "given Values: " << givenValues << endl << endl;
	FindPossibleSolution(givenValues);

	if(!found.empty())
	{
			// remove duplicate solutions
		sort(found.begin(), found.end());
		found.erase(unique(found.begin(), found.end()), found.end());

		cout << found.size() << " solutions found." << endl;
		for_each(found.begin(), found.end(), [](vector<Grid>::const_reference v)	{	cout << "Solution: " << v << endl;	});
//		copy(found.begin(), found.end(), ostream_iterator<Grid>(cout,"\n"));
	 }
	else
		cout << "No solution found!!" << endl;

	return 0;
}
