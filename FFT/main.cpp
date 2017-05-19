// memory leak detact

#include <iostream>
#include <fstream>
#include <math.h>
#include "FFT.h"
#include "readFile.h"

using namespace std;

int main()
{
	
	vector< vector<double> > Input;
	vector< vector<double> > OPTMTF;

	cout<<"read Img"<<endl;
	readFile("A.txt",Input);

	cout<<"read MTF"<<endl;
	readFile("OPTMTF.txt",OPTMTF);

	int height=Input.size();
	int width=Input[0].size();

	ofstream outfile;
	outfile.open("R.txt",ios::out);

	double **ImageData = new double*[height];
	for(int i = 0;i<height;++i)
		ImageData[i] = new double[width];

	double **MtfData = new double*[height];
	for(int i = 0;i<height;++i)
		MtfData[i] = new double[width];

	double **imgBufout = new double*[height];
	for(int i = 0;i<height;++i)
		imgBufout[i] = new double[width];

	for(int i=0;i<height;++i)
		for(int j=0;j<width;++j)
		{
			ImageData[i][j]=(int)Input[i][j];
			MtfData[i][j]=OPTMTF[i][j];
		}

		cout<<"start"<<endl;
		FourierTrans(ImageData,MtfData,imgBufout,width,height);


		cout<<"output res"<<endl;
		int t=0;
		for(int i=0;i<height;++i)
			for(int j=0;j<width;++j)
			{
				t++;
				outfile<<(int)imgBufout[i][j]<<" ";
				if(t%width==0)
					outfile<<endl;
			}
			outfile.close();

			cout<<"Release memory"<<endl;
			// Release Memory
			for(int i=0;i<height;i++)
				delete[] ImageData[i];
			delete[] ImageData;
			for(int i=0;i<height;i++)
				delete[] MtfData[i];
			delete[] MtfData;
			for(int i=0;i<height;i++)
				delete[] imgBufout[i];
			delete[] imgBufout;

			system("pause");
			return 0;
}
