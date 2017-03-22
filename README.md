## iheap -- a single-header C++ template for indexed binary heaps

An indexed binary heap supports both `O(1)` lookup of the in-heap index and
`O(log n)` update of the element value, for *any* element in the heap.
This is achieved by augmenting each heap operation with the corresponding
updates to an external index table.

This data structure often proves useful when implementing algorithms with a
greedy flavor.
For example, each iteration in [Dijkstra's algorithm][dijk] involves  
(a) finding the node `x` with the *shortest* "tentative distance", and  
(b) *updating* the tentative distances of `x`'s neighbors.  
A normal binary heap suffices for (a), but not quite for (b), as the latter
calls for both looking up the in-heap index of each neighbor node and modifying
elements deep inside the heap with destroying the heap order.
These two problems can be solved simultaneously using an indexed heap.

[dijk]: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm


### Usage

The `iheap` interface resembles that of `std::make/push/pop_heap`, taking a
pair of `RandomIt`s into a container. There are a few differences:

1. For a heap with element values in `T`, the `RandomIt` iterator should have
   `value_type` being `std::pair<T, K>` (instead of just `T`).
   Here, `K` is the type of the *key* of each element, generalizing an integer
   index in the simplest case.

   In the context of Dijkstra's algorithm over a 2D lattice, `T = int` is the
   tentative distance to the initial node, while `K = std::pair<int, int>`
   gives the node coordinates.

2. Each heap function takes as an additional argument a functor `indexer`.
   The expected signature is
   ```cpp
   int& indexer(K key);
   ```
   For each key, the indexer should return a *mutable* reference to the
   corresponding entry in the in-heap index table allocated elsewhere.

   For the Dijkstra's algorithm example above, `indexer({x, y})` should return
   a reference to the entry in the heap index table for the node at `{x, y}`,
   meaning that the node `{x, y}` currently resides in `heap[indexer({x, y})]`.

   More generally, the following invariant is maintained for all in-heap
   elements `{value, key}` by all operations:
   ```cpp
   heap[indexer(key)] == {value, key}
   ```
   Popping an element from the heap sets its `indexer(key)` to `-1`.

3. The comparator functor should compare two instances of `std::pair<T, K>`
   rather than just two `T`s. The expected signature is
   ```cpp
   bool comp(const std::pair<T, K>& a, const std::pair<T, K>& b);
   ```
   instead of just `bool comp(const T& a, const T& b);` as is the case for
   `std::` heaps. The more general comparator over `std::pair<T, K>` makes it
   possible to explicitly resolve equality over `T` by further comparing over
   `K`.

   Similar to the `std::` case, the comparator here defaults to
   `std::less<std::pair<T, K>>()`, corresponding to a max-indexed-heap.

4. The `O(1)` heap index lookup makes it straightforward to update the value of
   any heap element (as specified by a key) on the fly and "reheapify"
   accordingly, with a total time complexity of just `O(log n)`. This
   functionality is provided by `update`. This function returns `true` on a
   successful update, and `false` when the input key is not in the heap.
   Similarly, we also provide a function `pop_key` that pops a specific key
   from the heap, returning `true` if successful.
