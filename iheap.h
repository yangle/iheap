/*
 * iheap.h - A single-header C++ library for indexed binary heaps
 * <https://github.com/yangle/iheap>
 *
 * Copyright (c) 2017 Yang-Le Wu
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */ 

#pragma once

#include <iterator>
#include <utility>

namespace iheap { namespace detail
{
    template<typename RandomIt, typename Indexer, typename Compare>
    void bubble_up(RandomIt begin, RandomIt end, RandomIt it, Indexer indexer,
                   Compare comp = Compare())
    {
        using std::swap;

        if ((it < begin) || (it >= end))
            return;

        int i = it - begin;

        // zero-based indexing:
        // parent of [i] is [(i - 1) / 2]
        while ((i > 0) && comp(begin[(i - 1) / 2], begin[i]))
        {
            swap(begin[(i - 1) / 2], begin[i]);
            swap(indexer(begin[(i - 1) / 2].second), indexer(begin[i].second));
            i = (i - 1) / 2;
        }
    }

    template<typename RandomIt, typename Indexer, typename Compare>
    void bubble_down(RandomIt begin, RandomIt end, RandomIt it, Indexer indexer,
                     Compare comp = Compare())
    {
        using std::swap;

        if ((it < begin) || (it >= end))
            return;

        int i = it - begin;
        int n = end - begin;

        // zero-based indexing:
        // left and right children of [i] are [2 * i + 1] and [2 * i + 2]
        while (i < n)
        {
            int m = i; // the most dominant among [i] and its two children
            for (int c = 2 * i + 1; (c < n) && (c <= 2 * i + 2); ++c)
                if (comp(begin[m], begin[c]))
                    m = c;
            if (m == i)
                return;

            swap(begin[i], begin[m]);
            swap(indexer(begin[i].second), indexer(begin[m].second));
            i = m;
        }
    }
}} // namespace iheap::detail

namespace iheap
{
    template<typename RandomIt, typename Indexer,
        typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
    void push(RandomIt begin, RandomIt end, Indexer indexer, Compare comp = Compare())
    {
        if (begin >= end)
            return;
        indexer((end - 1)->second) = (end - begin) - 1;
        detail::bubble_up(begin, end, end - 1, indexer, comp);
    }

    template<typename RandomIt, typename Indexer,
        typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
    void pop(RandomIt begin, RandomIt end, Indexer indexer, Compare comp = Compare())
    {
        using std::swap;

        if (begin >= end)
            return;

        auto back = end - 1;
        swap(*begin, *back);
        swap(indexer(begin->second), indexer(back->second));
        indexer(back->second) = -1;

        detail::bubble_down(begin, back, begin, indexer, comp);
        // works just fine when begin == back
    }

    template<typename RandomIt, typename Indexer,
        typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
    void make(RandomIt begin, RandomIt end, Indexer indexer, Compare comp = Compare())
    {
        if (begin >= end) // ".end() - 1" goes crazy when the container is empty
            return;

        for (auto it = end - 1; it >= begin; --it)
            detail::bubble_down(begin, end, it, indexer, comp);
    }

    template<typename RandomIt, typename Indexer,
        typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
    void sort(RandomIt begin, RandomIt end, Indexer indexer, Compare comp = Compare())
    {
        while (begin < end)
            pop(begin, end--, indexer, comp);
    }

    template<typename RandomIt, typename Indexer,
        typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
    bool update(RandomIt begin, RandomIt end, Indexer indexer,
                typename std::iterator_traits<RandomIt>::value_type::second_type key,
                typename std::iterator_traits<RandomIt>::value_type::first_type value,
                Compare comp = Compare())
    {
        if (begin >= end)
            return false;

        int index = indexer(key);
        if (index == -1)
            return false;

        auto it = begin + index;
        auto old = *it;
        it->first = value;

        if (comp(*it, old))
            detail::bubble_down(begin, end, it, indexer, comp);
        else if (comp(old, *it))
            detail::bubble_up(begin, end, it, indexer, comp);

        return true;
    }

    template<typename RandomIt, typename Indexer,
        typename Compare = std::less<typename std::iterator_traits<RandomIt>::value_type>>
    bool pop_key(RandomIt begin, RandomIt end, Indexer indexer,
                 typename std::iterator_traits<RandomIt>::value_type::second_type key,
                 Compare comp = Compare())
    {
        using std::swap;

        if (begin >= end)
            return false;

        int index = indexer(key);
        if (index == -1)
            return false;

        auto it = begin + index;
        auto back = end - 1;

        auto old = *it;

        swap(*it, *back);
        swap(indexer(it->second), indexer(back->second));
        indexer(back->second) = -1;
        // works just fine when it == back

        if (comp(*it, old))
            detail::bubble_down(begin, back, it, indexer, comp);
        else if (comp(old, *it))
            detail::bubble_up(begin, back, it, indexer, comp);

        return true;
    }

} // namespace iheap
