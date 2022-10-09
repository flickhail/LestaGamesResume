#include "QuickSort.hpp"
#include <algorithm>

// The little modification to QuickSort algorithm.
// Using this under certain conditions leads to improved speed and stack memory consumption
static void InsertionSort(int* array, int arraySize) {
	for (int i{ 1 }; i < arraySize; i++) {
		int j = i;
		while (j > 0 && array[j - 1] > array[j]) {
			std::swap(array[j - 1], array[j]);
			j--;
		}
	}
}

// The QuickSort algorithm
void QuickSort(int* array, int arraySize)
{
	// "30" is the array size from which the insertion sorting algorithm should start.
	// this will be slightly faster than pure quicksort and also helps with the stack overflow problem
	if (arraySize <= 30)
	{
		InsertionSort(array, arraySize);
		return;
	}

	int leftBorderIndex{ 0 };
	int rightBorderIndex{ arraySize - 1 };

	int pivotValue{ array[rightBorderIndex / 2] };

	while (leftBorderIndex <= rightBorderIndex)
	{
		while (pivotValue < array[rightBorderIndex])
			rightBorderIndex--;

		while (array[leftBorderIndex] < pivotValue)
			leftBorderIndex++;

		if (leftBorderIndex <= rightBorderIndex)
		{
			int tmp{ array[leftBorderIndex] };
			array[leftBorderIndex] = array[rightBorderIndex];
			array[rightBorderIndex] = tmp;

			leftBorderIndex++;
			rightBorderIndex--;
		}
	}

	QuickSort(array, rightBorderIndex + 1);
	QuickSort(&array[leftBorderIndex], arraySize - leftBorderIndex);
}