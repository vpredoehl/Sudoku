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

#define verbose	// show solution steps

Grid givenValues =	// {{x,y}, value}
{
//    {{1,1}, 3}, {{4,1},4}, {{5,1}, 8}, {{6,1}, 5}, {{7,1},7}, {{8,1},2},
//    {{1,2},5}, {{4,2},3}, {{5,2},7}, {{7,2},6}, {{8,2},9},
//    {{2,3},2}, {{6,3},6},
//    {{2,4},8}, {{3,4},4}, {{5,4},6}, {{6,4},3},
//    {{4,5},1}, {{6,5},9},
//    {{4,6},8}, {{5,6},2}, {{7,6},4}, {{8,6},3},
//    {{4,7},6}, {{8,7},4},
//    {{2,8},7}, {{3,8},8}, {{5,8},5}, {{6,8}, 1}, {{9,8},3},
//    {{2,9},1}, {{3,9},6}, {{4,9},7}, {{5,9},3}, {{6,9},4}, {{9,9},2}
    

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

set<short> FindDigitsForPoint(const Grid& g, Point p)
{
    set<short> u,i, availableDigits;
	short pX = p.first, pY = p.second;

        // remove digits already used by pi region
    auto piRegionIter = find_if(piRegions.begin(), piRegions.end(), [pX,pY](const Region &r)    {   return r.find({pX,pY}) != r.end();  });
    if(piRegionIter != piRegions.end())
            // pX, pY is in a pi region
            // find availableDigits constrained by pi region
    {
        multiset<short> usedInPi;
        for(auto p : *piRegionIter) // traverse points in region
        {
            auto gridIter = g.find(p);
            if(gridIter != g.end())    usedInPi.insert(gridIter->second);
        }
        set_difference(piDigits.begin(), piDigits.end(), usedInPi.begin(), usedInPi.end(), inserter(availableDigits, availableDigits.begin()));
    }
    else
        availableDigits  = { digits.begin(), digits.end()  };

    
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
    auto regionIter = find_if(blocks.begin(), blocks.end(), [pX,pY](const Region &s)   {      return s.find({pX,pY}) != s.end();  });
    for(auto p : *regionIter)
    {
        auto gridIter = g.find(p);
        if(gridIter != g.end())  u.insert(gridIter->second); // add digits from grid found in region
    }
        // find eligible digits by subtracting the used digits
	set_difference(availableDigits.begin(), availableDigits.end(), u.begin(), u.end(), inserter(i, i.begin()));
	return i;
}

auto FindEligiblePiDigits(const Grid& g) -> EligibleDigits
{
    EligibleDigits e;
        // start with pi regions first because they are more restrictive
    for(auto r : piRegions)
        for(auto p : r) // each point in region
            if(g.find(p) == g.end())
            {
                set<short> s = FindDigitsForPoint(g, p);
                if(s.empty())   throw GotStuck(p);
                    e[p] = s;
            }
    return e;
}

EligibleDigits FindEligibleDigits(const Grid& g)
{
	EligibleDigits e;
    
	for(auto y: rows)	for(auto x: columns)
            // only try points without a value and NOT in a pi region
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

//    cout << "IsSolved: " << g << endl;
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
    
        // check blocks
    for(auto b: blocks)
    {
        vector<short> d;
        Grid::const_iterator gIter;
        
        for(auto p: b)  // each point in region
            if((gIter = g.find(p)) != g.end())    d.push_back(gIter->second);
        sort(d.begin(), d.end());
        d.erase(unique(d.begin(), d.end()), d.end());
        if(d.size() != digits.size())
            return false;
    }
    
        // check pi regions
    for(auto r : piRegions)
    {
        multiset<short> d;
        
        for(auto p : r)
        {
            auto gridIter = g.find(p);
            d.insert(gridIter->second);
        }
            // compared used digits against pi digits
        if(d != multiset<short>{ piDigits.begin(), piDigits.end() })
            return false;
    }

		// make sure given values are in the right place
	return includes(g.begin(), g.end(), givenValues.begin(), givenValues.end());
}

Grid FindPossibleSolution(const Grid& g)
{
    using FindDigitFunct = auto (*)(const Grid&) -> EligibleDigits;
    auto Permutate = [&g](FindDigitFunct FindDigits) -> Grid
    {
        auto IsSolution = [](EligibleDigits::const_reference v) noexcept -> bool 	{	return v.second.size() == 1;	};	// point is a solution if only one value is possible
        Grid solutionGrid = g, trialSolution;
        EligibleDigits cs;

        do
        {
            try	{	cs = FindDigits(solutionGrid);	}
            catch(GotStuck p)
            {
//                cout << "Got stuck: " << p << endl << solutionGrid << endl;
                throw;
            }

                // look for solutions ( sets of eligible digits with only one value )
            EligibleDigits::iterator aSolution = cs.begin();
            while(!cs.empty() && (aSolution = find_if(aSolution, cs.end(), IsSolution)) != cs.end())
            {
                solutionGrid[aSolution->first] = *aSolution->second.begin();
//                cout << "Unique solution at " << aSolution << solutionGrid << endl;
                aSolution = cs.erase(aSolution);
                auto regionIter = find_if(piRegions.begin(), piRegions.end(), [aSolution](const Region &r)   {   return r.find(aSolution->first) != r.end();    });
                if(regionIter != piRegions.end())
                {
                    trialSolution = FindPossibleSolution(solutionGrid);
                    if(isSolved(trialSolution))	throw FoundSolution(trialSolution);
                }
            }

                // have to start trying combinations.
            try	{	cs = FindDigits(solutionGrid);	}
            catch(GotStuck p)
            {
//                cout << "Got stuck: " << p << endl << solutionGrid << endl;
                throw;
            }

            aSolution = min_element(cs.begin(), cs.end(),	// Start with squares with least count of eligible digits
                    // compare number of eligible digits
                [](EligibleDigits::const_reference v1, EligibleDigits::const_reference v2) noexcept -> bool  {		return v1.second.size() < v2.second.size();	});

            if(aSolution != cs.end())
            {
                for(auto s: aSolution->second)
                {
                    solutionGrid[aSolution->first] = s;
//                    cout << "Possible solution " << s << " from " << aSolution << solutionGrid << endl;
                    try
                    {
                        trialSolution = FindPossibleSolution(solutionGrid);
                        if(isSolved(trialSolution))	throw FoundSolution(trialSolution);
                    }
                    catch(GotStuck)
                    {
//                        cout << "reverting back to: " << solutionGrid << endl;
                        continue;
                    }
                    catch(FoundSolution s)
                    {
//                        cout << "Found Solution: " << s << endl;
                        found.push_back(s);
                    }
                }
                aSolution = cs.erase(aSolution);
            }
        } while(!cs.empty());	// Go through all the combinations
        return solutionGrid;	// ** compiler should use move constructor **
    };
    
    Permutate(FindEligiblePiDigits);
    return Permutate(FindEligibleDigits);
}


int main(int argc, const char * argv[])
{
	cout << "given Values: " << givenValues << endl << endl;
    cout << "pi Regions: " << piRegions << endl << endl;
    
    
    sort(piDigits.begin(), piDigits.end()); // for set_intersect
    try {   FindPossibleSolution(givenValues);  }
    catch(GotStuck) {   cout << "Done!\n";    }
    catch(FoundSolution s)
    {
        cout << "Found Solution: " << s << endl;
        found.push_back(s);
    }

	if(!found.empty())
	{
			// remove duplicate solutions
        cout << found.size() << " solutions found." << endl;
		sort(found.begin(), found.end());
		found.erase(unique(found.begin(), found.end()), found.end());

		cout << found.size() << " unique solutions found." << endl;
		for_each(found.begin(), found.end(), [](vector<Grid>::const_reference v)	{	cout << "Solution: " << v << endl;	});
//		copy(found.begin(), found.end(), ostream_iterator<Grid>(cout,"\n"));
	 }
	else
		cout << "No solution found!!" << endl;

	return 0;
}
