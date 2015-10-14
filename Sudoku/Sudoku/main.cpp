//
//  main.cpp
//  Sudoku
//
//  Created by Vincent on 10/13/15.
//  Copyright (c) 2015 VJP. All rights reserved.
//

#include <iostream>
#include <map>

using namespace std;

typedef map<pair<short,short>, short> Grid;

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

void PrintGrid(const Grid& g)
{
	for(short y=1; y<10; y++)
		{
		for(short x=1; x<10; x++)
			{
			auto v = givenValues.find(make_pair(x, y));

			if(v != givenValues.end())	cout << v->second << ' ';
			else cout << "* ";
			}
		cout << endl;
		}
}

int main(int argc, const char * argv[])
{
	PrintGrid(givenValues);
    return 0;
}
