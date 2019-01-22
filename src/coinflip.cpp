/* Random-Tools Coinflip
 *
 * copyright 2019 Alec Hitchiner
 *
 * Simulates coin flips.
 *
 * Simulates a given number of coin flips. The number of flips can be specified,
 * and the sequence of flips can be printed. The total number of heads and tails
 * can also be printed at the end.
 *
 * Usage: coinflip -hnr {# of flips}
 * Command-line options:
 * 	-h: Print a usage message and exit, this supercedes all other options
 * 	-n: Supresses printing of coin flips, default is to print con flips using H
 * 		and T for heads and tails.
 * 	-r: Prints a final report of heads and tails at the end of the run
 * 	After these options, the number of flips to perform may be given, default 1.
 */

#include <cerrno>
#include <climits>
#include <cstdlib>

#include <unistd.h>

#include <iostream>
#include <random>

void print_usage_message(char *name){
	std::cerr << "Usage: "
		<< name
		<< " {-hnr} {# of flips}\n"
		<< "\nOptions:\n"
		<< "\t-h: Print usage message and exit\n"
		<< "\t-n: Suppress printing of individual flips\n"
		<< "\t-r: Print count of total flips at the end\n";
}

int main(int argc, char **argv){
	/* Configuration parameters */
	int print_flips = 1;
	int print_results = 0;
	long flips = 1;

	/* Read args */
	int read_flag;
	opterr = 0;
	while((read_flag = getopt(argc, argv, "hnr")) != -1){
		switch(read_flag){
			case 'n':
				print_flips = 0;
				break;
			case 'r':
				print_results = 1;
				break;
			case 'h':
				print_usage_message(argv[0]);
				return 0;
				break;
			case '?':
			default:
				print_usage_message(argv[0]);
				return -1;
				break;
		}
	}

	/* Check for junk args */
	if(optind < argc - 1){
			print_usage_message(argv[0]);
			return -1;
	}

	/* Find number of flips */
	if(optind == argc - 1){
		char *check;

		flips = strtol(argv[optind], &check, 10);
		if(*check != '\0'){
			print_usage_message(argv[0]);
			return -1;
		}
		if(flips < 0 || (flips == LONG_MAX && errno)){
			std::cerr << "Number of flips out of range 0 to "
				<< LONG_MAX << std::endl;
			return -1;
		}
	}

	/* Perform flips */
	long heads = 0;
	long tails = 0;
	std::random_device rng;
	std::uniform_int_distribution<int> flipper(0, 1);
	for(long i = 0; i < flips; i++){
		if(flipper(rng)){
			heads++;
			if(print_flips){
				std::cout << "H";
			}
		} else{
			tails++;
			if(print_flips){
				std::cout << "T";
			}
		}
	}
	if(print_flips){
		std::cout << std::endl;
	}

	/* Print final results */
	if(print_results){
		std::cout << "Heads: " << heads << "\nTails: " << tails << std::endl;
	}

	return 0;
}
