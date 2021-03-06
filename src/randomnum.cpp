/* Random-Tools Randomnum
 *
 * copyright 2019 Alec Hitchiner
 *
 * Generates random numbers.
 *
 * This program generates a given number of random numbers. It can generate both
 * integer and real numbers.
 *
 * Usage: randomnum {-hrp} {-n count} [Lower bound] [Upper bound]
 * Command-line options:
 * 	-h: Print usage message and exit, this supercedes all other options
 * 	-r: Generate real numbers, default is integers
 * 	-p: Uses a pseudorandom generator, instead of the default hardware RNG
 * 	-n count: Specifies number of numbers to generate, default 1.
 * 		This must be positive. When multiple numbers are generated, each will be
 * 		on a new line.
 * 	Lower bound: Smallest number that can be generated, inclusive
 * 	Upper bound: Largest number that can be generated, inclusive
 */

#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdlib>

#include <unistd.h>

#include <iostream>
#include <random>

void print_usage_message(char *name){
	std::cerr << "Usage: "
		<< name
		<< " {-hr} {-n count} [Lower bound] [Upper bound]\n"
		<< "\nOptions:\n"
		<< "\t-h: Print usage message and exit\n"
		<< "\t-r: Choose real numbers, rather than integers\n"
		<< "\t-p: Use a pseudorandom generator, rather than the hardware RNG\n"
		<< "\t-n count: Number of random numbers to generate\n";
}

int main(int argc, char **argv){
	/* Configuration parameters */
	long count = 1;
	int real = 0;
	int use_pseudo = 0;

	/* Read args */
	int read_flag;
	char *check_c;
	opterr = 0;
	while((read_flag = getopt(argc, argv, "hrpn:")) != -1){
		switch(read_flag){
			case 'h':
				print_usage_message(argv[0]);
				return 0;
				break;
			case 'n':
				count = strtol(optarg, &check_c, 10);
				if(*check_c != '\0'){
					print_usage_message(argv[0]);
					return -1;
				}
				if(count < 0 || (count == LONG_MAX && errno)){
					std::cerr << "-n out of range 0 to " << LONG_MAX
						<< std::endl;
					return -1;
				}
				break;
			case 'r':
				real = 1;
				break;
			case 'p':
				use_pseudo = 1;
				break;
			case '?':
			default:
				print_usage_message(argv[0]);
				return -1;
				break;
		}
	}

	/* Check for junk args */
	if(optind != argc - 2){
			print_usage_message(argv[0]);
			return -1;
	}

	/* Find bounds */
	union {
		long l;
		double f;
	} lower_bound = {0}, upper_bound = {0};
	if(real){
		lower_bound.f = strtod(argv[optind], &check_c);
		if(*check_c != '\0'){
			print_usage_message(argv[0]);
			return -1;
		}
		if(lower_bound.f == HUGE_VAL && errno){
			std::cerr << "Lower bound out of range\n";
			return -1;
		}
		if(lower_bound.f == NAN){
			std::cerr << "Cannot use NAN as a bound\n";
			return -1;
		}

		upper_bound.f = strtod(argv[optind + 1], &check_c);
		if(*check_c != '\0'){
			print_usage_message(argv[0]);
			return -1;
		}
		if(upper_bound.f == HUGE_VAL && errno){
			std::cerr << "Upper bound out of range\n";
			return -1;
		}
		if(upper_bound.f == NAN){
			std::cerr << "Cannot use NAN as a bound\n";
			return -1;
		}
	} else{
		lower_bound.l = strtol(argv[optind], &check_c, 10);
		if(*check_c != '\0'){
			print_usage_message(argv[0]);
			return -1;
		}
		if((lower_bound.l == LONG_MIN || lower_bound.l == LONG_MAX) && errno){
			std::cerr << "Lower bound out of range " << LONG_MIN << " to "
				<< LONG_MAX << std::endl;
			return -1;
		}

		upper_bound.l = strtol(argv[optind + 1], &check_c, 10);
		if(*check_c != '\0'){
			print_usage_message(argv[0]);
			return -1;
		}
		if((upper_bound.l == LONG_MIN || upper_bound.l == LONG_MAX) && errno){
			std::cerr << "Upper bound out of range " << LONG_MIN << " to "
				<< LONG_MAX << std::endl;
			return -1;
		}
	}

	/* Generate numbers */
	std::random_device rng;
	std::mt19937_64 prng(rng());
	if(real){
		std::uniform_real_distribution<double>
			dist(lower_bound.f, upper_bound.f);
		for(long i = 0; i < count; i++){
			if(use_pseudo){
				std::cout << dist(prng) << std::endl;
			} else{
				std::cout << dist(rng) << std::endl;
			}
		}
	} else{
		std::uniform_int_distribution<long>
			dist(lower_bound.l, upper_bound.l);
		for(long i = 0; i < count; i++){
			if(use_pseudo){
				std::cout << dist(prng) << std::endl;
			} else{
				std::cout << dist(rng) << std::endl;
			}
		}
	}

	return 0;
}
