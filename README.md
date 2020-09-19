# *Sorted stream merge*

## Algorithm description

#### Task:
Merge ascending sorted streams.

#### Solution:
Algorithm uses **binary heap** to merge streams.
The heap contains exactly the same number of elements as the quantity of 
available to read files.
Algorithm looks over all input files and read one element from each of them or
one element from file which has the previous (last) minimum.
The algorithm then extracts the minimum from the heap and puts it 
to the output stream.
This action continues until all data from files has been read.
If heap still contain items then algorithm extract minimum from heap 
until it is empty.

***

## Features

* Merges integer or string streams;
* Skips all incorrect data *(such as data of a different type or unsorted data)*
and reports position of that data;
* Skips empty files and reports which files are empty;
* Reports if files doesn't exist.

***

## Usage

Require minimum 3 parameters:
1. **'-s'** or **'-i'** to declare data type: string or integers;
2. Path to output file;
3. Path to first *(or only)* input file.

Next parameters are optional:
paths for other input files that need to be merged together.

***

## Example

input1.txt | input2.txt | input3.txt
---------- | ---------- | ----------
3          | 1          | 2
4          | 6          | 6
2          | 7          | 9
s          | p          |
9          | 10         |
0          | 13         |
           | s          |
  
output.txt |
---------- |
1          |
2          |
3          |
4          |
6          |
6          |
7          |
9          |
9          |
10         |
13         |
