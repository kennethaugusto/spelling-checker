Vallika Nayak (vn188)
Kenneth Augusto (kma234)

How to run:
Terminal
-> make
-> ./spchk dict.txt /.../path/data

For our spelling checker, we chose to use an array to store the words in a library and did binary search on them.
Originally, we implemented a HashMap, but it would have to rehash too many times and would be inefficient so we opted for the array.
We have 3 files handling functions before spchk for different reasons:
1) file_handler.c -> manages file operations; opening files, reading lines from files, closing files and has a cursor for tracking read positions
2) dict_handler.c -> handles the dictionary operations like reading a dictionary from a file, inserting words into dictionary, checking if words exist in dictionary
3) processFile.c -> processes text files and organizes them
4) spchk.c -> the main driver of this program. recursively checks files and directories for words that do not exist in the dictionary

We have many error messages set up to show where the program goes wrong, if any:
1) file_handler: 2 possible errors
- Unable to allocate file cursor
- Could not open file
2) dict_handler: 6 possible errors
- Table cannot be resized
- Word not allocated
- Dictionary cannot be allocated
- Dictionary table cannot be allocated
- Word cannot be inserted into dictionary
- Could not read file
3) processFile: 
- Allocation failed
- Unable to allocate parsed entry
4) spchk:
- Could not parse file
- Could not open directory
- Could not read dictionary

For the dictionary file, I generated dict.txt and we have 1.txt inside data folder. These both contains words from the First Amendment and cross-references them with each other.
dict.txt -> has all words with no misspelled words
1.txt -> intentionally misspelled words

Results:

60/100

Readme --15/20 (missing descriptions for test cases) Compilation --10/10 Spelling checking - Correct words 0/5 (Give error output: Error: Could not open directory: Not a directory) - Correct words with punctuation and capitalization 0/5 (Give error output: Error: Could not open directory: Not a directory) - Incorrect words 0/10 (Give error output: Error: Could not open directory: Not a directory) - Incorrect capitalization 0/10 (Give error output: Error: Could not open directory: Not a directory) - Hyphenated words 0/5 (Give error output: Error: Could not open directory: Not a directory) - Tricky punctuation 0/5 (Give error output: Error: Could not open directory: Not a directory) --0/40 Directory traversal - Program can check files ending in .txt in a single directory 15/15 - Program can check files ending in .txt recursively 10/10 - Program does not check files or directories starting with . or files not ending with .txt 5/5 -- 30/30