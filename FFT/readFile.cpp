#include "readFile.h"

#ifdef QT_PLATFORM
/******************************
*函数名:readFile
*函数功能:读取文件
*参数说明:
*fileName:文件全名
*vecList:输出数据引用
*****************************/
void readFile(const QString& fileName,QVector< QVector<double> >&vecList)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);


	const char *data = (char *)file.map(0,file.size());
	QString str(data);
	QStringList strList = str.split("\n");
	for(int i = 0;i<strList.size()-1;i++)   //导入文件如果最后一行也有回车的情况，所以是size-1
	{
		QStringList strLists = strList[i].split(" ");
		QVector<double>qvecDouble;
		for(int j = 0;j != strLists.size();j++)
		{
			qvecDouble.push_back(strLists[j].toDouble());
		}
		vecList.push_back(qvecDouble);
	}
}
#endif

#ifdef VS_PLATFORM
/***********************************
*函数名:split
*函数功能：分割字符串
*参数说明：
*src:源字符串
*separator: 分割字符
*dest:存储结果
***********************************/
void split(const string& src, const string& separator, vector<string>& dest)
{
	string str = src;
	string substring;
	string::size_type start = 0, index;

	do
	{
		index = str.find_first_of(separator,start);
		if (index != string::npos)
		{
			substring = str.substr(start,index-start);
			dest.push_back(substring);
			start = str.find_first_not_of(separator,index);
			if (start == string::npos) return;
		}
	}while(index != string::npos);

	//the last token
	substring = str.substr(start);
	dest.push_back(substring);
}
/******************************
*函数名:readFile
*函数功能:读取文件,VS平台下内存映射采用SDK的API
*参数说明:
*fileName:文件全名
*vecDoubleList:输出数据引用
*****************************/
void readFile(const string &fileName,vector< vector<double> >&vecDoubleList)
{
	HANDLE hFile = CreateFileA(fileName.c_str(),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return ;
	}
	// Create a file-mapping object for the file.
	HANDLE hFileMapping = CreateFileMappingA(hFile,NULL,PAGE_WRITECOPY,0, 0, NULL);
	char *data = (char *)MapViewOfFile(hFileMapping, FILE_MAP_COPY, 0, 0, 0);

	vector<string>vec;
	vector< vector<string> >veclist;
	split(data,"\n",vec);

	for(int i = 0;i != vec.size();i++)
	{
		vector<string>tmp;
		split(vec[i]," ",tmp);
		veclist.push_back(tmp);
	}
	/*******************************************************************************
	vec.size()修改为veclist.size(), 否则会出现vector越界。同时，移除了除以10000.主要因为
	本函数是一个读取文件通用函数，不是针对地面反射率数据，因此移除了除以10000，并把除以10000
	放到主函数外面。
	revised by SJ NIU, 7/16 2014
	********************************************************************************/
	for(int i = 0; i != veclist.size();i++)
	{
		vector<double>tmp;
		for(int j = 0;j != veclist[0].size();j++)
		{
			tmp.push_back(atof(veclist[i][j].c_str()));//参数除以10000转换
		}
		vecDoubleList.push_back(tmp);
	}

	UnmapViewOfFile(data);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
}
#endif