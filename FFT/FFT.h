#include <iostream>
#include <math.h>
#include <vld.h>
#define PI 3.14159265358979323846


// 复数类型结构体
struct ComplexNumber
{
	float imag; //虚部
	float real; //实部
};

using namespace std;

// 一维FFT
void FFT1D(ComplexNumber *arrayBuf, int n);
// 一维IFFT
void IFFT1D(ComplexNumber *arrayBuf, int n);

// 二维FFT
void ImgFFT2D(double* imgBuf, int width, int height,double*imgBufOut,ComplexNumber *m_pFFTBuf);
// 二维IFFT
void ImgIFFT2D(double* imgBufOut, int width, int height,ComplexNumber *m_pFFTBuf);

// 频率平移
void FFTShift(int width,int height,ComplexNumber *m_pFFTBuf);

// 图像大小扩充
void ImgSizeHandle(double *imgBuf, double *&imgBufout,int &width, int &height);

// 图像大小复原
void ResetImgSize(double *imgBuf, double *&imgBufout,int width, int height,int pWidth,int pHeight);

// 频域内卷积运算
void FourierTrans(double **imgInput,double **mtfInput ,double **imgBufout,int pWidth,int pHeight);
