#ifndef SORT_IT_BINARYHEAP_H
#define SORT_IT_BINARYHEAP_H

#include <vector>
#include <utility>

template<class T>
struct heap_data {
    int index;
    T item;

    heap_data(int index, T item) : index(index), item(item) {}
};

template<class T>
class BinaryHeap {
private:
    // pairs of template items and integer identifiers
    std::vector<heap_data<T>> list;

public:
    [[nodiscard]] inline int get_heap_size() const { return list.size(); }

    [[nodiscard]] inline heap_data<T> get_min() const { return list.front(); }

    BinaryHeap() = default;

    void add(heap_data<T> item) {
        list.emplace_back(item);

        int i = get_heap_size() - 1,
                parent = (i - 1) / 2;

        while (i > 0 && list[parent].item > list[i].item) {
            std::swap(list[i], list[parent]);
            i = parent;
            parent = (i - 1) / 2;
        }
    }

    void heapify(int index) {
        int left_child = 2 * index + 1;
        int right_child = 2 * index + 2;
        int smallest_child = index;

        if (left_child < get_heap_size() && list[left_child].item < list[smallest_child].item)
            smallest_child = left_child;

        if (right_child < get_heap_size() && list[right_child].item < list[smallest_child].item)
            smallest_child = right_child;

        if (smallest_child != index) {
            std::swap(list[index], list[smallest_child]);
            index = smallest_child;
            heapify(index);
        }
    }

    heap_data<T> extract_min() {
        auto result = get_min();

        list.front() = list.back();
        list.pop_back();

        heapify(0);
        return result;
    }
};

#endif //SORT_IT_BINARYHEAP_H
