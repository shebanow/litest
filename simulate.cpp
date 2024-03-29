/**
 * @file simulate.cpp
 * @author m.shebanow
 * @date 11/17/2019
 * @brief core simulator.
 */
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <random>
#include <sys/time.h>
#include "main.h"
#include "tensor.h"
#include "timer.h"

// forward declarations
extern Tensor_t<int8_t> *genActivation();
extern TensorArray_t<int8_t> *genFilters(const Tensor_t<int8_t> *);
extern Tensor_t<int8_t> *simulatedConv2D(Tensor_t<int8_t> *, TensorArray_t<int8_t> *); 
extern Tensor_t<float> *referenceConv2D(Tensor_t<float> *, TensorArray_t<float> *);
extern float compareTensors(Tensor_t<int8_t> *, Tensor_t<float> *);

// Code to run one trial
void conv2dTrial(const char* ofile) {
   	Timer timer;
   	Tensor_t<int8_t> *simulatedActivationTensor, *simulatedResultTensor;
   	TensorArray_t<int8_t> *simulatedFilterSet; 
   	Tensor_t<float> *referenceActivationTensor, *referenceResultTensor;
   	TensorArray_t<float> *referenceFilterSet; 
   	float rmsError;
   	static std::ostringstream buffer;
  
    timer.start(); {
    	// generate simulated and reference data sets
		simulatedActivationTensor = genActivation();
		simulatedFilterSet = genFilters(simulatedActivationTensor);
		referenceActivationTensor = new Tensor_t<float>(*simulatedActivationTensor); 
		referenceFilterSet = new TensorArray_t<float>(*simulatedFilterSet);
		buffer << (*simulatedActivationTensor)().c_str() << " by " << (*simulatedFilterSet)().c_str();

		// if verbose, compare input tensors
		if (option_verbose) {
			float actError, filterError;

			actError = compareTensors(simulatedActivationTensor, referenceActivationTensor);
			for (int n = 0; n < simulatedFilterSet-> count(); n++) {
				float ferr = compareTensors(simulatedFilterSet->pointer(n), referenceFilterSet->pointer(n));
				if (ferr > filterError) filterError = ferr;
			}
			std::cout.precision(2);
			std::cout << "Activation tensor diff = " << actError << ", max filter error = " << filterError << std::endl;
		}

		// simulate and generate refernce results
		simulatedResultTensor = simulatedConv2D(simulatedActivationTensor, simulatedFilterSet);
		referenceResultTensor = referenceConv2D(referenceActivationTensor, referenceFilterSet);

		// compare
	 	rmsError = compareTensors(simulatedResultTensor, referenceResultTensor);

	 	// if diagnostic math dump requested
	 	if (ofile) {
	 		std::filebuf fb;
	 		if (fb.open(ofile, std::ios::out)) {
	 			std::ostream os(&fb);
	 		// 	simulatedActivationTensor->csvDump(os, "simulatedActivationTensor");
				// referenceActivationTensor->csvDump(os, "referenceActivationTensor");
				// simulatedFilterSet->csvDump(os, "simulatedFilterSet");
				// referenceFilterSet->csvDump(os, "referenceFilterSet");
				simulatedResultTensor->csvDump(os, "simulatedResultTensor");
				referenceResultTensor->csvDump(os, "referenceResultTensor");
				fb.close();
	 		} else
	 			perror(ofile);
	 	}

	 	// cleanup
		delete simulatedActivationTensor;
		delete simulatedFilterSet;
		delete simulatedResultTensor;
		delete referenceActivationTensor;
		delete referenceFilterSet;
		delete referenceResultTensor;
	} timer.stop();

	// print results
	std::cout.precision(2);
	std::cout << "conv2D trial: " << buffer.str() << ", " << (rmsError * 100.0) << "% rms error, " << timer().c_str() << " sim time" << std::endl; 
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
// serialized vector lengths. In this case, we "slice" the vectors in "P" pieces, effectively tiling the 
// dot products. The HW multiplier is then doing this for each of the N vectors ("Q = P - 1" below):
//
// 					| <-------  P ------>|
//  | res 1 |		| f11  m12  ...  f1P | | v1 |	^
//  | res 2 |		| f21  m22  ...  f2P | | v2 |	|
// 	   ...		=	| ...  ...  ...  ... | | .. |   	P
//  | res Q |   	| fQ1  mQ2  ...  fQP | | vQ |	|
//  | res P |   	| fP1  mP2  ...  fPP | | vP |	V
//
// The "fij" represent filters @ i/j values; "vi" represents an input vector slice from 1..Q; "res" is the output vector slice.
// This form of multiplication can suffer from numeric overflows on the intermediate sums and in the latter accumulation (and the 
// errors can be pretty extreme).

Tensor_t<int8_t> *simulatedConv2D(Tensor_t<int8_t> *act, TensorArray_t<int8_t> *filtSet) { 
	// model the HW matrices and vector arrays
	VectorArray_t<int8_t> hwMMvectors(hwMM.N, hwMM.P); 
	VectorArray_t<int8_t> hwMMmatrix(hwMM.P, hwMM.P);
	Matrix_t<int8_t> hwMMres(hwMM.N, hwMM.P);

	// serialize versions of the filters
	VectorArray_t<int8_t> serializedFiltSet(filtSet->count(), filtSet->length());
	for (int c = 0; c < filtSet->count(); c++)
		serializeTensor2Vector(serializedFiltSet[c], (*filtSet)[c]);

	// compute output tensor dimensions
	int OW = act->width() - filtSet->width() + 1;
	int OH = act->height() - filtSet->height() + 1;
	int OS = OW * OH;									// output surface count
	int OD = filtSet->count();
	Tensor_t<int8_t>* res = new Tensor_t<int8_t>(OW, OH, OD);

	// get input dimensions
	int IL = filtSet->length();

	// Loop structure:
	// For each C (channel); grab up to next P serialized filters for channel
	//		For each S (tensor) in the surface (2D) of the activation tensor; grab up to next N serialized activation subtensors
	//			For each P-sized slice of both the P serialized filters and N serialized activation subtensors, DOT them to accumulate NxP output elements
	for (int c = 0; c < OD; c += hwMM.P) {
		VectorArray_t<int8_t> actVecArray(hwMM.N, filtSet->length());

		// grab up to next Q serialized filters for channel
		int chanCount = OD - c; if (chanCount > hwMM.P) chanCount = hwMM.P;

		// For each S (tensor) in the surface (2D) of the output tensor
		for (int s = 0; s < OS; s += hwMM.N) {
			// grab up to next N serialized activation subtensors. 
			// extract subtensors from the activation tensor and serialize them into up to N vectors; unused vectors remain 0
			int osLen = OS - s; if (osLen > hwMM.N) osLen = hwMM.N;
			for (int ss = 0; ss < osLen; ss++) {
				int ii = (s+ss) % OW; int jj = (s+ss) / OW; 
				serializeTensor2Vector(actVecArray[ss], act->extractSubtensor(ii, jj, 0, filtSet->width(), filtSet->height(), filtSet->depth()));
			}

			// init HW MM result matrix (the accumulators)
			for (int n = 0; n < hwMM.N; n++)
				hwMMres.setMatrix2constant(0);

			// For each P-sized slice of both the P serialized filters and N serialized activation subtensors
			for (int ijk = 0; ijk < IL; ijk += hwMM.P) {
				// compute slice length
				int sliceLen = IL - ijk; if (sliceLen > hwMM.P) sliceLen = hwMM.P;

				// extract up to N activation surface slices
				for (int ss = 0; ss < osLen; ss++) {
					hwMMvectors[ss].setVec2constant(0);
					hwMMvectors[ss].extractVecSlice(actVecArray[ss], ijk, sliceLen);
				}

				// extract up to P filter vector slices
				for (int cc = 0; cc < chanCount; cc++) {
					hwMMmatrix[cc].setVec2constant(0);
					hwMMmatrix[cc].extractVecSlice(serializedFiltSet[c+cc], ijk, sliceLen);
				}

				// For each P-sized slice of both the P serialized filters and N serialized activation subtensors, DOT them to accumulate NxP output elements
				// This simulates the HW multiplier
				for (int n = 0; n < hwMM.N; n++)
					for (int p = 0; p < hwMM.P; p++)
						hwMMres(n, p) += hwMMvectors[n] * hwMMmatrix[p]; 
			}

			// store the completed accumulators in the result tensor
			for (int ss = 0; ss < osLen; ss++) {
				for (int cc = 0; cc < chanCount; cc++) {
					int ii = (s+ss) % OW; int jj = (s+ss) / OW; 
					(*res)(ii, jj, c+cc) = hwMMres(ss, cc);
				}
			}
		}
	}
	return res; 
}

// referenceConv2D: much simpler than simulatedConv2D() as we do not need to serialize or slice the tensors.
// We wimply do the convolutions in 2D by extracting subtensors from the activation tensor and doing
// a direct dot product of the activation subtensor against each filter tensor.

Tensor_t<float> *referenceConv2D(Tensor_t<float> *act, TensorArray_t<float> *filtSet) { 
	// compute output tensor dimensions
	int OW = act->width() - filtSet->width() + 1;
	int OH = act->height() - filtSet->height() + 1;
	int OS = OW * OH;									// output surface count
	int OC = filtSet->count();

	Tensor_t<float>* res = new Tensor_t<float>(OW, OH, OC);
	for (int c = 0; c < OC; c++) 
		for (int i = 0; i < OW; i++)
			for (int j = 0; j < OH; j++)
				(*res)(i, j, c) = (*filtSet)[c].dot(act->extractSubtensor(i, j, 0, filtSet->width(), filtSet->height(), filtSet->depth()));
	return res; 
}

// compute the RMS error between a fixed point simulated tensor and a float reference tensor
float compareTensors(Tensor_t<int8_t> *sTensor, Tensor_t<float> *rTensor) { 
	float error = 0.0;

	int W = sTensor->width();
	int H = sTensor->height();
	int D = sTensor->depth();
	assert(W == rTensor->width() && H == rTensor->height() && D == rTensor->depth());
	for (int i = 0; i < W; i++)
		for (int j = 0; j < H; j++)
			for (int k = 0; k < D; k++)
				error += pow((float) (*sTensor)(i,j,k) - (*rTensor)(i,j,k), 2.0);
	return pow(error / ((float) (W * H * D)), 0.5); 
}

