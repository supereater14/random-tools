/* Random-Tools Shuffle
 *
 * copyright 2019 Alec Hitchiner
 *
 * Shuffles a list of items.
 *
 * This program is part of the random-tools package. It takes a list of items,
 * and prints them in a random order. The items can either be supplied as
 * command-line arguments, or as a file. If supplied as a file, each line is
 * treated as an item.
 * 
 * Usage: shuffle {-hf} {items...}
 * Command-line options:
 * 	-h: Prints a usage message and exits, this supercedes all other options
 * 	-p: Uses a pseudorandom generator, rather than the default hardware RNG
 * 	-f file: Specifies a list file to use. When this option is given, items
 * 		specified as arguments are ignored.
 * 	A list of items may be supplied after these options.
 */
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

void print_usage_message(char *name){
	std::cerr << "Usage: " << name << " {-hf} {item1 item2 ...}\n\n"
		<< "\t-h: Print usage message and exit\n"
		<< "\t-p: Use a pseudorandom generator, rather than the hardware RNG\n"
		<< "\t-f: File to read list from (if given, item list is ignored)\n";
}

int main(int argc, char **argv){
	/* Configuration */
	int use_pseudo = 0;
	int read_file = 0;
	char *list_filename = nullptr;

	/* Read arguments */
	char read_arg;
	while((read_arg = getopt(argc, argv, "hpf:")) != -1){
		switch(read_arg){
			case 'h':
				print_usage_message(argv[0]);
				return 0;
			case 'p':
				use_pseudo = 1;
				break;
			case 'f':
				read_file = 1;
				list_filename = optarg;
				break;
			case '?':
			default:
				print_usage_message(argv[0]);
				return -1;
				break;
		}
	}

	/* Read list */
	std::vector<std::string> shuffle_list;
	if(read_file){
		/* Open input file */
		std::ifstream list_file(list_filename);
		if(list_file.fail()){
			std::cerr << "Failed to open input file " << list_filename
				<< std::endl;
			return -1;
		}

		/* Read lines */
		std::string read_line;
		std::getline(list_file, read_line);
		while(list_file.good()){
			shuffle_list.push_back(read_line);
			std::getline(list_file, read_line);
		}
		if(list_file.bad()){
			std::cerr << "Failed to read input file\n" << std::endl;
			return -1;
		}


		/* Clean up */
		list_file.close();
	} else{
		/* Read args */
		for(int i = optind; i < argc; i++){
			shuffle_list.emplace_back(argv[i]);
		}
	}

	/* Shuffle elements */
	/* This is basically a Knuth shuffle, just backwards because iterating down
	 * to zero with size_type isn't really practical
	 */
	std::random_device rng;
	std::mt19937_64 prng(rng());
	for(std::vector<std::string>::size_type i = 0;
			i < shuffle_list.size();
			i++){
		std::uniform_int_distribution<std::vector<std::string>::size_type>
			selector(i, shuffle_list.size() - 1);
		if(use_pseudo){
			std::iter_swap(shuffle_list.begin() + i,
				shuffle_list.begin() + selector(prng));
		} else{
			std::iter_swap(shuffle_list.begin() + i,
				shuffle_list.begin() + selector(rng));
		}
	}

	/* Print shuffled list */
	for(std::vector<std::string>::iterator i = shuffle_list.begin();
			i != shuffle_list.end();
			i++){
		std::cout << *i << std::endl;
	}

	return 0;
}
