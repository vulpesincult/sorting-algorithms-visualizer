#pragma once

#include <stack>
#include <vector>
#include <array>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <random>

#include <fstream>
#include <chrono>




namespace sorts
{
	template<class iterator>
	void my_shuffle(iterator first, iterator last)
	{
		std::random_device rand_dev;
		std::mt19937 gen(rand_dev());
		gen.seed(gen.default_seed);

		typedef typename std::iterator_traits<iterator>::difference_type diff_t;
		typedef typename std::make_unsigned<diff_t>::type udiff_t;
		typedef typename std::uniform_int_distribution<udiff_t> distr_t;
		typedef typename distr_t::param_type param_t;

		distr_t D;
		diff_t n = last - first;
		for (diff_t i = n - 1; i > 0; --i)
		{
			std::iter_swap(first + i, first + (D(gen, param_t(0, i))));
		}
	}


	template<class iterator, class pred>
	void insertion_sort(iterator left, iterator right, pred Pred)
	{
		if (right - left > 1)
		{
			for (iterator _Next = left;
				++_Next != right;
				)
			{
				if (!Pred(*_Next, *(_Next - 1)))
				{
					continue;
				}

				std::_Iter_value_t<iterator> val = std::move(*_Next);
				iterator _Next1 = _Next;

				if (Pred(val, *left))
				{
					std::_Move_backward_unchecked(left, _Next, ++_Next1);
					*left = std::move(val);
				}
				else
				{
					iterator _First1 = _Next1;

					do
					{
						*_Next1 = std::move(*(--_First1));
						_Next1 = _First1;
					} while (Pred(val, *(_First1 - 1)));

					*_Next1 = std::move(val);
				}
			}
		}
	}

	template<class iterator>
	void insertion_sort(iterator left, iterator right)
	{
		insertion_sort(left, right, std::less<>());
	}

	template<class iterator, class pred_t>
	void shell_sort(iterator begin, iterator end, pred_t pred)
	{
		static const int steps[] = {
			1,
			2,
			3,
			5,
			8,
			13,
			21,
			34,
			55,
			89,
			144,
			233,
			377,
			610,
			987,
			1597,
			3194,
			6388,
			12776,
			25552,
			30000,
			55000,
			85000,
			140000,
			214000,
			354000
		};

		auto count = end - begin;

		if (count > 1)
		{
			int i = 25;
			while (steps[i] > count)
			{
				--i;
			}

			for (;
				i >= 0;
				--i)
			{
				auto step = steps[i];
				for (iterator _Next = begin + step - 1;
					(_Next += 1) < end;
					)
				{
					if (!pred(*_Next, *(_Next - step)))
					{
						continue;
					}

					std::_Iter_value_t<iterator> val = std::move(*_Next);
					iterator _Next1 = _Next;

					/*if (pred(val, *begin))
					{
						iterator _First1 = _Next1;

						do
						{
							*_Next1 = std::move(*(_First1 -= step));
							_Next1 = _First1;
						}
						while (_Next1 > begin);

						*_Next1 = std::move(val);
					}
					else
					{*/
					iterator _First1 = _Next1;

					do
					{
						*_Next1 = std::move(*(_First1 -= step));
						_Next1 = _First1;
					} while (((_First1 - step) > begin) && pred(val, *(_First1 - step)));

					*_Next1 = std::move(val);
					//}
				}
			}
		}
	}

	template<class iterator, class predicate>
	void heap_sort(iterator begin, iterator end, int branching_factor, predicate is_less)
	{
		if (begin == end)
		{
			return;
		}

		int len = end - begin;
		int leftovers = (len - 1 - (((len - 2) / branching_factor) * branching_factor)) % branching_factor;
		int heap_size = len - leftovers;
		iterator last_parent = begin + (heap_size - 2) / branching_factor;

		if (heap_size > 1)
		{
			for (iterator i = last_parent;
				i >= begin;
				)
			{
				iterator max_child = begin + (i - begin) * branching_factor + 1;
				iterator last_child = max_child + branching_factor;
				for (iterator child = max_child + 1;
					child < last_child;
					++child)
				{
					if (is_less(*max_child, *child))
					{
						max_child = child;
					}
				}

				if (is_less(*i, *max_child))
				{
					std::_Iter_value_t<iterator> val = std::move(*i);
					iterator j = i;
					*j = std::move(*max_child);
					j = max_child;

					while (j <= last_parent)
					{
						iterator max_child = begin + (j - begin) * branching_factor + 1;
						iterator last_child = max_child + branching_factor;
						for (iterator child = max_child + 1;
							child < last_child;
							)
						{
							if (is_less(*max_child, *child))
							{
								max_child = child;
							}
							++child;
						}

						if (is_less(val, *max_child))
						{
							*j = std::move(*max_child);
							j = max_child;
						}
						else
						{
							break;
						}
					}

					*j = std::move(val);
				}

				if (i != begin)
				{
					--i;
				}
				else
				{
					break;
				}
			}
		}

		insertion_sort(begin + heap_size, end, is_less);

		for (iterator iter = end - 1;
			iter >= begin + heap_size;
			--iter)
		{
			if (!is_less(*iter, *(begin)))
			{
				continue;
			}

			if (heap_size == 1)
			{
				std::iter_swap(begin, iter);
				continue;
			}

			std::_Iter_value_t<iterator> val = std::move(*iter);
			*iter = std::move(*begin);
			iterator max_child = begin + 1;
			iterator last_child = max_child + branching_factor;
			for (iterator child = max_child + 1;
				child < last_child;
				++child)
			{
				if (is_less(*max_child, *child))
				{
					max_child = child;
				}
			}

			if (!is_less(val, *max_child))
			{
				*begin = std::move(val);
				continue;
			}

			iterator j = begin;
			*j = std::move(*max_child);
			j = max_child;

			while (j <= last_parent)
			{
				iterator max_child = begin + (j - begin) * branching_factor + 1;
				iterator last_child = max_child + branching_factor;
				for (iterator child = max_child + 1;
					child < last_child;
					)
				{
					if (is_less(*max_child, *child))
					{
						max_child = child;
					}
					++child;
				}

				if (is_less(val, *max_child))
				{
					*j = std::move(*max_child);
					j = max_child;
				}
				else
				{
					break;
				}
			}

			*j = std::move(val);
		}

		end = begin + heap_size;

		while (end > begin)
		{
			--end;
			std::_Iter_value_t<iterator> val = std::move(*(end));
			*end = std::move(*begin);
			--heap_size;
			last_parent = begin + (heap_size - 2) / branching_factor;
			iterator j = begin;

			if (heap_size > 1)
			{
				while (j <= last_parent)
				{
					iterator max_child = begin + (j - begin) * branching_factor + 1;
					iterator last_child = max_child + branching_factor;
					if (last_child > end)
					{
						last_child = end;
					}

					for (iterator child = max_child + 1;
						child < last_child;
						)
					{
						if (is_less(*max_child, *child))
						{
							max_child = child;
						}
						++child;
					}

					*j = std::move(*max_child);
					j = max_child;
				}

				for (;
					j != begin;
					)
				{
					iterator parent = begin + (j - begin - 1) / branching_factor;
					if (is_less(val, *parent))
					{
						break;
					}
					*j = std::move(*parent);
					j = parent;
				}
			}

			*j = std::move(val);
		}
	}

	template<class iterator, class pred>
	void dual_pivot_qs(iterator left, iterator right, int max_depth, pred Pred)
	{
		int len;
		while ((len = right - left) > 32 && max_depth)
		{
			--max_depth;
			int step = len / 3;
			iterator median1 = left + step, median2 = right - step;

			if (Pred(*median2, *median1))
			{
				std::iter_swap(median1, median2);
			}

			auto pivot1 = *median1, pivot2 = *median2;

			std::iter_swap(left, median1);
			std::iter_swap(median2, right - 1);

			iterator less = left + 1, great = right - 2;

			while (Pred(pivot2, *great))
			{
				--great;
			}
			while (Pred(*less, pivot1))
			{
				++less;
			}

			for (iterator k = less;
				k <= great;
				)
			{
				if (Pred(*k, pivot1))
				{
					std::iter_swap(k, less);
					++less;
				}
				else if (Pred(pivot2, *k))
				{
					auto val = *great;
					*great = *k;
					if (Pred(val, pivot1))
					{
						*k = *less;
						*less = val;
						++less;
					}
					else
					{
						*k = val;
					}
					do { --great; } while (Pred(pivot2, *great));
				}
				++k;
			}

			--less;
			++great;

			std::iter_swap(left, less);
			std::iter_swap(great, right - 1);


			dual_pivot_qs(great + 1, right, max_depth, Pred);
			right = less;

			if (Pred(pivot1, pivot2) || Pred(pivot2, pivot1))
			{
				dual_pivot_qs(less + 1, great, max_depth, Pred);
			}
		}
		if (len <= 32)
		{
			insertion_sort(left, right, Pred);
			return;
		}
		else
		{
			heap_sort(left, right, 2, Pred);
		}
	}

	template<class iterator>
	void dual_pivot_qs(iterator left, iterator right)
	{
		if (right - left < 2)
		{
			return;
		}

		dual_pivot_qs(left, right, log2((right - left) / 32 + 1), std::less<>());
	}

	enum ordering
	{
		sorted,
		reversed,
		random,
	};

	template<class iterator>
	struct sub_sequence
	{
		iterator begin;
		size_t size;
		ordering order;

		sub_sequence() = default;
		sub_sequence(iterator begin, size_t size, ordering order) :
			begin(begin),
			size(size),
			order(order)
		{

		}
	};

	template<class iterator, class predicate>
	void merge_sub_sequences(sub_sequence<iterator> & sequence1, sub_sequence<iterator> & sequence2, predicate is_lesser)
	{
		std::vector<std::_Iter_value_t<iterator>> temp(sequence1.size + sequence2.size);

		if (sequence1.order == sorted)
		{
			if (sequence2.order == sorted)
			{
				std::inplace_merge(sequence1.begin, sequence1.begin + sequence1.size, sequence2.begin + sequence2.size, is_lesser);
			}
			else
			{
				std::reverse_iterator<iterator> sequence2_begin = std::make_reverse_iterator(sequence2.begin + sequence2.size);
				std::reverse_iterator<iterator> sequence2_end = std::make_reverse_iterator(sequence2.begin);
				std::merge(sequence1.begin, sequence1.begin + sequence1.size, sequence2_begin, sequence2_end, temp.begin(), is_lesser);
				std::copy(temp.begin(), temp.end(), sequence1.begin);
			}
		}
		else
		{
			if (sequence2.order == sorted)
			{
				std::reverse_iterator<iterator> sequence1_begin = std::make_reverse_iterator(sequence1.begin + sequence1.size);
				std::reverse_iterator<iterator> sequence1_end = std::make_reverse_iterator(sequence1.begin);
				std::merge(sequence1_begin, sequence1_end, sequence2.begin, sequence2.begin + sequence2.size, temp.begin(), is_lesser);
				std::copy(temp.begin(), temp.end(), sequence1.begin);
				sequence1.order = sorted;
			}
			else
			{
				std::reverse_iterator<iterator> sequence2_begin = std::make_reverse_iterator(sequence2.begin + sequence2.size);
				std::reverse_iterator<iterator> sequence2_end = std::make_reverse_iterator(sequence2.begin);
				std::reverse_iterator<iterator> sequence1_begin = std::make_reverse_iterator(sequence1.begin + sequence1.size);
				std::reverse_iterator<iterator> sequence1_end = std::make_reverse_iterator(sequence1.begin);
				std::inplace_merge(sequence2_begin, sequence2_end, sequence1_end, is_lesser);
			}
		}
	}

	template<class iterator, class predicate>
	void merge_collapse(std::vector<sub_sequence<iterator>> & sequences, predicate is_lesser)
	{
		while (sequences.size() >= 3)
		{
			sub_sequence<iterator> x = sequences.back();
			sequences.pop_back();
			sub_sequence<iterator> y = sequences.back();
			sequences.pop_back();
			sub_sequence<iterator> z = sequences.back();
			sequences.pop_back();

			if (z.size >= x.size + y.size && y.size >= x.size)
			{
				sequences.push_back(z);
				sequences.push_back(y);
				sequences.push_back(x);
				return;
			}
			else if (z.size >= x.size + y.size)
			{
				merge_sub_sequences(y, x, is_lesser);
				sequences.push_back(z);
				sequences.emplace_back(y.begin, x.size + y.size, y.order);
			}
			else
			{
				merge_sub_sequences(z, y, is_lesser);
				sequences.emplace_back(z.begin, z.size + y.size, z.order);
				sequences.push_back(x);
			}
		}
	}

	template<class iterator, class predicate>
	void merge_collapse_finale(std::vector<sub_sequence<iterator>> & sequences, predicate is_lesser)
	{
		while (sequences.size() != 1)
		{
			sub_sequence<iterator> x = sequences.back();
			sequences.pop_back();
			sub_sequence<iterator> y = sequences.back();
			sequences.pop_back();

			if (x.begin < y.begin)
			{
				sub_sequence<iterator> temp = std::move(x);
				x = std::move(y);
				y = std::move(temp);
			}
			merge_sub_sequences(y, x, is_lesser);
			sequences.emplace_back(y.begin, y.size + x.size, sorted);
		}

		if (sequences.back().order == reversed)
		{
			std::reverse(sequences.back().begin, sequences.back().begin + sequences.back().size);
		}
	}

	template<class iterator, class predicate>
	void timsort(iterator begin, iterator end, predicate is_lesser)
	{
		typedef std::_Iter_value_t<iterator> value_t;

		if (end - begin <= 1)
		{
			return;
		}
		std::vector<sub_sequence<iterator>> sequences;

		for (iterator current = begin;
			current != end;
			)
		{
			sequences.emplace_back(current, 0, sorted);
			++current;
			sequences.back().size += 1;

			while (current != end)
			{
				if (is_lesser(*current, *(current - 1)))
				{
					sequences.back().order = reversed;
					break;
				}
				if (is_lesser(*(current - 1), *current))
				{
					break;
				}

				++current;
				sequences.back().size += 1;
			}
			if (current == end)
			{
				break;
			}

			++current;
			sequences.back().size += 1;
			if (current == end)
			{
				break;
			}

			if (sequences.back().order == sorted)
			{
				while (!is_lesser(*current, *(current - 1)))
				{
					++current;
					sequences.back().size += 1;
					if (current == end)
					{
						break;
					}
				}
			}
			else
			{
				while (!is_lesser(*(current - 1), *current))
				{
					++current;
					sequences.back().size += 1;
					if (current == end)
					{
						break;
					}
				}
			}

			merge_collapse(sequences, is_lesser);
		}

		merge_collapse_finale(sequences, is_lesser);
	}
}