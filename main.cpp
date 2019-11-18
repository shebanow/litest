/**
 * @file main.cpp
 * @author m.shebanow
 * @date 11/09/2019
 * @brief main program.
 */
#include <stdio.h>
#include <iostream>
#include <getopt.h>
#include "main.h"

using namespace std;

// HW Multiplier configuration
struct hwMM_t hwMM = {		// set defaults
	16,
	16
};

// command line processing options
int option_maxInt = 16;
int option_hw_N = 16;
int option_hw_P = 16;
int option_minW = 16;
int option_minH = 16;
int option_minD = 1;
int option_maxW = 32;
int option_maxH = 32;
int option_maxD = 16;
int option_minKW = 1;
int option_minKH = 1;
int option_minC = 1;
int option_maxKW = 11;
int option_maxKH = 11;
int option_maxC = 32;
int option_verbose = 0;

static struct option options[] = {
	// generic help; must ALWAYS be first index
	{ "help", no_argument, NULL, 0 },
	{ "verbose", no_argument, NULL, 0 },

	// numerical options
	{ "maxInt", required_argument, &option_maxInt, 0 },

	// options for HW multiplier
	{ "hwN", required_argument, &option_hw_N, 0 },
	{ "hwP", required_argument, &option_hw_P, 0 },

	// options for activations; defines ranges on tensor sizes
	{ "minW", required_argument, &option_minW, 0 },
	{ "minH", required_argument, &option_minH, 0 },
	{ "minD", required_argument, &option_minD, 0 },
	{ "maxW", required_argument, &option_maxW, 0 },
	{ "maxH", required_argument, &option_maxH, 0 },
	{ "maxD", required_argument, &option_maxD, 0 },

	// options for filters; defines ranges on filter sizes
	{ "minKW", required_argument, &option_minKW, 0 },
	{ "minKH", required_argument, &option_minKH, 0 },
	{ "minC", required_argument, &option_minC, 0 },
	{ "maxKW", required_argument, &option_maxKW, 0 },
	{ "maxKH", required_argument, &option_maxKH, 0 },
	{ "maxC", required_argument, &option_maxC, 0 },

	// null termination
	{ 0, 0, 0, 0 }
};

void usage(int argc, char **argv) {
	cerr << "usage: " << argv[0] << " <options>\n    where options are:" << std::endl;
	cerr << "        --hwN <n>\t:\tHW multipler vector width (default " << option_hw_N << ")" << std::endl;
	cerr << "        --hwP <n>\t:\tHW multipler MM dimensions (default " << option_hw_P << ")" << std::endl;
	cerr << "        --minW <n>\t:\tminimum activation tensor width (default " << option_minW << ")" << std::endl;
	cerr << "        --maxW <n>\t:\tmaximum activation tensor width (default " << option_maxW << ")" << std::endl;
	cerr << "        --minH <n>\t:\tminimum activation tensor height (default " << option_minH << ")" << std::endl;
	cerr << "        --maxH <n>\t:\tmaximum activation tensor height (default " << option_maxH << ")" << std::endl;
	cerr << "        --minD <n>\t:\tminimum activation tensor depth (default " << option_minD << ")" << std::endl;
	cerr << "        --maxD <n>\t:\tmaximum activation tensor depth (default " << option_maxD << ")" << std::endl;
	cerr << "        --minC <n>\t:\tminimum filter tensor channel count (default " << option_minC << ")" << std::endl;
	cerr << "        --maxC <n>\t:\tmaximum filter tensor channel count (default " << option_maxC << ")" << std::endl;
	cerr << "        --minKW <n>\t:\tminimum filter tensor width (default " << option_minKW << ")" << std::endl;
	cerr << "        --maxKW <n>\t:\tmaximum filter tensor width (default " << option_maxKH << ")" << std::endl;
	cerr << "        --minKH <n>\t:\tminimum filter tensor height (default " << option_minKH << ")" << std::endl;
	cerr << "        --maxKH <n>\t:\tmaximum filter tensor height (default " << option_maxKH << ")" << std::endl;
	cerr << "        --maxInt <n>\t:\tintegers will be in the range [-n .. n] (default n = " << option_maxInt << ")" << std::endl; 
	cerr << "        -v, --verbose\t:\tbe verbose" << std::endl;
	cerr << "        -h, --help\t:\tprints help" << std::endl;
	cerr << "        -o <file>\t:\tsave matrices to csv file" << std::endl;
	cerr << "        -c   \t\t:\tprints tensor configurations and maxInt before run" << std::endl;
	exit(0);
}

// forward declarations
extern void conv2dTrial(const char *);

// main program
int main (int argc, char **argv) {
	int c;
	int option_index;
	int opt_c = 0;
	char *opt_o = NULL;

	while ((c = getopt_long(argc, argv, "vhco:", options, &option_index)) != -1) {
		switch (c) {
			case 0:
				if (!option_index) usage(argc, argv);
				if (option_index == 1) option_verbose = 1;
				*options[option_index].flag = atoi(optarg);
				break;
			case 'c':
				opt_c = 1;
				break;
			case 'v':
				option_verbose = 1;
				break;
			case 'o':
				opt_o = optarg;
				break;
			case 'h':
				usage(argc, argv);
				break;
			default:
				break;
		}
	}

	// range check options
	if (option_maxInt < 2 || option_maxInt > 127)
		option_maxInt = 127;
	if (option_hw_N < 2)
		option_hw_N = 2;
	if (option_hw_P < 3)
		option_hw_P = 3;

	// set up HW config
	hwMM.N = option_hw_N;
	hwMM.P = option_hw_P;

	// if option print requested
	if (opt_c) {
		cout << "HW MM: " << hwMM.N << " vectors by " << hwMM.P << " x " << hwMM.P << " MM" << std::endl;
		cout << "Ranges: [" << option_minW << ".." << option_maxW << "]x[" << option_minH << ".." << option_maxH << "]x[" << option_minD << ".." << option_maxD << "] by [" << 
				option_minC << ".." << option_maxC << "] of [" << option_minKW << ".." << option_maxKW << "]x[" << option_minKH << ".." << option_maxKH << "]x[" << option_minD << ".." << option_maxD << 
				"], maxInt = " << option_maxInt << std::endl;
	}

	// do a trial and quit
	conv2dTrial(opt_o);
	return 0;
}
