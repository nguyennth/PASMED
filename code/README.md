Requirements:
- g++
- Boost library (https://www.boost.org): 
    + please install the library and make sure it in /usr/local/include and /usr/local/lib
    + remember to run 'sudo ldconfig' after installing the library

Running:
- Compile the code by running Makefile
- Run the following command to extract relations between entities
./extractRel text_file entity_file enju_file dir_out

*text_file: path to a text file.

*entity_file: path to a file contains named entities corresponding to the text file in brat format

*enju_file: path to a .gz file contains PAS results corresponding to the text file by Enju parser. Please note that this is the output of enju in standoff format (-so option in Enju).

*dir_out: a folder to save the extracted relations, the output file will have the same names as the input entity file

Example:
"./extractRel ../example/134379_English_94205_42468314_1988.txt ../example/134379_English_94205_42468314_1988.ann ../example/134379_English_94205_42468314_1988.txt.gz ../example/relation"

Please be noted that this code was modified to extract occurrences from biodiversity documents, which were reported in this paper:

Nguyen, N. T. H., Gabud, R. and Ananiadou, S.. (2019). COPIOUS: A gold standard corpus of named entities towards extracting species occurrence from biodiversity literature. In: Biodiversity Data Journal, 7, e29626

https://doi.org/10.3897/BDJ.7.e29626
