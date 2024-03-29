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


	printf("\n****FIRST ROW OF TABLE****\n");
	//*********************** #2 first row
	int lPeak = -1; // left-most peak in this row. Needed to have circular bridging. Useful in part #3

	cell * fr = table[0];
	
	//first cell.
	fr[0].l = n - 1;
	fr[0].r = 1;
	if (isPeak(nums, n, fr, 0))
	{
		lPeak = 0;
		rpeaks[0] = 0;
	}

	for (int i = 1; i < n - 1; i++) //from second to penultimate
	{
		fr[i].l = i - 1;
		fr[i].r = i + 1;

		if (isPeak(nums, n, fr, i))
		{
			rpeaks[0] = i;
			if(lPeak < 0)
				lPeak = i;
		}
	}

	//last cell.
	fr[n - 1].l = n - 2;
	fr[n - 1].r = 0;
	if (isPeak(nums, n, fr, n-1))
	{
		if (lPeak < 0)
			lPeak = n-1;
		rpeaks[0] = n - 1;
	}

	printArray(nums, fr, n);

	printf("\n****REST OF TABLE****\n");

	//*********************** #3 build rest of table
	for (int vi = 1; vi < tableH; vi++) //vertical index
	{
		cell * prow = table[vi - 1];
		cell * row = table[vi];

		int previousLPeak = lPeak; //Remember that int lPeak is the left-most peak in this row. 
		lPeak = -1;
		for (int hi = previousLPeak; hi < n && prow[hi].r != previousLPeak; hi = prow[hi].r) //horizontal index, moving using cell.r values
		{
			if (isPeak(nums, n, prow, hi))
			{//promote cell to next row
				if(lPeak < 0)
					lPeak = hi;
				
				int lastRPeak = rpeaks[vi];//index of last r peak
				if (lastRPeak >= 0)
				{
					row[hi].l = lastRPeak;
					row[hi].r = lPeak;
					row[lastRPeak].r = hi;
				}
				else
				{//first cell of this row
					row[hi].l = hi;
					row[hi].r = hi;
				}

				rpeaks[vi] = hi; // new right most peak
			}
		}

		row[lPeak].l = rpeaks[vi]; // closing up first l with last r, circular bridging.

		printArray(nums, row, n);
	}

	printf("\n****R PEAKS****\n");
	printArray(rpeaks, tableH);

	int * output = new int[n];
	int outIndex = 0;

	printf("\n****BRIDGING****\n");

	//*********************** #4 iteratively find the min peak and update table - bridging
	for (int vi = tableH-1; vi >= 0;) //vertical index
	{
		int minPeak = rpeaks[vi]; // will always be the current min.
		rpeaks[vi] = -1;
		if (minPeak >= 0)
		{
			vi = tableH - 1;
			//writing output array :D
			output[outIndex] = nums[minPeak];
			printf("out[%d] = %d\n", outIndex, nums[minPeak]);
			++outIndex;

			//bridging
			bool bottom = false;
			for (int vii = 0; vii <= vi && !bottom; vii++) //vertical index of bridging
			{
				if (rpeaks[vii] == minPeak)
					rpeaks[vii] = -1;//remove index from peaks array

				cell * row = table[vii];
				cell mpcell = row[minPeak];

				if (!isLastCellOfRow(mpcell, minPeak))
				{
					//let's bridge current row
					row[mpcell.l].r = mpcell.r;
					row[mpcell.r].l = mpcell.l;

					//let's cascade update all bridges
					int cascadingCellRow = vii;
					int cascadingCellCol = -1;

					bool cascadeRight;//cascade update will always update in only one direction, depending on the first new peak, if there is one
					if (isPeak(nums, n, row, mpcell.r))
					{
						cascadeRight = true;
						cascadingCellCol = mpcell.r; //will search on the right
					}
					else if (isPeak(nums, n, row, mpcell.l))
					{
						cascadeRight = false; //will search on the left.
						cascadingCellCol = mpcell.l;
					}

					if (rpeaks[cascadingCellRow] < cascadingCellCol)
						rpeaks[cascadingCellRow] = cascadingCellCol;

					cell cascadingCell = table[cascadingCellRow][cascadingCellCol];
					if (!isLastCellOfRow(cascadingCell, cascadingCellCol))
					{
						while (cascadingCellCol >= 0 && cascadingCellCol != minPeak)
						{
							cascadingCell = table[cascadingCellRow][cascadingCellCol];
							cell * upperRow = table[cascadingCellRow];
							cell * lowerRow = table[cascadingCellRow + 1];

							if (cascadeRight)
							{
								/*
								if (cascadingCell.r == minPeak)
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
								*/
								{
									//insert right and check if goes down!
									int rightCell = lowerRow[minPeak].r;
									lowerRow[cascadingCellCol].l = minPeak;
									lowerRow[cascadingCellCol].r = rightCell;

									if (lowerRow[rightCell].l == lowerRow[rightCell].r)
										lowerRow[rightCell].r = cascadingCellCol;//handling case in which cell is pointing to itself
									
									lowerRow[rightCell].l = cascadingCellCol; //updating l value of right cell of lower row
									if (isPeak(nums, n, lowerRow, rightCell))
										cascadingCellCol = rightCell;
								}
							}
							else
							{//cascade left
								/*
								if (cascadingCell.l == minPeak)
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
								*/
								{
									//insert right and check if goes down!
									int leftCell = lowerRow[minPeak].l;
									lowerRow[cascadingCellCol].r = minPeak;
									lowerRow[cascadingCellCol].l = leftCell;

									if (lowerRow[leftCell].l == lowerRow[leftCell].r)
										lowerRow[leftCell].l = cascadingCellCol;//handling case in which cell is pointing to itself

									lowerRow[leftCell].r = cascadingCellCol; //updating r value of left cell of lower row
									if (isPeak(nums, n, lowerRow, leftCell))
										cascadingCellCol = leftCell;
								}
							}

							cascadingCellRow++;
						}
					}
					else
					{
						rpeaks[cascadingCellRow] = cascadingCellCol;
						/*
						if(vi == 0 && outIndex < n)//hammer
							vi++;//we have to redo this line in the last step, the cell is zero.
							*/
					}
				}
				else
				{//bottom of table!
				}
			}
		}
		else
		{//row is collapsing
			vi--;
		}
	}

	printf("\n\n\nOUTPUT:\n");
	printArray(output, n);
}

bool isPeak(int * nums, int n, cell * tableRow, int index)
{
	cell c = tableRow[index];
	return nums[c.l] >= nums[index] && nums[index] <= nums[c.r];
}

bool isLastCellOfRow(cell c, int index)
{
	return c.l == index && c.r == index;
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
cell cmp(int * nums, int l, int x, int r)
{
	cell c;
	c.l = l;
	c.r = r;
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

void printArray(int * nums, cell * cells, int n)
{
	printf("[ ");
	for (int i = 0; i < n; ++i) {
		if (cells[i].l > INT32_MIN || cells[i].r > INT32_MIN)
		{
			if(isPeak(nums, n, cells, i))
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
