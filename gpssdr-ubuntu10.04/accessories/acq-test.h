#include "includes.h"
#include "prn_codes.h"
#include "fft.h"
#include "simd.h"

//#define SAMPFILE "/home/azimout/samples/joaquim-generated.bin"
//#define SAMPFILE "/home/azimout/samples/iqdata-L12048MS384KHZ.bin"
//#define SAMPFILE "/home/azimout/samples/emsl09.11-2048kS-384kHz.dat"
#define SAMPFILE "/home/azimout/samples/2ms_emsl.bin"

#define SAMPLE_FREQUENCY (2048000)
#define IF_FREQ (38400)
#define DOPPMIN (-15000)
#define DOPPMAX (15000)

int32 file_size;
int32 total_read;
int32 data_size;
int32 samples_range;
int32 scaling;
FILE *fp;
CPX *buff;
struct stat buf;
CPX *fft_codes[MAX_SV];				//!< Store the FFTd Codes;
float relmagn[MAX_SV];					//!< Relative magnitude of 1st to 2nd peak in acq search space for each SV
float REL_THRESHOLD;

enum ACQ_TYPE{ACQ_TYPE_STRONG, ACQ_TYPE_MEDIUM, ACQ_TYPE_WEAK};

CPX *_000Hzwipeoff;						//!< Sinusoid used to perform mix to baseband
CPX	*_250Hzwipeoff;						//!< Sinusoid to mix by Fif - 250 Hz
CPX	*_500Hzwipeoff;						//!< Sinusoid to mix by Fif - 500 Hz
CPX	*_750Hzwipeoff;						//!< Sinusoid to mix by Fif - 750 Hz
CPX *rotate;							//!< Buffer used for circular rotation of vector
CPX *msbuff;							//!< Random buffer for 1 ms stuff
CPX *power;
CPX *baseband;							//!< Result after mixing the buffer to baseband
CPX *coherent;							//!< Used for the 10 ms coherent integration
CPX *baseband_shift;					//!< Result after mixing the buffer to baseband, used for the "circular shifts"
CPX **baseband_rows;					//!< Row pointer
MIX *dft;								//!< Used for the post correlation DFT
MIX **dft_rows;							//!< Used for the post correlation DFT

FFT *pFFT;								//!< The FFT used to perform correlation
FFT *piFFT;								//!< The FFT used to perform correlation

typedef struct Acq_Result{
	int32	code_phase;
	int32	doppler;
	int32	accel;
	uint32	magnitude;
	float	relmagn;
	//int32	success;
} Acq_Result;

Acq_Result results[2][MAX_SV];
int32 success[MAX_SV];

/* FUNCTIONS */
void Read_File();
void InitAcquisition();
void doPrepIF();
void doAcqStrong(int32 _sv, int32 _which);
void Discriminator();
void PrintAcq();
