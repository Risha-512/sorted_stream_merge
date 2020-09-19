#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include "binary_heap.h"

namespace fs = std::filesystem;

template <typename T>
int sort(const char* output_path, const std::vector<char*> &input_paths, BinaryHeap<T> &heap, T template_min)
{
    // check and open output file
    fs::path out(output_path);
    if (!fs::exists(out))
        std::cout << "Creating file " << out << "\n";

    std::ofstream output;
    try
    {
        output.open(output_path, std::ios::out | std::ios::trunc);
    }
    catch (std::ios_base::failure &e)
    {
        std::cerr << "Error opening file " << out
                  << "\nError code: " << e.code() << "\n";
        return 1;
    }

    // check and open all correct input files
    // save file names for each stream (make vector of pairs of file names and streams)
    std::vector< std::pair<fs::path, std::unique_ptr<std::ifstream> >> input_files;
    for (auto const &path : input_paths)
    {
        // not push invalid files in vector
        fs::path in(path);
        if (!fs::exists(in))
        {
            std::cerr << "Input file " << in << " doesn't exist!\n";
            continue;
        }
        if (fs::is_empty(in))
        {
            std::cerr << "Input file " << in << " is empty!\n";
            continue;
        }
        try
        {
            std::unique_ptr<std::ifstream> input(new std::ifstream(path));
            input_files.emplace_back(std::make_pair(in, std::move(input)));
        }
        catch (std::ios_base::failure &e)
        {
            std::cerr << "Error opening file " << in
                      << "\nError code: " << e.code() << "\n";
            continue;
        }
    }

    if (input_files.empty())
    {
        std::cerr << "All input files are empty or have an error with opening!\n";
        return 1;
    }

    bool loop_continue = true;
    T buf;
    // previous (last) minimum element
    T last_item = template_min;
    // the index of the file from which the element was the previous (last) minimum
    int last_index = -1;

    while (loop_continue)
    {
        for (int i = 0; i < input_files.size(); i++)
        {
            // skip closed or finished files
            if (!input_files[i].second->is_open()) continue;
            if (input_files[i].second->eof())
            {
                input_files[i].second->close();
                if (last_index == i)
                    last_index = -1;
                continue;
            }

            // read data from file which element was the previous (last) minimum
            // or read data from all files if file with the previous (last) minimum isn't available
            if (last_index < 0 || last_index == i)
            {
                *input_files[i].second >> buf;

                // check if the input is invalid and skip line with invalid data
                while ((input_files[i].second->fail() || buf < last_item) && !input_files[i].second->eof())
                {
                    input_files[i].second->clear();
                    std::cerr << "Invalid data in file " << input_files[i].first
                              << "\nPosition " << input_files[i].second->tellg() << "\n\n";

                    input_files[i].second->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    *input_files[i].second >> buf;
                }

                // add item to heap only if it's correct (it helps if the last data from this file is invalid)
                // heap element contains template item and integer identifier
                if (!input_files[i].second->fail() && buf >= last_item) heap.add(std::make_pair(i, buf));
            }
        }

        if (!heap.get_heap_size()) break;

        // write minimum to output
        auto min = heap.extract_min();
        last_index = min.first;
        last_item = min.second;
        output << last_item << '\n';

        // continue loop if there is a file which haven't been read
        loop_continue = [&input_files]
        {
            for (auto const &item : input_files)
                if (!item.second->eof())
                    return true;
            return false;
        }();
    }

    // get all remaining elements
    while(heap.get_heap_size())
        output << heap.extract_min().second << '\n';

    output.close();
    std::cout << "Finished!\n";

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Not enough parameters. Usage: sort.exe [-s | -i] [output.txt] [input1.txt ...]";
        return 1;
    }

    // vector of input files paths
    std::vector<char*> input_paths(argv + 3, argv + argc );

    if (!strcmp(argv[1], "-i"))
    {
        BinaryHeap<int> heap;
        return sort(argv[2], input_paths, heap, INT_MIN);
    }
    else if (!strcmp(argv[1], "-s"))
    {
        BinaryHeap<std::string> heap;
        return sort(argv[2], input_paths, heap, std::string(""));
    }
    else
    {
        std::cerr << R"(First parameter must be '-s' for files with strings or '-i' for files with integers)";
        return 1;
    }
}
