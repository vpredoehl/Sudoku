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
    {{4,1}, 9}, {{6,1}, 4},
    {{2,2}, 3}, {{8,2}, 9},
    {{1,4}, 1}, {{5,4}, 6}, {{9,4}, 2},
    {{4,5}, 4}, {{6,5}, 2},
    {{1,6}, 6}, {{5,6}, 3}, {{9,6}, 4},
    {{2,8}, 5}, {{8,8}, 4},
    {{4,9}, 7}, {{6,9}, 5}
    
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
ostream& operator<<(ostream& o, const RegionList &rl)
{
    o << endl;
    for(auto y : rows)
    {
        for(auto x : columns)
        {
            auto regionIter = find_if(rl.begin(), rl.end(), [x,y](const Region &r)  {   return r.end() != r.find({x,y});    });
            
            if(regionIter != rl.end())	cout << "* ";
            else cout << ". ";
        }
        cout << endl;
    }
    return o;
}


set<short> FindEligibleDigitsInRegionForColumn(short col)
{
    set<short> ret;
    
    return ret;
}

set<short> FindDigitsForPoint(const Grid& g, Point p)
{
	short pX = p.first, pY = p.second;

	set<short> u,i;
    
	for(auto x: columns)
    {
        auto xPos = g.find({x,pY});	// find value at this Point
        if(xPos != g.end())	u.insert(xPos->second);
    }
	for(auto y: rows)
    {
        auto yPos = g.find({pX,y});
        if(yPos != g.end())	u.insert(yPos->second);
    }
    
        // find 3x3 region that contains pX, pY
    auto regionIter = find_if(regions.begin(), regions.end(), [pX,pY](const Region &s)   {      return s.find({pX,pY}) != s.end();  });
    for(auto p : *regionIter)
    {
        auto regionPointIter = g.find(p);
        if(regionPointIter != g.end())  u.insert(regionPointIter->second); // add digits from grid found in region
    }
    
        // find eligible digits by subtracting the used digits
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
    
        // check digits in regions
    for(auto r: regions)
    {
        vector<short> d;
        Grid::const_iterator gIter;
        
        for(auto p: r)  // each point in region
            if((gIter = g.find(p)) != g.end())    d.push_back(gIter->second);
        sort(d.begin(), d.end());
        d.erase(unique(d.begin(), d.end()), d.end());
        if(d.size() != digits.size())
            return false;
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
		while(!cs.empty() && (aSolution = find_if(cs.begin(), cs.end(), IsSolution)) != cs.end())
		{
			#ifdef verbose
				cout << "Unique solution at " << aSolution << solutionGrid << endl;
			#endif
			solutionGrid[aSolution->first] = *aSolution->second.begin();
			cs.erase(aSolution);
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
    cout << "pi Regions: " << piRegions << endl << endl;
    
    sort(piDigits.begin(), piDigits.end()); // for set_intersect
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
