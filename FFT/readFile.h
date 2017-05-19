#ifndef READFILE_H
#define READFILE_H

//#define QT_PLATFORM 1
#define VS_PLATFORM 2

#ifdef QT_PLATFORM
#include <QVector>
#include <QStringList>
#include <QFile>
void readFile(const QString& fileName,QVector< QVector<double> >&vecList);
#endif

#ifdef VS_PLATFORM
#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;
void split(const string& src, const string& separator, vector<string>& dest);
void readFile(const string& fileName,vector< vector<double> >&vecList);
#endif

#endif // READFILE_H
