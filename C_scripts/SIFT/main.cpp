#include <iostream>
using namespace std;

#include "sift.h"



//GaborKernel:模板
//width:模板的宽度
//height:模板的高度
//sigma=4		高斯函数的标准差
//theta=47		Gabor核函数的方向
//lambd=11		正弦波波长
//gamma=0.5		空间宽高比
//psi=0.5*PI	相位偏移
bool GaborKernel(vector<vector<double>>& GaborKernel, int width, int height, double sigma, double theta, double lambd, double gamma, double psi)
{
	long double PI = 3.141592653589793;
	double sigma_x = sigma;
	double sigma_y = sigma/gamma;
	int nstds = 3;
	int xmin=0, xmax=0, ymin=0, ymax=0;//初始化一定赋值
	double c = cos(theta), s = sin(theta);

	if( width > 0 )
		xmax = width/2;
	else
		xmax = cvRound(std::max(fabs(nstds*sigma_x*c), fabs(nstds*sigma_y*s)));

	if( height > 0 )
		ymax = height/2;
	else
		ymax = cvRound(std::max(fabs(nstds*sigma_x*s), fabs(nstds*sigma_y*c)));

	xmin = -xmax;
	ymin = -ymax;

	GaborKernel.resize(ymax - ymin + 1);//重新定义大小

	double scale = 1;
	double ex = -0.5/(sigma_x*sigma_x);
	double ey = -0.5/(sigma_y*sigma_y);
	double cscale = PI*2/lambd;

	for( int y = ymin; y <= ymax; y++ )
	{
		for( int x = xmin; x <= xmax; x++ )
		{
			double xr = x*c + y*s;
			double yr = -x*s + y*c;
			double v = scale*std::exp(ex*xr*xr + ey*yr*yr)*cos(cscale*xr + psi);
			(GaborKernel[ymax - y]).push_back(v);
		}
	}
	return true;
};


int main(int argc, char **argv)
{
	Mat src = imread("jobs.jpg");


	vector<Keypoint> features;
	Sift(src, features, 1.6);
	DrawKeyPoints(src, features);
	DrawSiftFeatures(src, features);

	write_features(features, "descriptor.txt");

	imshow("src", src);
	imwrite("output.jpg", src);
	waitKey();
	
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	vector<vector<double>> GKernel;
	GaborKernel(GKernel,5,5,4,47,11,0.5,0.5*CV_PI);
	Mat inimg = imread("jobs.jpg");
	imshow("show",inimg);
	Mat gkgray,gkout;
	ConvertToGray(inimg,gkgray);
	imshow("out",gkgray);
	waitKey();
	imwrite("ooo.jpg",gkgray);
	{
		for(int j = 0; j < gkgray.cols-5; j++)
		{
			for(int i = 0; i < gkgray.rows-5; i++)
			{
				double acc = 0;
				for(int m = 0; m < 5; m++)
				{
					for(int n = 0; n < 5; n++)
					{
						acc += *((uchar*)gkgray.data + gkgray.step * (i+n) + gkgray.channels() * (j+m)) * (GKernel[m])[n]; 
					}
				}
				*((uchar*)gkout.data + gkout.step * (i + (5 - 1)/2) + (j + (5 -1)/2)) = (int)acc;
			}
		}
		for(int j = 0; j < src.cols; j++)//模板边缘用原象素填充
		{
			for(int i = src.rows - 5; i < src.rows; i++)
			{
				*((uchar*)gkout.data + gkout.step * i + j) = *((uchar*)gkgray.data + gkgray.step * i + j);
				*((uchar*)gkout.data  + gkout.step * j + i) = *((uchar*)gkgray.data + gkgray.step * j + i);
			}

			for(int i = 0; i < 5; i++)
			{
				*((uchar*)gkout.data  + gkout.step * i + j) = *((uchar*)gkgray.data + gkgray.step * i + j); 
				*((uchar*)gkout.data  + gkout.step * j + i) = *((uchar*)gkgray.data + gkgray.step * j + i);
			}
		}
		imwrite("test.jpg", gkout);
	}

	return 0;
}