#include <stdio.h>
#include <sys/time.h>
#include <assert.h>
#include <string>
#include <cmath>
#include "timer.h"
#include "tensor.h"

using namespace std;

// forward declarations
extern Tensor_t<int8_t> *genActivation();
extern TensorArray_t<int8_t> *genFilters();
extern Tensor_t<int8_t> *simulatedConv2D(Tensor_t<int8_t> *, TensorArray_t<int8_t> *); 
extern Tensor_t<float> *referenceConv2D(Tensor_t<float> *, TensorArray_t<float> *);
extern float compareTensors(Tensor_t<int8_t> *, Tensor_t<float> *);
extern void conv2dTrial();

int main (int argc, const char * argv[]) {
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
		simulatedFilterSet = genFilters();
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

Tensor_t<int8_t> *genActivation() { return new Tensor_t<int8_t>(5, 5, 5); }
TensorArray_t<int8_t> *genFilters() { return new TensorArray_t<int8_t>(10, 5, 5, 5); }	
Tensor_t<int8_t> *simulatedConv2D(Tensor_t<int8_t> *, TensorArray_t<int8_t> *) { return new Tensor_t<int8_t>(5, 5, 5); }
Tensor_t<float> *referenceConv2D(Tensor_t<float> *, TensorArray_t<float> *) { return new Tensor_t<float>(5, 5, 5); }

// compute the RMS error between the fixed point simulated result and the float reference result
float compareTensors(Tensor_t<int8_t> *simulatedResultTensor, Tensor_t<float> *referenceResultTensor) { 

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

