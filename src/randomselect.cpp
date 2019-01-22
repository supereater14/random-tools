/* Random-Tools Randomselect
 *
 * copyright 2019 Alec Hitchiner
 *
 * Selects random items from a list.
 *
 * This program selects random items from a supplied list. The list can be
 * specified as command-line arguments, or read from a file. The number of items
 * to select can also be specified, and duplicates can be allowed if chosen.
 *
 * Usage: randomselect {-hd} {-n count} {-f file} {items...}
 * Command-line options:
 * 	-h: Print a usage message and exit, this supercedes all other options
 * 	-d: Allow duplicate items (select-with-replacement), default is to not allow
 * 		duplicate items (select-without-replacement).
 * 	-p: Use pseudorandom generator, rather than the default hardware RNG
 * 	-n count: Specifies the number of items to select, default 1.
 * 	-f file: Specifies a file to read the list of items from. If this option is
 * 		given, items given as arguments are ignored.
 * 	These options may be followed by a list of items.
 */
#include <cerrno>
#include <climits>
#include <cstdlib>

#include <unistd.h>

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

void print_usage_message(char *name){
	std::cerr << "Usage: " << name
		<< " {-hdp} {-n count} {-f file} {items...}\n\n"
		<< "\t-h: Print usage message and exit\n"
		<< "\t-d: Allow duplicate selections\n"
		<< "\t-p: Use pseudorandom generator, rather than hardware RNG\n"
		<< "\t-n count: Number of items to select (default is 1)\n"
		<< "\t-f file: File to read list from (if given, item list is ignored)\n";
}

int main(int argc, char **argv){
	/* Configuration */
	int allow_duplicates = 0;
	int use_pseudo = 0;
	long num_select = 1;
	int read_file = 0;
	char *list_filename = nullptr;

	/* Read arguments */
	char read_arg;
	char *check_c;
	while((read_arg = getopt(argc, argv, "hdpn:f:")) != -1){
		switch(read_arg){
			case 'h':
				print_usage_message(argv[0]);
				return 0;
			case 'd':
				allow_duplicates = 1;
				break;
			case 'p':
				use_pseudo = 1;
				break;
			case 'n':
				num_select = strtol(optarg, &check_c, 10);
				if(*check_c != '\0'){
					std::cerr << "-n requires a numerical argument\n";
					return -1;
				}
				if(num_select < 0 || (num_select == LONG_MAX && errno)){
					std::cerr << "-n value out of range 0 to " << LONG_MAX
						<< std::endl;
					return -1;
				}
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
	std::vector<std::string> selection_list;
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
			selection_list.push_back(read_line);
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
			selection_list.emplace_back(argv[i]);
		}
	}

	/* Check selection list is sane */
	if(selection_list.size() == 0){
		std::cerr << "Selection list is empty\n";
		return -1;
	}
	if(!allow_duplicates
			&& selection_list.size() <= LONG_MAX
			&& num_select > (long) selection_list.size()){
		std::cerr << "-n value greater than number of elements\n";
		return -1;
	}

	/* Make selections */
	std::random_device rng;
	std::mt19937_64 prng(rng());
	for(long i = 0; i < num_select; i++){
		/* Select an element */
		std::uniform_int_distribution<std::vector<std::string>::size_type>
			dist(0, selection_list.size() - 1);
		std::vector<std::string>::size_type selection_index;
	   if(use_pseudo){
		   selection_index = dist(prng);
	   } else{
		   selection_index = dist(rng);
	   }

		/* Print selected element */
		std::cout << selection_list[selection_index] << std::endl;

		/* Remove selected element from list */
		if(!allow_duplicates){
			selection_list.erase(selection_list.begin() + selection_index);
		}
	}

	return 0;
}
