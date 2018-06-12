#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <algorithm>
#include <functional>
#include <ppltasks.h>

template<typename Iterator, typename Compare = std::less<>>
void quicksort(Iterator begin, Iterator end, Compare compare = {})
{
	static std::atomic<int> task_number = 0;

	if (begin == end)
		return;

	auto split = std::partition(begin + 1, end, std::bind(compare, std::placeholders::_1, *begin));
	//Use first element as pivot and partion the list accordingly
	std::iter_swap(begin, split - 1); //put pivot element in the "middle" 

	if(task_number >= 16)
	{
		quicksort(begin, split - 1, compare);
		quicksort(split, end, compare); 
		return;
	}

	auto task1 = concurrency::create_task([begin, split, &compare]()
	{
		task_number++;
		quicksort(begin, split - 1, compare);
	});

	auto task2 = concurrency::create_task([split, end, &compare]()
	{
		task_number++;
		quicksort(split, end, compare); 
	});

	task1.wait();
	task_number--;
	task2.wait();
	task_number--;
}
#endif
