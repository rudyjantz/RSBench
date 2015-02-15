#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>
//#include<complex.h>
#include<sys/time.h>
#include"occa_c.h"

#ifdef PAPI
#include "papi.h"
#endif

#define PI 3.14159265359

// typedefs
typedef enum __hm{SMALL, LARGE, XL, XXL} HM_size;

typedef struct{
	double real;
	double imag;
} Complex;

typedef struct{
	int nthreads;
	int n_nuclides;
	int lookups;
	HM_size HM;
	int avg_n_poles;
	int avg_n_windows;
	int numL;
	int doppler;
	char * mode;        // OCCA mode
	int device_id;      // OCCA deviceID
	char * device_info; // OCCA deviceInfo
	char * kernel;      // OCCA kernel
	long outer_dim;     // OCCA outer dimension
	long inner_dim;     // OCCA inner dimension
	long batch_dim;     // OCCA batch dimension
} Input;

typedef struct{
	int * num_nucs;
	int mats_sz;
	int * mats_idx;
	int * mats;
	double * concs;
} Materials;

typedef struct{
	//complex double MP_EA;
	//complex double MP_RT;
	//complex double MP_RA;
	//complex double MP_RF;
	Complex MP_EA;
	Complex MP_RT;
	Complex MP_RA;
	Complex MP_RF;
	short int l_value;
} Pole;

typedef struct{
	double T;
	double A;
	double F;
	int start;
	int end;
} Window;

typedef struct{
	int * n_poles;
	int * n_windows;
	Materials materials;
	Pole ** poles;
	Window ** windows;
	double ** pseudo_K0RS;
	int * poles_idx;
	int * windows_idx;
} CalcDataPtrs;


// io.c
void logo(int version);
void center_print(const char *s, int width);
void border_print(void);
void fancy_int( int a );
Input read_CLI( int argc, char * argv[] );
void print_CLI_error(void);
void print_input_summary(Input input);

// init.c
int * generate_n_poles( Input input );
int * generate_n_windows( Input input );
Pole ** generate_poles( Input input, int * n_poles );
Window ** generate_window_params( Input input, int * n_windows, int * n_poles );
double ** generate_pseudo_K0RS( Input input );
int * load_poles_idx( int n_nuclides, int * n_poles );
int * load_windows_idx( int n_nuclides, int * n_windows );

// material.c
int * load_num_nucs(Input input);
int * load_mats_idx( int * num_nucs );
int * load_mats( int * num_nucs, int * mats_idx, int mats_sz, long n_nuclides );
double * load_concs( int mats_sz );
int pick_mat( unsigned long * seed );
Materials get_materials(Input input);

// utils.c
double rn(unsigned long * seed);
double rn_v(void);
unsigned int hash(unsigned char *str, int nbins);
size_t get_mem_estimate( Input input );
Complex cadd(Complex a, Complex b);
Complex csubtract(Complex a, Complex b);
Complex cmultiply(Complex a, Complex b);
Complex cdivide(Complex a, Complex b);

// xs_kernel.c
//void calculate_macro_xs( double * macro_xs, int mat, double E, Input input, CalcDataPtrs data, complex double * sigTfactors );
//void calculate_micro_xs( double * micro_xs, int nuc, double E, Input input, CalcDataPtrs data, complex double * sigTfactors);
//void calculate_micro_xs_doppler( double * micro_xs, int nuc, double E, Input input, CalcDataPtrs data, complex double * sigTfactors);
//void calculate_sig_T( int nuc, double E, Input input, CalcDataPtrs data, complex double * sigTfactors );
void calculate_macro_xs( double * macro_xs, int mat, double E, Input input, CalcDataPtrs data, Complex * sigTfactors );
void calculate_micro_xs( double * micro_xs, int nuc, double E, Input input, CalcDataPtrs data, Complex * sigTfactors);
void calculate_micro_xs_doppler( double * micro_xs, int nuc, double E, Input input, CalcDataPtrs data, Complex * sigTfactors);
void calculate_sig_T( int nuc, double E, Input input, CalcDataPtrs data, Complex * sigTfactors );

// papi.c
void counter_init( int *eventset, int *num_papi_events );
void counter_stop( int * eventset, int num_papi_events );
