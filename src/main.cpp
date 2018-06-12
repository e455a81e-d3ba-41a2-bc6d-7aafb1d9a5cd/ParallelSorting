#include <iostream>
#include <omp.h>
#include <quicksort.h>
#include <mergesort.h>
#include <random>
#include <chrono>
#include <execution>
#include <chrono>

#ifdef NDEBUG
const size_t TEST_SIZE = 10000000;
#else
const size_t TEST_SIZE = 100;
#endif

template <size_t size>
void fill_vector(std::vector<int>& v)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	const auto dist_value = size / 2;
	std::uniform_int_distribution<> dist(-dist_value, dist_value);
	generate_n(back_inserter(v), size, bind(dist, gen));
}

template<typename ForwardIterator>
void assert_is_sorted(ForwardIterator first, ForwardIterator last)
{
	while (first != (last - 1))
	{
		volatile auto previous = *first;
		volatile auto current = *(first + 1);
		if (previous > current)
		{
			throw std::exception("Not sorted!");
		}
		++first;
	}
}


double TestMergeSort()
{
	std::vector<int> v;
	v.reserve(TEST_SIZE);
	fill_vector<TEST_SIZE>(v);

	const auto start = std::chrono::steady_clock::now();

	mergesort(std::begin(v), std::end(v));
	assert_is_sorted(std::begin(v), std::end(v));

	const auto end = std::chrono::steady_clock::now();
	const auto diff = end - start;
	return std::chrono::duration<double, std::milli>(diff).count();
}

double TestQuickSort()
{
	std::vector<int> v;
	v.reserve(TEST_SIZE);
	fill_vector<TEST_SIZE>(v);

	const auto start = std::chrono::steady_clock::now();

	quicksort(std::begin(v), std::end(v));
	assert_is_sorted(std::begin(v), std::end(v));

	const auto end = std::chrono::steady_clock::now();
	const auto diff = end - start;
	return std::chrono::duration<double, std::milli>(diff).count();
}

double TestStdSort()
{
	std::vector<int> v;
	v.reserve(TEST_SIZE);
	fill_vector<TEST_SIZE>(v);


	const auto start = std::chrono::steady_clock::now();

	std::sort(std::execution::par, std::begin(v), std::end(v));

	const auto end = std::chrono::steady_clock::now();
	const auto diff = end - start;
	return std::chrono::duration<double, std::milli>(diff).count();
}

int main(int argc, char* argv[])
{
	auto previous_result = std::numeric_limits<double>::max();
	std::cout << "Sorting " << TEST_SIZE << " values." << std::endl;

	{
		std::cout << "Testing Quicksort: " << std::endl;

		auto testrun_result = 0.0;
		for (auto i = 0; i < 10; ++i) {
			testrun_result = TestQuickSort();
			previous_result = std::min(previous_result, testrun_result);
		}
		std::cout << "Best result: " << previous_result << " ms" << std::endl;
		std::cout << "------------------------------------------------" << std::endl;
	}
	{
		previous_result = std::numeric_limits<double>::max();
		std::cout << "Testing Mergesort: " << std::endl;

		auto testrun_result = 0.0;
		for (auto i = 0; i < 10; ++i) {
			testrun_result = TestMergeSort();
			previous_result = std::min(previous_result, testrun_result);
		}

		std::cout << "Best result: " << previous_result << " ms" << std::endl;
		std::cout << "------------------------------------------------" << std::endl;
	}
	{
		previous_result = std::numeric_limits<double>::max();
		std::cout << "Testing std:sort(): " << std::endl;

		auto testrun_result = 0.0;
		for (auto i = 0; i < 10; ++i) {
			testrun_result = TestStdSort();
			previous_result = std::min(previous_result, testrun_result);
		}
		std::cout << "Best result: " << previous_result << " ms" << std::endl;
	}
	std::getchar();

	return 0;
}
