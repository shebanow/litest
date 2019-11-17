#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <string>
#include <cmath>
#include <getopt.h>
#include <random>
#include "timer.h"
#include "tensor.h"

using namespace std;

// HW Multiplier configuration
struct {
	int N;				// dimension of the vector array input to the MM
	int P;				// dimension of teh operand matrix (PxP)
	int Q; 				// always = P-1
} hwMultConfig = {		// set defaults
	16,
	17,
	16
};

// command line processing options
static int option_maxInt = 16;
static int option_hw_N = 16;
static int option_hw_P = 17;
static int option_minW = 16;
static int option_minH = 16;
static int option_minD = 1;
static int option_maxW = 1024;
static int option_maxH = 768;
static int option_maxD = 16;
static int option_minKW = 1;
static int option_minKH = 1;
static int option_minC = 1;
static int option_maxKW = 11;
static int option_maxKH = 11;
static int option_maxC = 512;

static struct option options[] = {
	// generic help; must ALWAYS be first index
	{ "help", no_argument, NULL, 0 },

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
	{ "maxD", required_argument, &option_maxD, 16 },

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
	fprintf(stderr, "usage: %s <options>\n    where options are:\n", argv[0]);
	fprintf(stderr, "        --hwN <n>\t:\tHW multipler vector width (default %d)\n", option_hw_N);
	fprintf(stderr, "        --hwP <n>\t:\tHW multipler MM dimensions (default %d)\n", option_hw_P);
	fprintf(stderr, "        --minW <n>\t:\tminimum activation tensor width (default %d)\n", option_minW);
	fprintf(stderr, "        --maxW <n>\t:\tmaximum activation tensor width (default %d)\n", option_maxW);
	fprintf(stderr, "        --minH <n>\t:\tminimum activation tensor height (default %d)\n", option_minH);
	fprintf(stderr, "        --maxH <n>\t:\tmaximum activation tensor height (default %d)\n", option_maxH);
	fprintf(stderr, "        --minD <n>\t:\tminimum activation tensor depth (default %d)\n", option_minD);
	fprintf(stderr, "        --maxD <n>\t:\tmaximum activation tensor depth (default %d)\n", option_maxD);
	fprintf(stderr, "        --minC <n>\t:\tminimum filter tensor channel count (default %d)\n", option_minC);
	fprintf(stderr, "        --maxC <n>\t:\tmaximum filter tensor channel count (default %d)\n", option_maxC);
	fprintf(stderr, "        --minKW <n>\t:\tminimum filter tensor width (default %d)\n", option_minKW);
	fprintf(stderr, "        --maxKW <n>\t:\tmaximum filter tensor width (default %d)\n", option_maxKH);
	fprintf(stderr, "        --minKH <n>\t:\tminimum filter tensor height (default %d)\n", option_minKH);
	fprintf(stderr, "        --maxKH <n>\t:\tmaximum filter tensor height (default %d)\n", option_maxKH);
	fprintf(stderr, "        --maxInt <n>\t:\tintegers will be in the range [-n .. n] (default n = %d)\n", option_maxInt);
	fprintf(stderr, "        -h, --help\t:\tprints help\n");
	fprintf(stderr, "        -c   \t\t:\tprints tensor configurations and maxInt before run\n");
	exit(0);
}

// forward declarations
extern Tensor_t<int8_t> *genActivation();
extern TensorArray_t<int8_t> *genFilters(const Tensor_t<int8_t> *);
extern Tensor_t<int8_t> *simulatedConv2D(Tensor_t<int8_t> *, TensorArray_t<int8_t> *); 
extern Tensor_t<float> *referenceConv2D(Tensor_t<float> *, TensorArray_t<float> *);
extern float compareTensors(Tensor_t<int8_t> *, Tensor_t<float> *);
extern void conv2dTrial();

// main program
int main (int argc, char **argv) {
	int c;
	int option_index;
	int opt_c = 0;

	while ((c = getopt_long(argc, argv, "hc", options, &option_index)) != -1) {
		switch (c) {
			case 0:
				if (!option_index) usage(argc, argv);
				*options[option_index].flag = atoi(optarg);
				break;
			case 'c':
				opt_c = 1;
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
	hwMultConfig.N = option_hw_N;
	hwMultConfig.P = option_hw_P;
	hwMultConfig.Q = hwMultConfig.P - 1;

	// if option print requested
	if (opt_c) {
		printf("Ranges: [%d..%d]x[%d..%d]x[%d..%d] by [%d..%d] of [%d..%d]x[%d..%d]x[%d..%d], maxInt = %d\n", 
			option_minW, option_maxW, option_minH, option_maxH, option_minD, option_maxD, option_minC, option_maxC, 
			option_minKW, option_maxKW, option_minKH, option_maxKH, option_minD, option_maxD, option_maxInt);

	}

	// do a trial and quit
	conv2dTrial();
	return 0;
}

// Code to run one trial
void conv2dTrial() {
   	Timer timer;
   	Tensor_t<int8_t> *simulatedActivationTensor, *simulatedResultTensor;
   	TensorArray_t<int8_t> *simulatedFilterSet; 
   	Tensor_t<float> *referenceActivationTensor, *referenceResultTensor;
   	TensorArray_t<float> *referenceFilterSet; 
   	float rmsError;
   	static char geomBuffer[64];
  
    timer.start(); {
    	// generate simulated and reference data sets
		simulatedActivationTensor = genActivation();
		simulatedFilterSet = genFilters(simulatedActivationTensor);
		referenceActivationTensor = new Tensor_t<float>(*simulatedActivationTensor); 
		referenceFilterSet = new TensorArray_t<float>(*simulatedFilterSet);
		sprintf(geomBuffer, "%s by %s", (*simulatedActivationTensor)().c_str(), (*simulatedFilterSet)().c_str());

		// simulate and generate refernce results
		simulatedResultTensor = simulatedConv2D(simulatedActivationTensor, simulatedFilterSet);
		referenceResultTensor = referenceConv2D(referenceActivationTensor, referenceFilterSet);

		// compare
	 	rmsError = compareTensors(simulatedResultTensor, referenceResultTensor);

	 	// cleanup
		delete simulatedActivationTensor;
		delete simulatedFilterSet;
		delete simulatedResultTensor;
		delete referenceActivationTensor;
		delete referenceFilterSet;
		delete referenceResultTensor;
	} timer.stop();
	printf("conv2D trial: %s, %1.2f%% rms error, %s sim time\n", geomBuffer, rmsError * 100.0, timer().c_str());
}

// genActivation: generate an activation tensor using random fixed point data.
// 8-bit integer numbers are assumed signed in the range -128 .. +127. 
// No quantization scale factor is assumed (or in effect, == 1.0; that is, if given a real number N, the corresponding 8-bit integer would have value int(1.0 * N)).
// Limits are assumed in the tensor size generated per command line options.

Tensor_t<int8_t> *genActivation() { 
	// random number generators
	std::random_device rd; 
    std::mt19937 gen(rd());
	std::uniform_int_distribution<> randW(option_minW, option_maxW);
	std::uniform_int_distribution<> randH(option_minH, option_maxH);
	std::uniform_int_distribution<> randD(option_minD, option_maxD);
	std::uniform_int_distribution<> randData(-option_maxInt, option_maxInt);

	// local data
	Tensor_t<int8_t>* act;
	int W, H, D;

	W = randW(gen);
	H = randH(gen);
	D = randD(gen);
	act = new Tensor_t<int8_t>(W, H, D); 
	for (int i = 0; i < W; i++)
		for (int j = 0; j < H; j++)
			for (int k = 0; k < D; k++)
//				(*act)(i,j,k) = rand.uniform(-option_maxInt, option_maxInt);
				(*act)(i,j,k) = randData(gen);
	return act;
}

// genFilters: generate a filter tensor set using random fixed point data.
// Similar to genActivation() in that 8-bit integer numbers are assumed signed in the range -128 .. +127, and similarly no quantization scale factor is employed. 
// Limits are assumed in the tensor array size generated per command line options.

TensorArray_t<int8_t> *genFilters(const Tensor_t<int8_t>* act) { 
	// random number generators
	std::random_device rd; 
    std::mt19937 gen(rd());
	std::uniform_int_distribution<> randKW(option_minKW, option_maxKW);
	std::uniform_int_distribution<> randKH(option_minKH, option_maxKH);
	std::uniform_int_distribution<> randC(option_minC, option_maxC);
	std::uniform_int_distribution<> randData(-option_maxInt, option_maxInt);

	// local data
	TensorArray_t<int8_t>* filterArray;
	Tensor_t<int8_t>* filt;
	int C, KW, KH, D;

	// generate a random filter size, but constrain to be no wider/taller than activation
	// BUG: account for padding
	KW = randKW(gen); if (act->width() < KW) KW = act->width();
	KH = randKH(gen); if (act->height() < KH) KH = act->height();

	// constrain depth to match activation; generate random channel count
	D = act->depth();
	C = randC(gen);

	// generate the filter set and initialize data
	filterArray = new TensorArray_t<int8_t>(C, KW, KH, D); 
	for (int c = 0; c < C; c++)
		for (int i = 0; i < KW; i++)
			for (int j = 0; j < KH; j++)
				for (int k = 0; k < D; k++)
					(*filterArray)[c](i,j,k) = randData(gen);
	return filterArray;
}	

// simulatedConv2D: simulate 2D convolution on 8-bit ints using a simulated HW MM engine.
// Since we assume no padding and stride = 1, the result tensor is inset by 1 on all for sides 
// of the face of the activation sensor and has depth = channel count in the filter set. 
//
// To do this, we successively extract filter-size subtensors (filter tensors from the "filtSet" array)
// from the activation tensor ("act"). Each such subtensor is then serialized (meaning converted from tensor
// to vector). The serialized subtensors (vectors) are the dotted w/ serialized versions of the filter tensors.
// Each dot product represents one element of the output tensor.
//
// In using the HW MM unit, we have to acount for the fact that the core dimension P may be smaller than the
// serialized vector lengths. In this case, we "slice" the vectors in "Q = P - 1" pieces, effectively tiling the 
// dot products. To form an accumulator between pieces, we use the Pth row of the multiplier as the result vector
// from a prior multiply and set the last input vector element to 1 (multiply accumulator by 1 in the dot product):
//
// 					| <--------  P -------->|
//  | res 1 |		| f11  m12  ...  f1Q A1 | | v1 |	^
//  | res 2 |		| f21  m22  ...  f2Q A2 | | v2 |	|
// 	   ...		=	| ...  ...  ...  ... .. | | .. |   	P
//  | res Q |   	| fQ1  mQ2  ...  fQQ AQ | | vQ |	|
//  |   1   |		|  0    0   ...   0   1 | | 1  |	v
//
// The "fij" represent filters @ i/j values; "vi" represents an input vector slice from 1..Q; "res" is the output vector slice.
// In the end, we discard the last row of the result vector (always 1). This form of multiplication can suffer from numeric 
// overflows on the intermediate sums.

Tensor_t<int8_t> *simulatedConv2D(Tensor_t<int8_t> *act, TensorArray_t<int8_t> *filtSet) { 
	printf("Inside simulatedConv2D()\n");

	// serialize versions of the filters
	VectorArray_t<int8_t>* serializedFiltSet = new VectorArray_t<int8_t>(filtSet->count(), filtSet->length());
	for (int c = 0; c < filtSet->count(); c++)
		serializeTensor2Vector((*serializedFiltSet)[c], (*filtSet)[c]);

	// compute output tensor dimensions
	int OW = act->width() - filtSet->width() + 2;
	int OH = act->height() - filtSet->height() + 2;
	int OS = OW * OH;									// output surface count
	int OD = filtSet->count();
	Tensor_t<int8_t>* res = new Tensor_t<int8_t>(OW, OH, OD);

	// main outer loop (over output tensor)
	int i = 0; int j = 0;
	for (int s = 0; s < OS; s += hwMultConfig.N) {
		// extract subtensors from the activation tensor and serialize them into up to N vectors; unused vectors remain 0
		VectorArray_t<int8_t>* actVecArray = new VectorArray_t<int8_t>(hwMultConfig.N, filtSet->length());
		for (int ss = 0; ss < hwMultConfig.N; ss++) {
			int ii = s % OW; int jj = s / OW; if (jj >= OH) break;
			serializeTensor2Vector((*actVecArray)[ss], act->extractSubtensor(ii, jj, 0, filtSet->width(), filtSet->height(), filtSet->depth()));
		}

		// data structures representing the vector matrices going into the HW MM
		VectorArray_t<int8_t>* hwActSliceArray = new VectorArray_t<int8_t>(hwMultConfig.N, hwMultConfig.P); 
		Matrix_t<int8_t>* hwFiltMatrix = new Matrix_t<int8_t>(hwMultConfig.P, hwMultConfig.P);

		// set up "1" elements

		for (int d = 0; d < filtSet->length(); d += hwMultConfig.Q) {
			// set up the N vector slices
			int len = filtSet->length() - d; if (len > hwMultConfig.Q) len = hwMultConfig.Q;
			for (int nn = 0; nn < hwMultConfig.N; nn++) {
				(*hwActSliceArray)[nn].setVec2constant(0);
				(*hwActSliceArray)[nn].extractVecSlice((*actVecArray)[nn], d, len);
				(*hwActSliceArray)[nn](hwMultConfig.Q) = 1;
			}

			int c = 0;
			int width = filtSet->count() - c; if (width > hwMultConfig.Q) width = hwMultConfig.Q;
			hwFiltMatrix->setMatrix2constant(0);
			(*hwFiltMatrix)(hwMultConfig.Q, hwMultConfig.Q) = 1;
			for (int cc = 0; cc < width; cc++) {
				Vector_t<int8_t> temp(hwMultConfig.P);
				hwFiltMatrix->insertRowFromVec(temp.extractVecSlice((*serializedFiltSet)[cc], c, len), cc);
			}

			delete hwActSliceArray;
			delete hwFiltMatrix;
		}

		for (int kk = 0; kk < hwMultConfig.Q; kk++) {

		}

	}
	delete serializedFiltSet;
	return res; 
}

// referenceConv2D: much simpler than simulatedConv2D() as we do not need to serialize or slice the tensors.
// We wimply do the convolutions in 2D by extracting subtensors from the activation tensor and doing
// a direct dot product of the activation subtensor against each filter tensor.

Tensor_t<float> *referenceConv2D(Tensor_t<float> *act, TensorArray_t<float> *filtSet) { 
	printf("Inside referenceConv2D()\n");

	// compute output tensor dimensions
	int OW = act->width() - filtSet->width() - 2;
	int OH = act->height() - filtSet->height() - 2;
	int OS = OW * OH;									// output surface count
	int OD = filtSet->count();

	Tensor_t<float>* res = new Tensor_t<float>(OW, OH, OD);
	for (int c = 0; c < OD; c++) 
		for (int i = 0; i < OW; i++)
			for (int j = 0; j < OH; j++)
				(*res)(i, j, c) = (*filtSet)[c].dot(act->extractSubtensor(i+1, j+1, 0, filtSet->width(), filtSet->height(), filtSet->depth()));
	return res; 
}

// compute the RMS error between the fixed point simulated result and the float reference result
float compareTensors(Tensor_t<int8_t> *simulatedResultTensor, Tensor_t<float> *referenceResultTensor) { 
	printf("Inside compareTensors()\n");

	float error = 0.0;

	int W = simulatedResultTensor->width();
	int H = simulatedResultTensor->height();
	int D = simulatedResultTensor->depth();
	for (int i = 0; i < W; i++)
		for (int j = 0; j < H; j++)
			for (int k = 0; k < D; k++)
				error += pow((float) (*simulatedResultTensor)(i,j,k) - (*referenceResultTensor)(i,j,k), 2.0);
	return pow(error / ((float) (W * H * D)), 0.5); 
}

