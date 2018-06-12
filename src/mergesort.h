#ifndef MERGESORT_H
#define MERGESORT_H

#include <algorithm>
#include <functional>
#include <vector>
#include <iterator>
#include <ppltasks.h>

namespace merge::impl
{
	template <typename Iterator, typename BufferIterator, typename Distance>
	Iterator rotate(Iterator first,
	                 Iterator middle,
	                 Iterator last,
	                 Distance len1, Distance len2,
	                 BufferIterator buffer_begin,
	                 Distance buffer_size)
	{
		BufferIterator buffer_end;
		if (len1 > len2 && len2 <= buffer_size)
		{
			if (len2)
			{
				buffer_end = std::move(middle, last, buffer_begin);
				std::move_backward(first, middle, last);
				return std::move(buffer_begin, buffer_end, first);
			}
			return first;
		}

		if (len1 <= buffer_size)
		{
			if (len1)
			{
				buffer_end = std::move(first, middle, buffer_begin);
				std::move(middle, last, first);
				return std::move_backward(buffer_begin, buffer_end, last);
			}
			return last;
		}
		std::rotate(first, middle, last);
		std::advance(first, std::distance(middle, last));
		return first;
	}

	template <typename BufferIterator, typename Iterator, typename OutputIterator, typename Compare>
	void move_merge(BufferIterator buffer_begin, BufferIterator buffer_end, Iterator first, Iterator last, OutputIterator result, Compare comp)
	{
		while (buffer_begin != buffer_end && first != last)
		{
			if (comp(*first, *buffer_begin))
			{
				*result = std::move(*first);
				++first;
			}
			else
			{
				*result = std::move(*buffer_begin);
				++buffer_begin;
			}
			++result;
		}
		if (buffer_begin != buffer_end)
			std::move(buffer_begin, buffer_end, result);
	}


	template <typename Iterator, typename BufferIterator, typename OutputIterator, typename Compare>
	void move_merge_backward(Iterator first1,
	                         Iterator last1,
	                         BufferIterator buffer_begin,
	                         BufferIterator buffer_end,
	                         OutputIterator result,
	                         Compare comp)
	{
		if (first1 == last1)
		{
			std::move_backward(buffer_begin, buffer_end, result);
			return;
		}
		if (buffer_begin == buffer_end)
			return;

		--last1;
		--buffer_end;
		while (true)
		{
			if (comp(*buffer_end, *last1))
			{
				*--result = std::move(*last1);
				if (first1 == last1)
				{
					std::move_backward(buffer_begin, ++buffer_end, result);
					return;
				}
				--last1;
			}
			else
			{
				*--result = std::move(*buffer_end);
				if (buffer_begin == buffer_end)
					return;
				--buffer_end;
			}
		}
	}

	template <typename Iterator, typename Distance, typename BufferIterator, typename Compare>
	void merge(Iterator first, Iterator middle, Iterator last, Compare compare,
	           Distance left_group_length,
	           Distance right_group_length,
	           BufferIterator buffer, Distance buffer_size)
	{
		if (left_group_length <= right_group_length && left_group_length <= buffer_size)
		{
			BufferIterator buffer_end = std::move(first, middle, buffer);
			move_merge(buffer, buffer_end, middle, last, first, compare);
		}
		else if (right_group_length <= buffer_size)
		{
			BufferIterator buffer_end = std::move(middle, last, buffer);
			move_merge_backward(first, middle, buffer, buffer_end, last, compare);
		}
		else
		{
			Iterator first_cut = first;
			Iterator second_cut = middle;
			Distance left_group_midpoint = 0;
			Distance right_group_midpoint = 0;

			if (left_group_length > right_group_length)
			{
				left_group_midpoint = left_group_length / 2;
				std::advance(first_cut, left_group_midpoint);
				second_cut = std::lower_bound(middle, last, *first_cut, compare);
				right_group_midpoint = std::distance(middle, second_cut);
			}
			else
			{
				right_group_midpoint = right_group_length / 2;
				std::advance(second_cut, right_group_midpoint);
				first_cut = std::upper_bound(first, middle, *second_cut, compare);
				left_group_midpoint = std::distance(first, first_cut);
			}

			Iterator new_middle = rotate(first_cut, middle, second_cut, left_group_length - left_group_midpoint, right_group_midpoint, buffer, buffer_size);

			merge(first, first_cut, new_middle, compare, left_group_midpoint, right_group_midpoint, buffer, buffer_size);
			merge(new_middle, second_cut, last, compare, left_group_length - left_group_midpoint, right_group_length - right_group_midpoint, buffer, buffer_size);
		}
	}

	template <typename Iterator, typename Compare>
	void inplace_merge(Iterator first, Iterator middle, Iterator last, Compare compare)
	{
		if (first == middle || middle == last)
			return;

		typedef typename std::iterator_traits<Iterator>::difference_type distance_type;

		distance_type len1 = std::distance(first, middle);
		distance_type len2 = std::distance(middle, last);

		typedef std::vector<typename std::iterator_traits<Iterator>::value_type> buffer_type;
		buffer_type buffer(first, last);

		merge(first, middle, last, compare, len1, len2, buffer.begin(), distance_type(buffer.size()));
	}
}

template <typename Iterator, typename Compare = std::less<>>
void mergesort(Iterator begin, Iterator end, Compare compare = {})
{
	static std::atomic<int> task_number = 0;

	if (end - begin <= 1)
		return;

	const auto middle = std::next(begin, (end - begin) / 2);

	if (task_number >= 16) {
		mergesort(begin, middle, compare);
		mergesort(middle, end, compare);
		merge::impl::inplace_merge(begin, middle, end, compare);
		//std::inplace_merge(begin, middle, end, compare);
		return;
	}

	auto task1 = concurrency::create_task([begin, middle, &compare]()
	{
		task_number++;
		mergesort(begin, middle, compare);
	});

	auto task2 = concurrency::create_task([middle, end, &compare]()
	{
		task_number++;
		mergesort(middle, end, compare);
	});

	task1.wait();
	task_number--;
	task2.wait();
	task_number--;

	merge::impl::inplace_merge(begin, middle, end, compare);
	//std::inplace_merge(begin, middle, end, compare);
}

#endif
