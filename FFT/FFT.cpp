#include "FFT.h"

//一维FFT
void FFT1D(ComplexNumber *arrayBuf, int n)
{
	//循环变量
	int i, k, r;

	//申请临时复数缓冲区buf1，长度为n
	ComplexNumber *buf1=new ComplexNumber[n];

	//将arrayBuf拷贝进buf1
	memcpy(buf1,arrayBuf,sizeof(ComplexNumber)*n);

	//申请临时复数缓冲区buf2，长度为n
	ComplexNumber *buf2=new ComplexNumber[n];

	//将arrayBuf数组元素基2抽取并重新排列
	//若0、1、2、3、4、5、6、7八点序列对调后变作0、4、2、6、1、5、3、7
	int t1,t2;
	for(r=1;pow(2.0,r)<n;r++){
		t1=pow(2.0,r);
		t2=pow(2.0,r-1);
		for(k=0;k<t1;k++){
			for(i=0;i<n/t1;i++){
				buf2[k*n/t1+i].real=buf1[k/2*n/t2+i*2+k%2].real;
				buf2[k*n/t1+i].imag=buf1[k/2*n/t2+i*2+k%2].imag;
			}
		}
		memcpy(buf1,buf2,sizeof(ComplexNumber)*n);
	}


	//采用蝶型算法进行快速傅立叶变换
	//buf1是第r级的输入，buf2存放第r级的输出
	float c,s;
	for(r=1;pow(2.0,r)<=n;r++){
		t1=pow(2.0,r);
		for(k=0;k<n/t1;k++){
			for(i=t1/2;i<t1;i++){
				//加权因子
				c=cos(-2*PI*(i-t1/2)/t1);
				s=sin(-2*PI*(i-t1/2)/t1);
				buf1[k*t1+i].real= buf2[k*t1+i].real*c - buf2[k*t1+i].imag*s;
				buf1[k*t1+i].imag=buf2[k*t1+i].imag*c+buf2[k*t1+i].real*s;
			}
		}
		for(k=0; k<n/t1; k++){
			for(i=0;i<t1/2;i++){
				buf2[k*t1+i].real= buf1[k*t1+i].real+buf1[k*t1+i+t1/2].real;
				buf2[k*t1+i].imag= buf1[k*t1+i].imag+buf1[k*t1+i+t1/2].imag;
			}
			for(i=t1/2;i<t1;i++){
				buf2[k*t1+i].real= buf1[k*t1+i-t1/2].real-buf1[k*t1+i].real;
				buf2[k*t1+i].imag= buf1[k*t1+i-t1/2].imag-buf1[k*t1+i].imag;
			}
		}

		//第r级的输出存入buf1,作为下一级的输入数据
		memcpy(buf1,buf2,sizeof(ComplexNumber)*n);
	}


	//傅立叶变换的结果存入arrayBuf
	memcpy(arrayBuf,buf2,sizeof(ComplexNumber)*n);

	//释放缓冲区
	delete[]buf2;
	delete[]buf1;

}
//二维FFT
void ImgFFT2D(double* imgBuf, int width, int height,double *imgBufOut,ComplexNumber *m_pFFTBuf)
{
	//循环变量
	int i, j, u, v;

	//图像数据变成复数类型存入m_pFFTBuf
	for(i=0;i<width*height;i++){
		m_pFFTBuf[i].real=imgBuf[i];
		m_pFFTBuf[i].imag=0;
	}

	//申请ComplexNumber结构体数组,长度为height
	ComplexNumber *array=new ComplexNumber[height];

	//先纵向一维快速傅立叶变换
	for(u=0;u<width;u++){
		for(v=0;v<height;v++){
			array[v].real=m_pFFTBuf[v*width+u].real;
			array[v].imag=m_pFFTBuf[v*width+u].imag;
		}
		FFT1D(array, height);
		for(v=0;v<height;v++){
			m_pFFTBuf[v*width+u].real=array[v].real;
			m_pFFTBuf[v*width+u].imag=array[v].imag;
		}
	}
	delete []array;

	//再横向一维快速傅立叶变换
	for(v=0;v<height;v++){
		FFT1D(m_pFFTBuf+v*width, width);
	}

	//将频谱图以图像形式存入imgBufOut
	float t;
	int i0,j0;
	for(i=0;i<height;i++){
		//i0 = i;
		//j0 = j;
		for(j=0;j<width;j++){
			if(i<height/2)
				i0=i+height/2;
			else
				i0=i-height/2;
			if(j<width/2)
				j0=j+width/2;
			else
				j0=j-width/2;

			t=sqrt(m_pFFTBuf[i0*width+j0].real*m_pFFTBuf[i0*width+j0].real
				+m_pFFTBuf[i0*width+j0].imag*m_pFFTBuf[i0*width+j0].imag);
			t=t/500;
			if(t>255)
				imgBufOut[i*width+j]=255;
			else 
				imgBufOut[i*width+j]=t;
		}
	}
}

//一维IFFT
void IFFT1D(ComplexNumber *arrayBuf, int n)
{
	//变量
	int i;

	//对数组arrayBuf求共轭
	for(i=0;i<n;i++)
		arrayBuf[i].imag=-arrayBuf[i].imag;

	//一维快速傅立叶变换
	FFT1D(arrayBuf, n);

	//时域数据求共轭，并除以N
	for(i=0;i<n;i++){
		arrayBuf[i].real=arrayBuf[i].real/n;
		arrayBuf[i].imag=-arrayBuf[i].imag/n;
	}

}

// 二维IFFT
void ImgIFFT2D(double* imgBufOut, int width, int height,ComplexNumber *m_pFFTBuf)
{
	//若傅立叶变换缓冲区为空,则失败返回
	if(!m_pFFTBuf)
		return;

	//变量
	int i, u, v;

	//先纵向傅立叶反变换
	ComplexNumber *array=new ComplexNumber[height];
	for(u=0;u<width;u++){
		for(v=0;v<height;v++){
			array[v].real=m_pFFTBuf[v*width+u].real;
			array[v].imag=m_pFFTBuf[v*width+u].imag;
		}
		IFFT1D(array, height);
		for(v=0;v<height;v++){
			m_pFFTBuf[v*width+u].real=array[v].real;
			m_pFFTBuf[v*width+u].imag=array[v].imag;
		}
	}
	delete []array;

	//再横向傅立叶反变换
	for(v=0;v<height;v++){
		IFFT1D(m_pFFTBuf+v*width, width);
	}

	//求m_pFFTBuf的幅度谱,恢复图像存入imgBufOut
	float t;
	for(i=0;i<width*height;i++){
		t=sqrt(m_pFFTBuf[i].real*m_pFFTBuf[i].real
			+m_pFFTBuf[i].imag*m_pFFTBuf[i].imag);
		if(t>255)
			imgBufOut[i]=255;
		else imgBufOut[i]=t;
	}
}

// 图像尺寸处理(宽、高为2的幂次方)
void ImgSizeHandle(double *imgBuf, double *&imgBufout, int &width, int &height)
{
	//判别高、宽是2的幂次方
	int flag1=1,flag2=1;
	int isValidateFlag=1;

	while(flag1<height)
		flag1=flag1*2;
	if(flag1!=height)			//图像高不是2的幂次方
		isValidateFlag=0;

	while(flag2<width)
		flag2=flag2*2;
	if(flag2!=width)			//图像宽不是2的幂次方
		isValidateFlag=0;

	if(isValidateFlag==0)
	{

		imgBufout=new double[flag1*flag2];
		memset(imgBufout, 0, sizeof(double)*flag1*flag2);
		int x=(flag1-height)/2;
		int y=(flag2-width)/2;
		int k=0;
		for(int i=x;i<x+height;++i)
			for(int j=y;j<y+width;++j)
			{
				imgBufout[i*flag2+j]=imgBuf[k++];
			}
			height=flag1;
			width=flag2;
	}
	else
		imgBufout=imgBuf;
}


// 频率中心移动
void FFTShift(int width,int height,ComplexNumber *m_pFFTBuf)
{

	// 针对二维复矩阵
	int mid=height/2;
	if(height%2==1)
		mid=mid+1;

	ComplexNumber *tmpBuf=new ComplexNumber[mid];
	for(int i=0;i<width;++i)
	{
		for(int k=0;k<mid;++k)
			tmpBuf[k]=m_pFFTBuf[k*width+i];
		int t=0;
		for(int k=mid;k<height;++k)
		{
			m_pFFTBuf[t*width+i]=m_pFFTBuf[k*width+i];
			++t;
		}
		int p=0;
		for(int k=t;k<height;++k)
		{
			m_pFFTBuf[k*width+i]=tmpBuf[p];
			++p;
		}
	}
	delete []tmpBuf;

	mid=width/2;
	if(width%2==1)
		mid=mid+1;
	tmpBuf=new ComplexNumber[mid];
	for(int i=0;i<height;++i)
	{
		for(int k=0;k<mid;++k)
			tmpBuf[k]=m_pFFTBuf[i*width+k];
		int t=0;
		for(int k=mid;k<width;++k)
		{
			m_pFFTBuf[i*width+t]=m_pFFTBuf[i*width+k];
			++t;
		}
		int p=0;
		for(int k=t;k<width;++k)
		{
			m_pFFTBuf[i*width+k]=tmpBuf[p];
			++p;
		}
	}
	delete []tmpBuf;
}


void ResetImgSize(double *imgBuf, double *&imgBufout,int width, int height,int pWidth,int pHeight)
{
	imgBufout=new double[pWidth*pHeight];
	memset(imgBufout,0,sizeof(double)*pWidth*pHeight);

	int x=(height-pHeight)/2;
	int y=(width-pWidth)/2;

	int k=0;
	for(int i=x;i<x+pHeight;++i)
	{
		for(int j=y;j<y+pWidth;++j)
		{
			imgBufout[k++]=imgBuf[i*width+j];
		}
	}
}


void FourierTrans(double **imgInput,double **mtfInput ,double **imgBufout,int pWidth,int pHeight)
{

	// 变换过程中图像大小
	int width,height;
	// 赋予原大小
	width=pWidth;
	height=pHeight;

	double *m_pImgData,*m_pImgResize,*m_pImgDataOut,*finalRes; 

	// 二维数组->>一维数组
	m_pImgData=new double[pWidth*pHeight];
	memset(m_pImgData, 0, sizeof(double)*pWidth*pHeight);
	int k=0;
	for(int i=0;i<pHeight;++i)
		for(int j=0;j<pWidth;++j)
			m_pImgData[k++]=imgInput[i][j];



	// 图像大小处理、返回指向新空间指针m_pImgResize(ImgSizeHandle内开辟空间)
	m_pImgResize=NULL;
	ImgSizeHandle(m_pImgData,m_pImgResize,width,height);

	// 申请傅里叶缓冲区
	ComplexNumber *m_pFFTBuf=new ComplexNumber[width*height];
	memset(m_pFFTBuf, 0, sizeof(ComplexNumber)*width*height);

	m_pImgDataOut=new double[width*height];
	memset(m_pImgDataOut, 0, sizeof(double)*width*height);


	// FFT
	ImgFFT2D(m_pImgResize, width, height, m_pImgDataOut, m_pFFTBuf);


	// FFTShift
	FFTShift(width,height,m_pFFTBuf);

	//频域卷积操作
	int x=(height-pHeight)/2;
	int y=(width-pWidth)/2;
	for(int i=x;i<x+pHeight;++i)
	{
		for(int j=y;j<y+pWidth;++j)
		{
			m_pFFTBuf[i*width+j].real=m_pFFTBuf[i*width+j].real*mtfInput[i-x][j-y];
			m_pFFTBuf[i*width+j].imag=m_pFFTBuf[i*width+j].imag*mtfInput[i-x][j-y];
		}
	}

	// IFFT
	ImgIFFT2D(m_pImgDataOut, width, height,m_pFFTBuf);

	// 图像大小恢复
	ResetImgSize(m_pImgDataOut,finalRes,width,height,pWidth,pHeight);

	int m,n;
	// 返回二维数组
	for(int i=0;i<pWidth*pHeight;++i)
	{
		m=i/pWidth;
		n=i%pWidth;
		imgBufout[m][n]=finalRes[i];
	}


	//内存释放
	if(m_pImgData==m_pImgResize)
		delete []m_pImgData;
	else
	{
		delete []m_pImgData;
		delete []m_pImgResize;
	}
	delete []m_pImgDataOut;
	delete []finalRes;
	delete []m_pFFTBuf;

}