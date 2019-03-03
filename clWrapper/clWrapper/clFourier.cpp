#include "clFourier.h"
#include <math.h>


clFourier::~clFourier() {
	if (fftplan) {
		clfftStatus fftStatus = clfftDestroyPlan(&fftplan);
		clFftError::Throw(fftStatus, "clFourier");
	}
}

void clFourier::Setup(unsigned int _width, unsigned int _height) {
	// Perform setup for FFT's
	clfftStatus fftStatus = clfftInitSetupData(&fftSetupData);
	clFftError::Throw(fftStatus);

	fftStatus = clfftSetup(&fftSetupData);
	clFftError::Throw(fftStatus);

	//	Local Data
	clfftDim fftdim = CLFFT_2D;
	clfftResultLocation	place = CLFFT_OUTOFPLACE;
	clfftLayout inLayout  = CLFFT_COMPLEX_INTERLEAVED;
	clfftLayout outLayout = CLFFT_COMPLEX_INTERLEAVED;

	size_t clLengths[ 3 ];
	size_t clPadding[ 3 ] = {0, 0, 0};  // *** TODO
	size_t clStrides[ 4 ];
	size_t batchSize = 1;


	clLengths[0] = _width;
	clLengths[1] = _height;
	clLengths[2]=1;

	clStrides[ 0 ] = 1;
	clStrides[ 1 ] = clStrides[ 0 ] * (clLengths[ 0 ] + clPadding[ 0 ]);
	clStrides[ 2 ] = clStrides[ 1 ] * (clLengths[ 1 ] + clPadding[ 1 ]);
	clStrides[ 3 ] = clStrides[ 2 ] * (clLengths[ 2 ] + clPadding[ 2 ]);

	fftStatus = clfftCreateDefaultPlan(&fftplan, Context.GetContext()(), fftdim, clLengths);
	clFftError::Throw(fftStatus, "clFourier");

	//	Default plan creates a plan that expects an inPlace transform with interleaved complex numbers
	fftStatus = clfftSetResultLocation(fftplan, place);
	clFftError::Throw(fftStatus, "clFourier");
	fftStatus = clfftSetPlanPrecision(fftplan, CLFFT_SINGLE);
	clFftError::Throw(fftStatus, "clFourier");
	fftStatus = clfftSetLayout(fftplan, inLayout, outLayout);
	clFftError::Throw(fftStatus, "clFourier");
	fftStatus = clfftSetPlanBatchSize(fftplan, batchSize);
	clFftError::Throw(fftStatus, "clFourier");
	fftStatus = clfftSetPlanScale(fftplan, CLFFT_FORWARD, 1.0f / sqrtf(_width * _height));
	clFftError::Throw(fftStatus, "clFourier");
	fftStatus = clfftSetPlanScale(fftplan, CLFFT_BACKWARD, 1.0f / sqrtf(_width * _height));
	clFftError::Throw(fftStatus, "clFourier");

	// Not using padding here yet
	if ((clPadding[ 0 ] | clPadding[ 1 ] | clPadding[ 2 ]) != 0) {
		clfftSetPlanInStride  ( fftplan, fftdim, clStrides );
		clfftSetPlanOutStride ( fftplan, fftdim, clStrides );
		clfftSetPlanDistance  ( fftplan, clStrides[ fftdim ], clStrides[ fftdim ]);
	}

	fftStatus = clfftBakePlan(fftplan, 1, &Context.GetQueue()(), NULL, NULL);
	clFftError::Throw(fftStatus, "clFourier");
	
	//get the buffersize
	fftStatus = clfftGetTmpBufSize(fftplan, &buffersize);
	clFftError::Throw(fftStatus, "clFourier");

	if (buffersize) {
		// because buffersize should be in bytes already...
		clMedBuffer = clMemory<char, Manual>(Context, buffersize);
	}
}

clFourier::clFourier(clContext Context, unsigned int _width, unsigned int _height) : Context(Context), width(_width), height(_height), buffersize(0), fftplan(0) {
	Setup(_width, _height);
	AutoTeardownFFT::GetInstance();
};