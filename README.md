# Indexer
The Indexer is a program that allows you to analyze multiple files and create an index of all the words found within them. It builds a tree data structure to efficiently store the words along with their total and per-file frequency, as well as the sentences in which they appeared. Once the index is created, you can query the program to retrieve information about a specific word.  
## Usage
Compilation: To compile the program, simply use the make command.  

Execution: The program should be run with the following command:  

./main <path_to_file1> [<path_to_file2> [...]]  
Replace <path_to_file1>, <path_to_file2>, and so on, with the paths to the files you want to analyze. You can specify multiple files, separated by spaces.

## Querying the Program
After running the program, it will prompt you with: "Word: ". You should enter the word for which you want to retrieve information.  

Once you enter a word, the program will prompt: "Print how many occurrences from each file: ". You can choose how many occurrences of the word in each individual file to print.  
