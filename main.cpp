#include <iostream>
#include <filesystem>
#include <string>
#include <cstring>
#include <bits/stdc++.h>
#include "binary_heap.h"

namespace fs = std::filesystem;

struct input_file_data {
    fs::path path;
    std::ifstream *stream;

    input_file_data(fs::path &path, std::ifstream *stream) : path(path), stream(stream) {}
};

std::vector<input_file_data> get_input_files(const std::vector<char *> &input_paths) {
    /* check and open all correct input files
     * save file names for each stream (make vector of pairs of file names and streams) */

    std::vector<input_file_data> input_files;

    // skip invalid files
    for (auto const &path : input_paths) {
        fs::path in(path);

        if (!fs::exists(in)) {
            std::cerr << "Input file " << in << " doesn't exist!\n";
            continue;
        }
        if (fs::is_empty(in)) {
            std::cerr << "Input file " << in << " is empty!\n";
            continue;
        }
        try {
            input_files.emplace_back(input_file_data(in, new std::ifstream(path)));
        }
        catch (std::ios_base::failure &e) {
            std::cerr << "Error opening file " << in
                      << "\nExplanation: " << e.what() << "\n";
            continue;
        }
    }

    return input_files;
}

template<typename T>
void push_item_to_heap(
        input_file_data &file,
        BinaryHeap<T> &heap,
        T last_item,
        int last_file_index,
        int current_file_index
) {
    /* read data from file which element was the previous (last) minimum
     * or read data from all files if file with the previous (last) minimum isn't available */

    if (last_file_index >= 0 && last_file_index != current_file_index) return;

    T current_item;
    *file.stream >> current_item;

    // check if the input is invalid and skip line with invalid data
    while ((file.stream->fail() || current_item < last_item) && !file.stream->eof()) {
        file.stream->clear();
        std::cerr << "Invalid data in file " << file.path
                  << "\nPosition " << file.stream->tellg() << "\n\n";

        file.stream->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        *file.stream >> current_item;
    }

    // add item to heap only if it's correct (it helps if the last data from this file is invalid)
    // heap element contains template item and integer identifier
    if (!file.stream->fail() && current_item >= last_item)
        heap.add(heap_data<T>(current_file_index, current_item));
}

template<typename T>
int sort(
        const char *output_path,
        const std::vector<char *> &input_paths,
        BinaryHeap<T> &heap,
        T template_min
) {
    // check and open output file
    fs::path out(output_path);
    if (!fs::exists(out))
        std::cout << "Creating file " << out << "\n";

    std::ofstream output;
    try {
        output.open(output_path, std::ios::out | std::ios::trunc);
    }
    catch (std::ios_base::failure &e) {
        std::cerr << "Error opening file " << out
                  << "\nExplanation: " << e.what() << "\n";
        return 1;
    }

    auto input_files = get_input_files(input_paths);

    if (input_files.empty()) {
        std::cerr << "All input files are empty or have an error with opening!\n";
        return 1;
    }

    // previous (last) minimum element
    T last_item = template_min;

    // the index of the file from which the element was the previous (last) minimum
    int last_file_index = -1;

    bool loop_continue = true;

    while (loop_continue) {
        for (int i = 0; i < input_files.size(); i++) {
            // skip closed or finished files
            if (!input_files[i].stream->is_open()) {
                continue;
            }

            if (input_files[i].stream->eof()) {
                input_files[i].stream->close();
                if (last_file_index == i)
                    last_file_index = -1;
                continue;
            }

            push_item_to_heap(input_files[i], heap, last_item, last_file_index, i);
        }

        if (!heap.get_heap_size()) break;

        // write minimum to output
        auto min = heap.extract_min();
        last_file_index = min.index;
        last_item = min.item;
        output << last_item << '\n';

        // continue loop if there is a file which haven't been read
        loop_continue = [&input_files] {
            return std::ranges::any_of(input_files, [](auto const &file) { return !file.stream->eof(); });
        }();
    }

    // get all remaining elements
    while (heap.get_heap_size())
        output << heap.extract_min().item << '\n';

    output.close();
    std::cout << "Finished!\n";

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Not enough parameters. Usage: sort.exe [-s | -i] [output.txt] [input1.txt ...]";
        return 1;
    }

    // vector of input files paths
    std::vector<char *> input_paths(argv + 3, argv + argc);

    if (!strcmp(argv[1], "-i")) {
        BinaryHeap<int> heap;
        return sort(argv[2], input_paths, heap, INT_MIN);
    } else if (!strcmp(argv[1], "-s")) {
        BinaryHeap<std::string> heap;
        return sort(argv[2], input_paths, heap, std::string(""));
    } else {
        std::cerr << R"(First parameter must be '-s' for files with strings or '-i' for files with integers)";
        return 1;
    }
}
