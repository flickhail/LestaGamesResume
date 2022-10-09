#include "QuickSort.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

// Creates a simple timer.
// Requires a floating-point type
template <typename T>
	requires std::is_floating_point_v<T>
class Timer
{
public:
	Timer() = default;
	Timer(const Timer& timer) = default;
	Timer(Timer&& timer) = delete;
	~Timer() = default;

	// Resets the timer to zero
	void Reset() noexcept
	{
		m_Beginning = clock_type::now();
	}

	// Returns the value of timer in seconds
	T Elapsed() const noexcept
	{
		return std::chrono::duration_cast<second_type>(clock_type::now() - m_Beginning).count();
	}

private:
	using clock_type = std::chrono::steady_clock;
	using second_type = std::chrono::duration<T, std::ratio<1>>;

	std::chrono::time_point<clock_type> m_Beginning{ clock_type::now() };
};

void PrintArray(const int* array, size_t size)
{
	std::cout << "\n\n";
	for (size_t i{ 0 }; i < size; ++i)
	{
		std::cout << array[i] << ' ';
		if (i % 20 == 0 && i != 0)
			std::cout << '\n';
	}

	std::cout << "\n\n";
}

// Tests whether the array is correctly sorted or not
bool QuickSortTest(int* array, size_t size)
{
	for (int i{}; i < size; ++i)
	{
		if ( i != (size - 1) && array[i] > array[i + 1])
		{
			std::cout << "\n\n\nTEST FAILED ON INDEX: " << i << '\n';
			return false;
		}
	}

	return true;
}


// Fills the given array with random numbers
void FillWithRand(int* array, size_t size)
{
	std::mt19937* randGen{ new std::mt19937{ static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count())} };
	std::uniform_int_distribution range{ 0, 1000 };

	for (int i{ 0 }; i < size; ++i)
		array[i] = range(*randGen);

	delete randGen;
}


// Compares the QuickSort() and std::sort algorithms by sorting time
void StressTest(int arraySize, int iterations)
{
	int* array = new int[arraySize];
	Timer<float> timer{};

	for (int i{}; i < iterations; ++i)
	{
		float resultTime{ 0 };

		FillWithRand(array, arraySize);
		std::cout << "std::sort(): ";
		timer.Reset();
		std::sort(array, array + arraySize);
		resultTime = timer.Elapsed();
		std::cout << resultTime << " seconds\n";

		FillWithRand(array, arraySize);
		std::cout << "QuickSort(): ";
		timer.Reset();
		QuickSort(array, arraySize);
		resultTime = timer.Elapsed();
		std::cout << resultTime << " seconds\n\n";
		QuickSortTest(array, arraySize);
	}

	delete[] array;
}

int main()
{
	std::cout << "TESTING WITH A MILLION RANDOM ELEMENTS IN THE ARRAY: \n";
	StressTest(1000000, 3);

	std::cout << "\nTESTING WITH A 2 MILLION RANDOM ELEMENTS IN THE ARRAY: \n";
	StressTest(2000000, 3);

	std::cout << "\nTESTING WITH A 3 MILLION RANDOM ELEMENTS IN THE ARRAY: \n";
	StressTest(3000000, 3);

	std::cout << "\nTESTING WITH A 4 MILLION RANDOM ELEMENTS IN THE ARRAY: \n";
	StressTest(4000000, 3);

	std::cout << "\nTESTING WITH A 40 MILLION RANDOM ELEMENTS IN THE ARRAY: \n";
	StressTest(40000000, 3);
	return 0;
}