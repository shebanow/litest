/**
 * @file main.h
 * @author m.shebanow
 * @date 11/27/2019
 * @brief headers for main.cpp.
 */
#ifndef _MAIN_H_
#define _MAIN_H_

// HW multiplier config
struct hwMM_t {
	int N;				// dimension of the vector array input to the MM (N)
	int P;				// dimension of the operand matrix (PxP)
}; 

extern hwMM_t hwMM;

// command line processing options
extern int option_maxInt;
extern int option_hw_N;
extern int option_hw_P;
extern int option_minW;
extern int option_minH;
extern int option_minD;
extern int option_maxW;
extern int option_maxH;
extern int option_maxD;
extern int option_minKW;
extern int option_minKH;
extern int option_minC;
extern int option_maxKW;
extern int option_maxKH;
extern int option_maxC;
extern int option_verbose;

#endif // _MAIN_H_