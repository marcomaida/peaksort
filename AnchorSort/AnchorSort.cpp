// AnchorSort.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include "AnchorSort.h"

struct cell
{
	int l = INT32_MIN;
	int r = INT32_MIN;
};


int main()
{
	int n = 7;
	int max = 20;
	int * nums = getRandom(n, max);
	printArray(nums, n);
	peakSort(nums, n);
	printf("\n\n\n\n\n"); 
}

/* function to generate and return random numbers */
int * getRandom(int n, int max) {

	int * r = new int[n];
	for (int i = 0; i < n; ++i) {
		r[i] = rand() % max;
	}
	return r;
}


void peakSort(int * nums, int n)
{
	int tableH = ceil(log2(n)); // in each step, table is at least halved
	cell ** table = new cell*[tableH]; // table of peaks
	int * rpeaks = new int[tableH]; // right most peak in each row


	//*********************** #1  init table and peaks array
	for (int i = 0; i < tableH; i++)
	{
		table[i] = new cell[n];
		rpeaks[i] = -1;
	}


	printf("\n****R FIRST ROW OF TABLE****\n");
	//*********************** #2 first row
	int lPeak = -1; // left-most peak in this row. Needed to have circular bridging. Useful in part #3

	cell * fr = table[0];
	
	fr[0] = cmp(nums[n - 1], 1, nums[0], nums[1], 1); //first and last cell are done this way because of circular bridging
	if (isPeak(fr[0]))
	{
		lPeak = 0;
		rpeaks[0] = 0;
	}

	for (int i = 1; i < n - 1; i++)
	{
		fr[i] = cmp(nums[i - 1], 1, nums[i], nums[i + 1], 1);
		if (isPeak(fr[i]))
		{
			rpeaks[0] = i;
			if(lPeak < 0)
				lPeak = i;
		}
	}


	fr[n - 1] = cmp(nums[n - 2], 1, nums[n - 1], nums[0], 1); //first and last cell are done this way because of circular bridging
	if (isPeak(fr[n - 1]))
	{
		if (lPeak < 0)
			lPeak = n-1;
		rpeaks[0] = n - 1;
	}

	printArray(fr, n);

	printf("\n****REST OF TABLE****\n");

	//*********************** #3 build rest of table
	for (int vi = 1; vi < tableH; vi++) //vertical index
	{
		cell * prow = table[vi - 1];
		cell * row = table[vi];

		int firstPeakIndex = (rpeaks[vi - 1] + abs(prow[rpeaks[vi - 1]].r)) % n; //using circular bridging to find first peak in the previous row
		int previousLPeak = lPeak; //Remember that int lPeak is the left-most peak in this row. 
		lPeak = -1;
		for (int hi = previousLPeak; hi < n && abs(prow[hi].r) > 0; hi += abs(prow[hi].r)) //horizontal index, moving using cell.r values
		{
			if (isPeak(prow[hi]))
			{//promote cell to next row
				if(lPeak < 0)
					lPeak = hi;
				
				int lastRPeak = rpeaks[vi];//index of last r peak
				if (lastRPeak >= 0)
				{
					row[hi] = cmp(nums[lastRPeak], hi - lastRPeak, nums[hi], nums[lPeak], (lPeak + n - hi) % n);
					row[lastRPeak].r = -row[hi].l;
				}
				else
				{//first cell of this row
					row[hi].l = 0;
					row[hi].r = 0;
				}

				rpeaks[vi] = hi; // new right most peak
			}
		}

		row[lPeak].l = -row[rpeaks[vi]].r; // closing up first l with last r, circular bridging.

		printArray(row, n);
	}

	printf("\n****R PEAKS****\n");
	printArray(rpeaks, tableH);

	int * output = new int[n];
	int outIndex = 0;

	printf("\n****BRIDGING****\n");

	//*********************** #4 iteratively find the min peak and update table - bridging
	for (int vi = tableH-1; vi >= 0; vi--) //vertical index
	{
		int minPeak = rpeaks[vi]; // will always be the current min.
		rpeaks[vi] = -1;
		if (minPeak >= 0)
		{
			//writing output array :D
			output[outIndex] = nums[minPeak];
			printf("out[%d] = %d\n", outIndex, nums[minPeak]);
			++outIndex;

			//bridging
			bool bottom = false;
			for (int vii = 0; vii <= vi && !bottom; vii++) //vertical index of bridging
			{
				cell * row = table[vii];
				int li = lCell(row, n, minPeak);
				int ri = rCell(row, n, minPeak);

				//let's bridge current row
				row[li].r = (abs(row[li].r) + abs(row[minPeak].r)) % n;
				if (nums[li] < nums[ri])
					row[li].r *= -1;
				row[ri].l = -row[li].r;

				if (rpeaks[vii] == minPeak)
					rpeaks[vii] = -1;//remove index from peaks array

				//let's cascade update all bridges
				int cascadingCellRow = vii;
				int cascadingCellCol = -1;

				bool cascadeRight = true;//cascade update will always update in only one direction, depending on the first new peak, if there is one
				if (isPeak(row[ri])) 
					cascadingCellCol = ri; //will search on the right
				else if (isPeak(row[li]))
				{
					cascadeRight = false; //will search on the left.
					cascadingCellCol = li;
				}

				if (rpeaks[cascadingCellRow] < cascadingCellCol)
					rpeaks[cascadingCellRow] = cascadingCellCol;

				if (!isZero(table[cascadingCellRow][cascadingCellCol])) //if it's zero, then no cascading is necessary: this is the only cell!
				{
					while (cascadingCellCol >= 0)
					{
						cell * upperRow = table[cascadingCellRow];
						cell * lowerRow = table[cascadingCellRow + 1];

						if (cascadeRight)
						{
							int currentRight = rCell(lowerRow, n, minPeak);

							if (currentRight == minPeak)
							{//special case: only the minPeak and the cascading number are left in this row!
								lowerRow[cascadingCellCol].l = 0;
								lowerRow[cascadingCellCol].r = 0;
								rpeaks[cascadingCellRow + 1] = cascadingCellCol;
								if (vi < cascadingCellRow + 1)
									vi = cascadingCellRow + 2;
								cascadingCellCol = -1;
								bottom = true;
							}
							else
							{
								//bridge right and check if goes down!
								lowerRow[cascadingCellCol].l = upperRow[cascadingCellCol].l; // left will stay the same as the upper row
								lowerRow[cascadingCellCol].r = abs(upperRow[minPeak].r) + abs(upperRow[cascadingCellCol].r);
								int rCellIndex = (cascadingCellCol + lowerRow[cascadingCellCol].r) % n;
								if (nums[cascadingCellCol] < nums[rCellIndex])
									lowerRow[cascadingCellCol].r *= -1; //it's smaller, become negative
								lowerRow[rCellIndex].l = -lowerRow[cascadingCellCol].r;

								if (isPeak(lowerRow[rCellIndex]))
									cascadingCellCol = rCellIndex;
							}
						}
						else
						{//cascade left
							int currentLeft = lCell(lowerRow, n, minPeak);

							if (currentLeft == minPeak)
							{//special case: only the minPeak and the cascading number are left in this row!
								lowerRow[cascadingCellCol].l = 0;
								lowerRow[cascadingCellCol].r = 0;
								rpeaks[cascadingCellRow + 1] = cascadingCellCol;
								if (vi < cascadingCellRow + 1)
									vi = cascadingCellRow + 2;
								cascadingCellCol = -1;
								bottom = true;
							}
							else
							{
								//bridge right and check if goes down!
								lowerRow[cascadingCellCol].r = upperRow[cascadingCellCol].r; // right will stay the same as the upper row
								lowerRow[cascadingCellCol].l = abs(upperRow[minPeak].l) + abs(upperRow[cascadingCellCol].l);
								int lCellIndex = (cascadingCellCol - lowerRow[cascadingCellCol].l + n) % n;
								if (nums[cascadingCellCol] < nums[lCellIndex])
									lowerRow[cascadingCellCol].l *= -1; //it's smaller, become negative
								lowerRow[lCellIndex].r = -lowerRow[cascadingCellCol].l;

								if (isPeak(lowerRow[lCellIndex]))
									cascadingCellCol = lCellIndex;
							}
						}

						cascadingCellRow++;
					}
				}
				else
				{
					if(vi == 0 && outIndex < n)//hammer
						vi++;//we have to redo this line in the last step, the cell is zero.
				}
			}
		}
		else
		{/*row is collapsing*/
		}
	}

	printf("\n\n\nOUTPUT:\n");
	printArray(output, n);
}

bool isPeak(cell c)
{
	return c.l <= 0 && c.r <= 0;
}

bool isZero(cell c)
{
	return c.l == 0 && c.r == 0;
}

int lCell(cell * row, int hSize, int index)
{
	return (index - abs(row[index].l) + hSize) % hSize;
}

int rCell(cell * row, int hSize, int index)
{
	return (index + abs(row[index].r)) % hSize;
}


/*
Given numbers and distances, creates the corresponding cell.
-The sign of each value indicase if x is greater or smaller
-The value indicates the distance
*/
cell cmp(int l, int ldist, int x, int r, int rdist)
{
	cell c;

	if (l < x)
		c.l = ldist;
	else
		c.l = -ldist;

	if (r < x)
		c.r = rdist;
	else
		c.r = -rdist;

	return c;
}

void printArray(int * nums, int n)
{
	printf("[");
	for (int i = 0; i < n; ++i) {
		printf("%d", nums[i]);
		if (i < n - 1)
			printf(",");
	}
	printf("]\n");
}

void printArray(cell * cells, int n)
{
	printf("[ ");
	for (int i = 0; i < n; ++i) {
		if (cells[i].l > INT32_MIN || cells[i].r > INT32_MIN)
		{
			if(isPeak(cells[i]))
				printf("<%d,%d>", cells[i].l, cells[i].r);
			else
				printf("(%d,%d)", cells[i].l, cells[i].r);
			if (i < n - 1)
				printf(" ");
		}
	}
	printf("]\n");
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
