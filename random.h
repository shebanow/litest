/**
 * @file random.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief simple random class.
 */
#ifndef _RANDOM_H_
#define _RANDOM_H_
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

class Random_t {
public:
	Random_t(unsigned seed = 0) { currentState = initstate(seed, randomState, sizeof(randomState)); }

	// generate uniform random numbers in either floating point or with integers
	float uniform(float max, float min) {
		float result;

		assert(min < max);
		if (currentState != randomState)
			(void) setstate(randomState);
		result = (float) random();
		result /= max - min;
		result += min;
		return result;
	}

	int uniform(int max, int min) {
		int result;

		assert(min < max);
		if (currentState != randomState)
			(void) setstate(randomState);
		result = (int) random();
		result %= max - min + 1;
		result += min;
		return result;
	}

private:
	char randomState[256];
	static char *currentState;
};

#endif // _RANDOM_H_