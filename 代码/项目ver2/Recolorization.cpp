// Image.cpp: 定义控制台应用程序的入口点。

#include "stdafx.h"
#include<iostream>
#include<string>
#include<cmath>
#include<opencv2\opencv.hpp>
#include<opencv2\highgui.hpp>
#include<opencv2\core.hpp>
#include<opencv2\imgproc\imgproc.hpp>
using namespace cv;
using namespace std;

void CreateWindowShow(const char a[], const Mat& b)
{
	namedWindow(a);
	imshow(a, b);
}

unsigned char _Matrix[1024][1024];

int main()
{
	string Fname1, Fname2, Tem;
	Mat Pic1, Pic2;
	cout << "Choose the Picture to Re-Colorize :\n";
	while (true)
	{
		cin >> Fname1;
		Pic1 = imread(Fname1.c_str());
		if (!Pic1.data)
		{
			cout << "Picture Load Failed, Please Enter the File Path again.\n";
			continue;
		}
		else if (Fname1 == "exit")
			return 0;
		else if (Pic1.data && (Pic1.rows > 1000 || Pic1.cols > 1000 || Pic1.rows*Pic1.cols > 512000))
		{
			cout << "图片过大，请重新选择\n";
			Pic1.release();
			continue;
		}
		else
			break;
	}
	cout << Fname1 << " loaded.\n\n";
	cout << "Choose the Target Picture to refer to :\n";
	while (true)
	{
		cin >> Fname2;
		Pic2 = imread(Fname2.c_str());
		if (!Pic2.data)
		{
			cout << "Picture Load Failed, Please Enter the File Path again.\n";
			continue;
		}
		else if (Fname2 == "exit")
			return 0;
		else if (Pic2.data)
		{
			if (Pic2.rows > 1000 || Pic2.cols > 1000 || Pic2.rows*Pic2.cols > 500000)
			{
				double scale = max(Pic2.rows, Pic2.cols) / 800.0;
				Size dsize = Size(Pic2.cols / scale, Pic2.rows / scale);
				resize(Pic2, Pic2, dsize);
			}
			break;
		}
	}
	cout << Fname2 << " loaded.\n\n";

	//Pic1 = imread("D:/d.jpg");
	//Pic2 = imread("D:/d.jpg");
	CreateWindowShow("Source", Pic1);
	CreateWindowShow("Target", Pic2);
	int Row1 = Pic1.rows, Col1 = Pic1.cols, Row2 = Pic2.rows, Col2 = Pic2.cols;
	
	GaussianBlur(Pic2, Pic2, Size(3, 3), 0);
	unsigned long long Sl1 = 0, Sa1 = 0, Sb1 = 0, Sl2 = 0, Sa2 = 0, Sb2 = 0;
	double SPl1 = 0, SPa1 = 0, SPb1 = 0, SPl2 = 0, SPa2 = 0, SPb2 = 0;
	double Al1, Aa1, Ab1, Al2, Aa2, Ab2;
	double SDl1, SDa1, SDb1, SDl2, SDa2, SDb2;

	Mat PSrc = Pic1.clone();
	Mat PTar = Pic2.clone();
	
	int Pos = 0;
	for (auto it = PTar.begin<Vec3b>(); it != PTar.end<Vec3b>(); it++)
	{
		short Max = (*it)[0], Mid = (*it)[1], Min = (*it)[2];
		if (Max < Mid)
			swap(Max, Mid);
		if (Max < Min)
			swap(Max, Min);
		if (Min > Mid)
			swap(Min, Mid);
		if (Max < 16 || Min >= 247 || Max - Min <= 4)//纯灰
			_Matrix[Pos / Row2][Pos%Row2] = 1;
		if (Max < 64 || Max - Min < 24)//
			_Matrix[Pos / Row2][Pos%Row2] = 2;
		else if ((float)(Max - Min) / Max > 0.75)//饱和
			_Matrix[Pos / Row2][Pos%Row2] = 4;
		else
			_Matrix[Pos / Row2][Pos%Row2] = 3;
		Pos++;
	}
	Pos = 0;
	unsigned int PivNum = 0;
	cvtColor(PTar, PTar, COLOR_BGR2Lab);//

	for (auto it = PTar.begin<Vec3b>(); it != PTar.end<Vec3b>(); it++)
	{
		int T = _Matrix[Pos / Row2][Pos%Row2];
		PivNum += T;
		Sl2 += T * (*it)[0];
		Sa2 += T * (*it)[1];
		Sb2 += T * (*it)[2];
		Pos++;
	}
	Al2 = (double)Sl2 / PivNum;
	Aa2 = (double)Sa2 / PivNum;
	Ab2 = (double)Sb2 / PivNum;
	Pos = 0;
	for (auto it = PTar.begin<Vec3b>(); it != PTar.end<Vec3b>(); it++)
	{
		int T = _Matrix[Pos / Row2][Pos%Row2];
		SPl2 += T * pow((*it)[0] - Al2, 2);
		SPa2 += T * pow((*it)[1] - Aa2, 2);
		SPb2 += T * pow((*it)[2] - Ab2, 2);
		Pos++;
	}
	SDl2 = sqrt(SPl2 / PivNum);
	SDa2 = sqrt(SPa2 / PivNum);
	SDb2 = sqrt(SPb2 / PivNum);

	Pos = 0;
	for (auto it = PSrc.begin<Vec3b>(); it != PSrc.end<Vec3b>(); it++)
	{
		short Max = (*it)[0], Mid = (*it)[1], Min = (*it)[2];
		if (Max < Mid)
			swap(Max, Mid);
		if (Max < Min)
			swap(Max, Min);
		if (Min > Mid)
			swap(Min, Mid);
		if (Max < 16 || Min >= 247 || Max - Min <= 4)
			_Matrix[Pos / Row1][Pos%Row1] = 4;
		if (Max < 64 || Max - Min < 24)
			_Matrix[Pos / Row1][Pos%Row1] = 5;
		else if ((float)(Max - Min) / Max > 0.75)
			_Matrix[Pos / Row1][Pos%Row1] = 7;
		else
			_Matrix[Pos / Row1][Pos%Row1] = 6;
		Pos++;
	}
	Pos = 0;
	PivNum = 0;
	cvtColor(PSrc, PSrc, COLOR_BGR2Lab);
	for (auto it = PSrc.begin<Vec3b>(); it != PSrc.end<Vec3b>(); it++)
	{
		int T = _Matrix[Pos / Row1][Pos%Row1];
		PivNum += T;
		Sl1 += T * (*it)[0];
		Sa1 += T * (*it)[1];
		Sb1 += T * (*it)[2];
		Pos++;
	}
	Al1 = (double)Sl1 / PivNum;
	Aa1 = (double)Sa1 / PivNum;
	Ab1 = (double)Sb1 / PivNum;
	
	Pos = 0;
	for (auto it = PSrc.begin<Vec3b>(); it != PSrc.end<Vec3b>(); it++)
	{
		int T = _Matrix[Pos / Row1][Pos%Row1];
		SPl1 += T * pow((*it)[0] - Al1, 2);
		SPa1 += T * pow((*it)[1] - Aa1, 2);
		SPb1 += T * pow((*it)[2] - Ab1, 2);
		Pos++;
	}
	SDl1 = sqrt(SPl1 / PivNum);
	SDa1 = sqrt(SPa1 / PivNum);
	SDb1 = sqrt(SPb1 / PivNum);

	if (SDl1 < 4 || SDa1 < 1.0 || SDb1 < 1.0)
	{
		cout << "请勿选择纯色图片\n";
		waitKey(0);
		system("pause");
		return 0;
	}

	double L, A, B;
	L = SDl2 / SDl1;
	A = SDa2 / SDa1;
	B = SDb2 / SDb1;
	cout << "Base Color:\n" << Al2 << " \t " << Aa2 << " \t " << Ab2 << endl;
	cout << "Base SD:\n" << SDl2 << " \t " << SDa2 << " \t " << SDb2 << endl;
	cout << "Deviation:\n" << L << " \t " << A << " \t " << B << "\n";

	int OverFlow = 0;
	for (auto it = PSrc.begin<Vec3b>(); it != PSrc.end<Vec3b>(); it++)
	{
		double T1, T2, T3;
		T1 = L * ((*it)[0] - Al1) + Al2;
		T2 = A * ((*it)[1] - Aa1) + Aa2;
		T3 = B * ((*it)[2] - Ab1) + Ab2;
		if (T1 > 255)
		{
			(*it)[0] = 255;
			OverFlow++;
		}
		else if (T1 < 0)
		{
			(*it)[0] = 0;
			OverFlow++;
		}
		else
			(*it)[0] = (uchar)T1;
		if (T2 > 255)
		{
			(*it)[1] = 255;
			OverFlow++;
		}
		else if (T2 < 0)
		{
			(*it)[1] = 0;
			OverFlow++;
		}
		else
			(*it)[1] = (uchar)T2;
		if (T3 > 255)
		{
			(*it)[2] = 255;
			OverFlow++;
		}
		else if (T3 < 0)
		{
			(*it)[2] = 0;
			OverFlow++;
		}
		else
			(*it)[2] = (uchar)T3;
	}
	cvtColor(PSrc, PSrc, COLOR_Lab2BGR);
	//结束
	CreateWindowShow("Result", PSrc);
	cout << "OverFlow:\n" << OverFlow << " \t " << (double)OverFlow / Row1 / Col1 << endl;
	waitKey(0);

	cout << "Save this image? [y]/n\n";
	cin >> Tem;
	if (Tem == "y" || Tem == "Y")
	{
		imwrite("mySave.bmp", PSrc);
		cout << "File saved as D:/mySave.bmp\n";
	}

	system("pause");
	return 0;
}
