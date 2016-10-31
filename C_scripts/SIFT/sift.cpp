#include "sift.h"
#include <fstream>
#include <iostream>
using namespace std;

/************************************************************************/
/*                    1.1 GaussKernel生成   	                        */
/************************************************************************/
bool GaussKernel1D(vector<double>& gausskernel, double sigma)//一维gauss模板
{
	sigma = sigma > 0 ? sigma : 0;
	int ksize = cvRound(sigma * 3) * 2 + 1;
	gausskernel.resize(ksize);

	if(ksize == 1)
	{
		gausskernel.push_back(1);
		return false;
	}
	else
	{
		double scale = -0.5/(sigma*sigma);
		const double PI = 3.141592653;
		double cons = -scale/PI;
		double sum = 0;

		for(int i = 0; i < ksize; i++)
		{
			int x = i-(ksize-1)/2;
			gausskernel[i] = cons * exp(scale * (x*x));
			sum += gausskernel[i];
		}
		
		for(int i = ksize-1; i >=0; i--)
		{
			gausskernel[i] =  gausskernel[i]/sum;//normalization
		}
		return true;
	}	
}

bool GaussKernel2D(vector<double>& gausskernel, double sigma)//二维gauss模板
{
	sigma = sigma > 0 ? sigma : 0;
	int ksize = cvRound(sigma * 3) * 2 + 1;
	gausskernel.resize(ksize);

	if(ksize == 1)
	{
		gausskernel.push_back(1);
		return false;
	}
	else
	{
		double scale = -0.5/(sigma*sigma);
		const double PI = 3.141592653;
		double cons = -scale/PI;
		double sum = 0;

		for(int i = 0; i < ksize; i++)
		{
			for(int j = 0; j < ksize; j++)
			{
				int x = i-(ksize-1)/2;
				int y = j-(ksize-1)/2;
				gausskernel[i*ksize + j] = cons * exp(scale * (x*x + y*y));
				sum += gausskernel[i*ksize+j];
			}
		}
		
		for(int i = ksize*ksize-1; i >=0; i--)
		{
			gausskernel[i] =  gausskernel[i]/sum;//normalization
		}
		return true;
	}	
}


/************************************************************************/
/*                    1.2 图像的GaussKernel滤波	                    	*/
/************************************************************************/
void GaussianTemplateSmooth(const Mat &src, Mat &dst, double sigma)//使用自建模板平滑，未使用sigma，边缘无处理
{
	//高斯模板(7*7)，sigma = 0.84089642，归一化后得到
	static const double gaussianTemplate[7][7] = 
	{
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067},
		{0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
		{0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
		{0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771},
		{0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117},
		{0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292},
		{0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067}
	};

	dst.create(src.size(), src.type());
	uchar* srcData = src.data;
	uchar* dstData = dst.data;

	for(int j = 0; j < src.cols-7; j++)
	{
		for(int i = 0; i < src.rows-7; i++)
		{
			double acc = 0;
			double accb = 0, accg = 0, accr = 0; 
			for(int m = 0; m < 7; m++)
			{
				for(int n = 0; n < 7; n++)
				{
					if(src.channels() == 1)
						acc += *(srcData + src.step * (i+n) + src.channels() * (j+m)) * gaussianTemplate[m][n];
					else
					{
						accb += *(srcData + src.step * (i+n) + src.channels() * (j+m) + 0) * gaussianTemplate[m][n];
						accg += *(srcData + src.step * (i+n) + src.channels() * (j+m) + 1) * gaussianTemplate[m][n];
						accr += *(srcData + src.step * (i+n) + src.channels() * (j+m) + 2) * gaussianTemplate[m][n];
					}
				}
			}
			if(src.channels() == 1)
				*(dstData + dst.step * (i+3) + dst.channels() * (j+3))=(int)acc;
			else
			{
				*(dstData + dst.step * (i+3) + dst.channels() * (j+3) + 0)=(int)accb;
				*(dstData + dst.step * (i+3) + dst.channels() * (j+3) + 1)=(int)accg;
				*(dstData + dst.step * (i+3) + dst.channels() * (j+3) + 2)=(int)accr;
			}
		}
	}
}

bool GaussianSmoothSingle(const Mat &src, Mat &dst, double sigma, bool isOpenCV)//只能处理单通道的图像
{
	if(isOpenCV)
	{
		GaussianBlur(src, dst, Size(0,0), sigma, sigma);//调用OpenCV的GaussianBlur
		return true;
	}
	else
	{
		if(src.channels() != 1)
			return false;

		sigma = sigma > 0 ? sigma : 0;
		int ksize = cvRound(sigma * 3) * 2 + 1;//高斯核矩阵的半径为：3*sigma
		if(ksize == 1)
		{
			src.copyTo(dst);//sigma=0时不做任何的处理
			return true;
		}
		else
		{
			dst.create(src.size(),src.type());
			vector<double> gausskernel;
			if(GaussKernel2D(gausskernel, sigma))
			{
				cout<<"the kernel is :"<<ksize<<endl;//由sigma生成的二维高斯模板大小为：
			}

			ofstream gaussout("gausskernel2D.txt");
			gaussout<<"the sigma is:"<<sigma<<"and the r is:"<<ksize<<endl;//gaussout<<"当前sigma值为："<<sigma<<"，对应的高斯模板半径为："<<ksize<<endl;
			for(int i = 0; i < ksize; i++)
			{
				for(int j = 0; j < ksize; j++)
				{
					gaussout << " " << gausskernel[i*ksize + j];
				}
				gaussout <<endl;
			}
			gaussout <<endl<<endl;
			gaussout.close();

			uchar* srcData = src.data;
			uchar* dstData = dst.data;//灰度图像

			for(int j = 0; j < src.cols-ksize; j++)
			{
				for(int i = 0; i < src.rows-ksize; i++)
				{
					double acc = 0;
					for(int m = 0; m < ksize; m++)
					{
						for(int n = 0; n < ksize; n++)
						{
							acc += *(srcData + src.step * (i+n) + src.channels() * (j+m)) * gausskernel[m*ksize+n]; 
						}
					}
					*(dstData + dst.step * (i + (ksize - 1)/2) + (j + (ksize -1)/2)) = (int)acc;
				}
			}

			for(int j = 0; j < src.cols; j++)//模板边缘用原象素填充
			{
				for(int i = src.rows - ksize; i < src.rows; i++)
				{
					*(dstData + dst.step * i + j) = *(srcData + src.step * i + j);
					*(dstData + dst.step * j + i) = *(srcData + src.step * j + i);
				}

				for(int i = 0; i < ksize; i++)
				{
					*(dstData + dst.step * i + j) = *(srcData + src.step * i + j); 
					*(dstData + dst.step * j + i) = *(srcData + src.step * j + i);
				}
			}
			return true;
		}
	}
}

bool GaussianSmoothMult(const Mat &src, Mat &dst, double sigma, bool isOpenCV)//处理多通道的图像
{
	//if(isOpenCV)
	if(true)
	{
		GaussianBlur(src, dst, Size(0,0), sigma, sigma);//调用OpenCV的GaussKernel
		return true;
	}
	else
	{
		if(src.channels() != 1 && src.channels() != 3)
			return false;
		else
		{
			sigma = sigma > 0 ? sigma : 0;
			int ksize = cvRound(sigma * 3) * 2 + 1;//高斯核矩阵的半径为：3*sigma
			if(ksize == 1)
			{
				src.copyTo(dst);//sigma=0时不做任何的处理
				return true;
			}
			else
			{
				dst.create(src.size(), src.type());
				vector<double> gausskernel;
				if(GaussKernel1D(gausskernel, sigma))//这儿使用一维实验，也可以使用二维的模板
				{
					cout << "the kernel size is:" <<ksize<<endl;//cout << "由sigma生成的一维高斯模板大小为：" <<ksize<<endl;
				}
				ofstream gaussout("gausskernel1D.txt");
				gaussout << "using the in-build GaussianKernel, the sigma is: " <<sigma<<" ,and the size is:"<<ksize<<endl;
				//gaussout << "当前sigma值为：" << sigma<<"，对应的高斯模板半径为："<<ksize<<endl;//
				for(int i = 0; i < ksize; i++)
				{
					gaussout << " " << gausskernel[i];
					gaussout <<endl;
				}
				gaussout <<endl<<endl;
				gaussout.close();
				
				Mat temp;
				temp.create(src.size(), src.type());
				double* srcData = (double*)src.data;
				double* dstData = (double*)dst.data;
				double* tempData = (double*)temp.data;

				int srcStep = src.step/sizeof(srcData[0]);
				int dstStep = src.step/sizeof(dstData[0]);
				int tempStep = src.step/sizeof(tempData[0]);
				
				//x方向一维高斯模糊
				for(int y = 0; y < src.rows; y++)
				{
					for(int x = 0; x < src.cols; x++)
					{
						double mul = 0;
						int sum = 0;
						double bmul = 0, gmul = 0, rmul = 0;
						for(int i = -ksize; i <= ksize; i++)
						{
							if((x+i) >= 0 && (x+i) < src.cols)
							{
								if(src.channels() == 1)
								{
									mul += *(srcData+y*srcStep+(x+i))*(gausskernel[ksize+i]);
								}
								else 
								{
									bmul += *(srcData+y*srcStep+(x+i)*src.channels() + 0)*(gausskernel[ksize+i]);
									gmul += *(srcData+y*srcStep+(x+i)*src.channels() + 1)*(gausskernel[ksize+i]);
									rmul += *(srcData+y*srcStep+(x+i)*src.channels() + 2)*(gausskernel[ksize+i]);
								}
								sum += gausskernel[ksize+i];
							}
						}
						if(src.channels() == 1)
						{
							*(tempData+y*tempStep+x) = mul/sum;
						}
						else
						{
							*(tempData+y*tempStep+x*temp.channels()+0) = bmul/sum;
							*(tempData+y*tempStep+x*temp.channels()+1) = gmul/sum;
							*(tempData+y*tempStep+x*temp.channels()+2) = rmul/sum;
						}
					}
				}
				
				//y方向一维高斯模糊
				for(int x = 0; x < temp.cols; x++)
				{
					for(int y = 0; y < temp.rows; y++)
					{
						double mul = 0;
						int sum = 0;
						double bmul = 0, gmul = 0, rmul = 0;
						for(int i = -ksize; i <= ksize; i++)
						{
							if((y+i) >= 0 && (y+i) < temp.rows)
							{
								if(temp.channels() == 1)
								{
									mul += *(tempData+(y+i)*tempStep+x)*(gausskernel[ksize+i]);
								}
								else
								{
									bmul += *(tempData+(y+i)*tempStep+x*temp.channels() + 0)*(gausskernel[ksize+i]);
									gmul += *(tempData+(y+i)*tempStep+x*temp.channels() + 1)*(gausskernel[ksize+i]);
									rmul += *(tempData+(y+i)*tempStep+x*temp.channels() + 2)*(gausskernel[ksize+i]);
								}
								sum += (gausskernel[ksize+i]);
							}
						}
						if(temp.channels() == 1)
						{
							*(dstData+y*dstStep+x) = mul/sum;
						}
						else
						{
							*(dstData+y*dstStep+x*dst.channels()+0) = bmul/sum;
							*(dstData+y*dstStep+x*dst.channels()+1) = gmul/sum;
							*(dstData+y*dstStep+x*dst.channels()+2) = rmul/sum;
						}
					}
				}
				return true;
			}
		}
	}
}


/************************************************************************/
/*                      2.0	GaussPyramid初始化	                        */
/************************************************************************/
void ConvertToGray(const Mat& src, Mat& dst)
{
	Size size = src.size();
	if(dst.empty())
		dst.create(size, CV_64F);

	uchar* srcData = src.data;
	double* dstData = (double*)dst.data;
	int dstStep = dst.step/sizeof(dstData[0]);

	for(int j = 0; j < src.cols; j++)
	{
		for(int i = 0; i < src.rows; i++)
		{
			double b = *(srcData + src.step * i + src.channels() * j + 0) / 255.0;
			double g = *(srcData + src.step * i + src.channels() * j + 1) / 255.0;
			double r = *(srcData + src.step * i + src.channels() * j + 2) / 255.0;

			*((dstData + dstStep * i + dst.channels() * j)) = (b + g + r)/3.0;
		}
	}
}

void UpSample(const Mat &src, Mat &dst)//升采样
{
	if(src.channels() != 1)
		return;
	dst.create(src.rows*2, src.cols*2, src.type());

	double* srcData = (double*)src.data;
	double* dstData = (double*)dst.data;

	int srcStep = src.step/sizeof(srcData[0]);
	int dstStep = dst.step/sizeof(dstData[0]);

	int m = 0, n = 0;
	for(int j = 0; j < src.cols-1; j++, n+=2)
	{
		m = 0;
		for(int i = 0; i < src.rows-1; i++, m+=2)
		{
			double sample = *(srcData + srcStep * i + src.channels() * j);
			*(dstData + dstStep * m + dst.channels() * n) = sample;
			
			double rs = *(srcData + srcStep * (i) + src.channels()*j)+(*(srcData + srcStep * (i+1) + src.channels()*j));
			*(dstData + dstStep * (m+1) + dst.channels() * n) = rs/2;
			double cs = *(srcData + srcStep * i + src.channels()*(j))+(*(srcData + srcStep * i + src.channels()*(j+1)));
			*(dstData + dstStep * m + dst.channels() * (n+1)) = cs/2;

			double center = (*(srcData + srcStep * (i+1) + src.channels() * j))
						+ (*(srcData + srcStep * i + src.channels() * j))
						+ (*(srcData + srcStep * (i+1) + src.channels() * (j+1)))
						+ (*(srcData + srcStep * i + src.channels() * (j+1)));

			*(dstData + dstStep * (m+1) + dst.channels() * (n+1)) = center/4;
		}
	}

	if(dst.rows < 3 || dst.cols < 3)
		return;

	//最后两行两列
	for(int k = dst.rows-1; k >=0; k--)
	{
		*(dstData + dstStep *(k) + dst.channels()*(dst.cols-2))=*(dstData + dstStep *(k) + dst.channels()*(dst.cols-3));
		*(dstData + dstStep *(k) + dst.channels()*(dst.cols-1))=*(dstData + dstStep *(k) + dst.channels()*(dst.cols-3));
	}
	for(int k = dst.cols-1; k >=0; k--)
	{
		*(dstData + dstStep *(dst.rows-2) + dst.channels()*(k))=*(dstData + dstStep *(dst.rows-3) + dst.channels()*(k));
		*(dstData + dstStep *(dst.rows-1) + dst.channels()*(k))=*(dstData + dstStep *(dst.rows-3) + dst.channels()*(k));
	}
}


void CreateInitSmoothGray(const Mat &src, Mat &dst, double sigma)//构建-1层初始图像，SIGMA=1.6
{
	Mat gray, up;

	ConvertToGray(src, gray);
	//imshow("gray", gray);//debug
	UpSample(gray, up);
	//imshow("up", up);//debug

	//-1层的sigma
	double  sigma_init = sqrt(sigma * sigma - (INIT_SIGMA * 2) * (INIT_SIGMA * 2));//INIT_SIGMA默认为0.5

	//GaussianSmoothMult(up, dst, sigma_init, true);
	GaussianSmoothMult(up, dst, sigma_init, false);//debug
}


/************************************************************************/
/*                      2.1	GaussPyramid生成		                        */
/************************************************************************/
double PyrAt(const vector<vector<Mat>>& pyr, int octaves, int intervals, int x, int y)
{
	double *data = (double*)(pyr[octaves])[intervals].data;
	int step = (pyr[octaves])[intervals].step/sizeof(data[0]);
	double val = *(data+y*step+x);
	return val;
}//计算当前的数据
#define At(dog_pyr, octaves, intervals, x, y) (PyrAt(dog_pyr, (octaves), (intervals), (x), (y)))//取出当前位置处的图像信息

void DownSample(const Mat& src, Mat& dst)//降采样
{
	if(src.channels() != 1)
		return;

	if(src.cols <=1 || src.rows <=1)
	{
		src.copyTo(dst);
		return;
	}

	dst.create((int)(src.rows/2), (int)(src.cols/2), src.type());//减半复制

	double* srcData = (double*)src.data;
	double* dstData = (double*)dst.data;

	int srcStep = src.step/sizeof(srcData[0]);
	int dstStep = dst.step/sizeof(dstData[0]);//多通道复制

	int m = 0, n = 0;
	for(int j = 0; j < src.cols; j+=2, n++)
	{
		m = 0;
		for(int i = 0; i < src.rows; i+=2, m++)
		{
			double sample = *(srcData + srcStep * i + src.channels() * j);
		
			if(m < dst.rows && n < dst.cols)//防止当图像长宽不一致时，长宽为奇数时，m,n越界
			{
				*(dstData + dstStep * m + dst.channels() * n) = sample;
			}
		}
	}
}

void GaussPyramid(const Mat& src, vector<vector<Mat>>& gausspyriads, int octaves , int intervals, double sigma)
{
	vector<double> interval_sigma;
	//interval_sigma.resize(intervals+3);
	interval_sigma.push_back(sigma);
	
	double k = pow(2.0, 1.0/intervals);//k系数
	double sig_prev, sig_total;
	for(int i = 1; i < intervals + 3; i++ )
	{
		sig_prev = pow( k, i - 1 ) * sigma;
		sig_total = sig_prev * k;
		interval_sigma.push_back(sqrt( sig_total * sig_total - sig_prev * sig_prev ));//得到不同层的sigma数值
	}
	
	gausspyriads.resize(octaves);
	for(int o = 0; o < octaves; o++)//建立金字塔
	{
		for(int i = 0; i < intervals+3; i++)
		{
			Mat mat;
			if(o == 0 && i == 0)
			{
				//-1层保持原图像不变，也不高斯模糊
				src.copyTo(mat);
			}
			else if(i == 0)
			{
				//下一组第一张图的图像是上一组下标倒数第3的图像降采样得来
				DownSample((gausspyriads[o-1])[intervals], mat);//每一组中下标为:0~(interval+2),故倒数第三个下标就是intervals
			}
			else
			{
				//每组中下一层由上一层高斯模糊得到
				GaussianSmoothMult((gausspyriads[o])[i-1], mat, interval_sigma[i],true);
				//GaussianSmoothMult((gausspyriads[o])[i-1], mat, interval_sigma[i],false);//debug	溢出错误 error
			}
			gausspyriads[o].push_back(mat);
		}
	}
}


/************************************************************************/
/*                      2.2 建立DOG_Pyramid	                            */
/************************************************************************/
void Sub(const Mat& a, const Mat& b, Mat & c)//图像相减
{
	if(a.rows != b.rows || a.cols != b.cols || a.type() != b.type())
		return;
	if(!c.empty())
		return;
	c.create(a.size(), a.type());

	double* ap = (double*)a.data;
	double* ap_end = (double*)a.dataend;
	double* bp = (double*)b.data;
	double* cp = (double*)c.data;
	int step = a.step/sizeof(double);

	while(ap != ap_end)
	{
		*cp++ = *ap++ - *bp++;
	}
}

void DogPyramid(const vector<vector<Mat>>& gauss_pyr, vector<vector<Mat>>& dog_pyr, int octaves, int intervals)
{
	dog_pyr.resize(octaves);//必须初始化才行，因为是二维的
	for(int o = 0; o < octaves; o ++)
	{
		for(int i = 1; i < intervals+3; i++)
		{
			Mat mat;
			Sub((gauss_pyr[o])[i], (gauss_pyr[o])[i-1], mat);
			dog_pyr[o].push_back(mat);
		}
	}
}

/************************************************************************/
/*                      3.1 空间极值点检测		                        */
/************************************************************************/
bool isExtremum(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y)
{
	//double * data = (double *)(dog_pyr[octaves]).[intervals].data;
	//int step = (dog_pyr[octaves]).[intervals].step/sizeof(data[0]);
	//double val = *(data+y*step+x);//当前的数据
	
	double val = At(dog_pyr, octaves, intervals, x, y);

	if(val > 0)//检查最大极值
	{
		for(int i = -1; i <= 1; i++)
		{
			//int stp = (dog_pyr[octaves]).[intervals+i].step/sizeof(data[0]);
			for(int j = -1; j <= 1; j++)//3*3范围内查找
			{
				for(int k = -1; k <= 1; k++)
				{
					//if( val < *((double*)(dog_pyr[octaves]).[intervals+i].data+stp*(y+j)+(x+k)))
					if( val < At(dog_pyr, octaves, intervals+i, x+k, y+j))
					{
						return false;//只要在范围内不是最大就被放弃
					}
				}
			}
		}
	}
	else//检查最小极值
	{
		for(int i = -1; i <= 1; i++)
		{
			//int stp = (dog_pyr[octaves]).[intervals+i].step/sizeof(double);
			for(int j = -1; j <= 1; j++)
			{
				for(int k = -1; k <= 1; k++)
				{
					//if( val > *((double*)(dog_pyr[octaves]).[intervals+i].data+stp*(y+j)+(x+k)))
					if( val < At(dog_pyr, octaves, intervals+i, x+k, y+j))
					{
						return false;//只要在范围内不是最小就被放弃
					}
				}
			}
		}
	}
	return true;
}

/************************************************************************/
/*                      3.2 Hessian矩阵以及相关计算                     */
/************************************************************************/
void DerivativeOf3D(const vector<vector<Mat>>& dog_pyr, int octave, int interval, int x, int y, double *dx)//一阶导数矩阵
{
	double Dx = (At(dog_pyr, octave, interval, x+1, y)-At(dog_pyr, octave, interval, x-1, y))/2.0;
	double Dy = (At(dog_pyr, octave, interval, x, y+1)-At(dog_pyr, octave, interval, x, y-1))/2.0;
	double Ds = (At(dog_pyr, octave, interval+1, x, y)-At(dog_pyr, octave, interval-1, x, y))/2.0;

	dx[0] = Dx;
	dx[1] = Dy;
	dx[2] = Ds;
}

#define Hat(i, j) (*(H+(i)*3 + (j)))
void Hessian3D(const vector<vector<Mat>>& dog_pyr, int octave, int interval, int x, int y, double *H)//3*3阶Hessian矩阵
{
	double val, Dxx, Dyy, Dss, Dxy, Dxs, Dys;
	
	val = At(dog_pyr, octave, interval, x, y);

	Dxx = At(dog_pyr, octave, interval, x+1, y) + At(dog_pyr, octave, interval, x-1, y) - 2*val;
	Dyy = At(dog_pyr, octave, interval, x, y+1) + At(dog_pyr, octave, interval, x, y-1) - 2*val;
	Dss = At(dog_pyr, octave, interval+1, x, y) + At(dog_pyr, octave, interval-1, x, y) - 2*val;

	Dxy = (At(dog_pyr, octave, interval, x+1, y+1) + At(dog_pyr, octave, interval, x-1, y-1)
		 - At(dog_pyr, octave, interval, x+1, y-1) - At(dog_pyr, octave, interval, x-1, y+1))/4.0;

	Dxs = (At(dog_pyr, octave, interval+1, x+1, y) + At(dog_pyr, octave, interval-1, x-1, y)
		 - At(dog_pyr, octave, interval-1, x+1, y) - At(dog_pyr, octave, interval+1, x-1, y))/4.0;

	Dys = (At(dog_pyr, octave, interval+1, x, y+1) + At(dog_pyr, octave, interval-1, x, y-1)
		 - At(dog_pyr, octave, interval+1, x, y-1) - At(dog_pyr, octave, interval-1, x, y+1))/4.0;

	Hat(0, 0) = Dxx;
	Hat(1, 1) = Dyy;
	Hat(2, 2) = Dss;

	Hat(1, 0) = Hat(0, 1) = Dxy;
	Hat(2, 0) = Hat(0 ,2) = Dxs;
	Hat(2, 1) = Hat(1, 2) = Dys;
}

#define HIat(i, j) (*(H_inve+(i)*3 + (j)))
bool Inverse3D(const double *H, double *H_inve)//3*3阶Hessian矩阵求逆
{
	double A = Hat(0, 0)*Hat(1, 1)*Hat(2, 2) 
			 + Hat(0, 1)*Hat(1, 2)*Hat(2, 0)
			 + Hat(0, 2)*Hat(1, 0)*Hat(2, 1)
			 - Hat(0, 0)*Hat(1, 2)*Hat(2, 1)
			 - Hat(0, 1)*Hat(1, 0)*Hat(2, 2)
			 - Hat(0, 2)*Hat(1, 1)*Hat(2, 0);
	
	if(fabs(A) < 1e-10)//没有逆矩阵
	{
		return false;
	}
	else
	{
		HIat(0, 0) = Hat(1, 1) * Hat(2, 2) - Hat(2, 1)*Hat(1, 2);
		HIat(0, 1) = -(Hat(0, 1) * Hat(2, 2) - Hat(2, 1) * Hat(0, 2));
		HIat(0, 2) = Hat(0, 1) * Hat(1, 2) - Hat(0, 2)*Hat(1, 1);
	
		HIat(1, 0) = Hat(1, 2) * Hat(2, 0) - Hat(2, 2)*Hat(1, 0);
		HIat(1, 1) = -(Hat(0, 2) * Hat(2, 0) - Hat(0, 0) * Hat(2, 2));
		HIat(1, 2) = Hat(0, 2) * Hat(1, 0) - Hat(0, 0)*Hat(1, 2);
	
		HIat(2, 0) = Hat(1, 0) * Hat(2, 1) - Hat(1, 1)*Hat(2, 0);
		HIat(2, 1) = -(Hat(0, 0) * Hat(2, 1) - Hat(0, 1) * Hat(2, 0));
		HIat(2, 2) = Hat(0, 0) * Hat(1, 1) - Hat(0, 1)*Hat(1, 0);

		for(int i = 0; i < 9; i++)
		{
			*(H_inve+i) /= A;
		}
		return true;
	}
}

//计算x^
void GetOffsetX(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double *offset_x)
{
	//x^ = -H^(-1) * dx;
	//dx = (Dx, Dy, Ds)^T
	
	double H[9], H_inve[9]= {0};
	Hessian3D(dog_pyr, octaves, intervals, x, y, H);
	Inverse3D(H, H_inve);
	double dx[3];
	DerivativeOf3D(dog_pyr, octaves, intervals, x, y, dx);
	
	for(int i = 0; i < 3; i ++)
	{
		offset_x[i] = 0.0;
		for(int j = 0; j < 3; j++)
		{
			offset_x[i] += H_inve[i*3 + j] * dx[j];
		}
		offset_x[i] = -offset_x[i];
	}
}

//计算|D(x^)|
double GetFabsDx(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, const double* offset_x)
{
	//|D(x^)|=D + 0.5 * dx * offset_x; dx=(Dx, Dy, Ds)^T
	double dx[3];
	DerivativeOf3D(dog_pyr, octaves, intervals, x, y, dx);

	double term = 0.0;
	for(int i = 0; i < 3; i++)
		term += dx[i] * offset_x[i];
	double val = At(dog_pyr, octaves, intervals, x, y);
	return fabs(val + 0.5 * term);
}


/************************************************************************/
/*                      3.3 修正极值点，删除不稳定点                    */
/************************************************************************/
Keypoint* InterploationExtremum(const vector<vector<Mat>>& dog_pyr, int octave, int interval, int x, int y, double dxthreshold)
{
	//计算x=(x,y,sigma)^T
	//x^ = -H^(-1) * dx; 
	//dx = (Dx, Dy, Ds)^T
	
	double offset_x[3]={0};
	const Mat &mat = (dog_pyr[octave])[interval];//当前的图像
	int idx = octave;
	int intvl = interval;
	int i = 0; 
	while(i < MAX_INTERPOLATION_STEPS)//MAX_INTERPOLATION_STEPS=5 迭代次数
	{
		GetOffsetX(dog_pyr, octave, interval, x, y, offset_x);

		//如果offset_x 的任一维度大于0.5，it means that the extremum lies closer to a different sample point.
		if(fabs(offset_x[0]) < 0.5 && fabs(offset_x[1]) < 0.5 && fabs(offset_x[2]) < 0.5)
			break;

		//用周围的点代替
		//
		x += cvRound(offset_x[0]);
		y += cvRound(offset_x[1]);
		interval += cvRound(offset_x[2]);

		idx = octave, interval - intvl + interval;
		//idx = octave*(INTERVALS+2)+interval;//默认INTERVALS = 3

		if( interval < 1 || interval > INTERVALS ||
			x >= mat.cols-1 || x < 2 ||
			y >= mat.rows-1 || y < 2)  //此处保证检测边时 x+1,y+1和x-1, y-1有效
		{
			return NULL;
		}
		i++;
	}

	//迭代计算次数溢出
	if(i >= MAX_INTERPOLATION_STEPS)
		return NULL;

	//rejecting unstable extrema
	//|D(x^)| < 0.03取经验值
	if(GetFabsDx(dog_pyr, octave, interval, x, y, offset_x) < dxthreshold/INTERVALS)
	{
		return NULL;
	}

	Keypoint *keypoint = new Keypoint;

	keypoint->x = x;
	keypoint->y = y;

	keypoint->offset_x = offset_x[0];
	keypoint->offset_y = offset_x[1];

	keypoint->interval = interval;
	keypoint->offset_interval = offset_x[2];

	keypoint->octave = octave;

	keypoint->dx = (x + offset_x[0])*pow(2.0, octave);
	keypoint->dy = (y + offset_x[1])*pow(2.0, octave);

	return keypoint;
}


/************************************************************************/
/*                 3.4 使用hessian矩阵消除边缘响应点			        */
/************************************************************************/
#define DAt(x, y) (*(data+(y)*step+(x)))
//#define DAt(x, y) At(dog_pyr, (octaves), (intervals), (x), (y))
bool passEdgeResponse(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double r)//默认RATIO = 10
{
	double *data = (double *)(dog_pyr[octaves])[intervals].data;
	int step = (dog_pyr[octaves])[intervals].step/sizeof(data[0]);
	double val = *(data+y*step+x);

	double Dxx, Dyy, Dxy;
	double Tr_h, Det_h;
	
	//hessian矩阵
	//	   _ 	    _
	//    | Dxx  Dxy |
	// H =|			 |
	//	  |_Dxy  Dyy_|	
	//	  
	Dxx = DAt(x+1, y) + DAt(x-1, y) - 2*val;
	Dyy = DAt(x, y+1) + DAt(x, y-1) - 2*val;
	Dxy = (DAt(x+1, y+1) + DAt(x-1,y-1) - DAt(x-1, y+1) - DAt(x+1, y-1))/4.0;

	Tr_h = Dxx + Dyy;
	Det_h = Dxx * Dyy - Dxy * Dxy;

	if(Det_h <=0)
		return false;
	
	if(Tr_h * Tr_h / Det_h < (r + 1) * (r + 1) / r)
		return true;

	return false;
}


/************************************************************************/
/*                      3.5 查找并确认极值点	                        */
/************************************************************************/
void DetectionLocalExtrema(const vector<vector<Mat>>& dog_pyr, vector<Keypoint>& extrema, int octaves, int intervals)
{
	long int totla_num=0,first_num=0,second_extream=0,third_unstable=0,fourth_unedage=0;

	double thresh = 0.5 * DXTHRESHOLD / intervals;
	for(int o = 0; o < octaves; o++)
	{
		for(int i = 1; i < intervals + 1; i++)//在DOG的中间层寻找
		{
			double *data = (double *)(dog_pyr[o])[i].data;//数据起始点
			int step = (dog_pyr[o])[i].step/sizeof(data[0]);
			
			for(int y = 5; y < (dog_pyr[o])[i].rows-5; y++)//这儿为什么是5？
			{
				for(int x = 5; x < (dog_pyr[o])[i].cols-5; x++)
				{
					totla_num++;//统计遍历总点数
					double val = *(data+y*step+x);
					if( fabs(val) > thresh ) //排除阈值过小的点
					{
						first_num++;//统计需要计算极值的点数
						if(isExtremum(dog_pyr, o, i, x, y))
						{
							second_extream++;//统计极值点数
							Keypoint *extrmum = InterploationExtremum(dog_pyr, o, i, x, y);//极值点修正
							if(extrmum)
							{
								third_unstable++;//统计精确极值点数
								if(passEdgeResponse(dog_pyr, o, i, extrmum->x, extrmum->y))
								{	
									extrmum->val = *(data+extrmum->y*step+extrmum->x);
									extrema.push_back(*extrmum);
									fourth_unedage++;//统计去除强边缘响应点数
								}
								delete extrmum;
	
							}
						}
					}
				}
			}
		
		}
	}
	//cout << "总共需要检测的点数："<<totla_num<<endl;//debug
	//cout << "排除阈值过小的点后有："<<first_num<<endl;//debug
	//cout << "经过检测确认是空间极值点有："<<second_extream<<endl;//debug
	//cout << "统计精确极值点数有："<<third_unstable<<endl;//debug
	//cout << "去除强边缘响应点后有："<<fourth_unedage<<endl;//debug
	cout << totla_num<<endl;//debug
	cout << first_num<<endl;//debug
	cout << second_extream<<endl;//debug
	cout << third_unstable<<endl;//debug
	cout << fourth_unedage<<endl;//debug
}


/************************************************************************/
/*         3.6 关键点定位精确计算——计算精确定位点的尺度信息             */
/************************************************************************/
void CalculateScale(vector<Keypoint>& features, double sigma, int intervals)
{
	//由层数和组数计算关键点的尺度信息
	double intvl = 0;
	for(int i = 0; i < features.size(); i++)
	{
		intvl = features[i].interval + features[i].offset_interval;//offset_interval是插值后的偏移
		features[i].scale = sigma * pow(2.0, features[i].octave + intvl/intervals);
		features[i].octave_scale = sigma * pow(2.0, intvl/intervals);//组内尺度
	}
}

void HalfFeatures(vector<Keypoint>& features)//由于计算Hessian矩阵导数的时候用了Taylor级数，有2倍关系
{
	for(int i = 0; i < features.size(); i++)
	{
		features[i].dx /= 2;
		features[i].dy /= 2;
		features[i].scale /= 2;
	}
}


/************************************************************************/
/*        	4.1 DOG中检测出的关键点计算模值和方向					    */
/************************************************************************/
//计算模值和方向：计算在gauss图像中坐标为xy处的模值和方向
bool CalcGradMagOri(const Mat& gauss, int x, int y, double& mag, double& ori)
{
	if(x > 0 && x < gauss.cols-1 && y > 0 && y < gauss.rows -1)
	{
		double *data = (double*)gauss.data;
		int step = gauss.step/sizeof(*data);

		double dx = *(data+step*y+(x+1))-(*(data+step*y+(x-1)));
		double dy = *(data+step*(y+1)+x)-(*(data+step*(y-1)+x));

		mag = sqrt(dx*dx + dy*dy);
		ori = atan2(dy, dx);//atan2返回[-Pi, -Pi]的弧度值
		return true;
	}
	else
		return false;
}


/************************************************************************/
/*        		 4.2 DOG中检测关键点点计算直方图					    */
/************************************************************************/
void CalculateOrientationHistogram(const Mat& gauss, int x, int y, int bins, int radius, double sigma, vector<double>& hist)
{	//radius就是3sigma窗口

	hist.assign(bins,0.0);

	double mag, ori;
	
	double weight;

	int bin;
	const double PI2 = 2.0*CV_PI;

	double econs = -1.0/(2.0*sigma*sigma);

	for(int i = -radius; i <= radius; i++)
	{
		for(int j = -radius; j <= radius; j++)//在取得的区域中统计直方图
		{
			if(CalcGradMagOri(gauss, x+i, y+j, mag, ori))//获得位置和方向
			{
				weight = exp((i*i+j*j)*econs);//计算权重
			
				//使用Pi-ori将ori转换到[0,2*PI]之间
				bin = cvRound(bins * (CV_PI - ori)/PI2);
				bin = bin < bins ? bin : 0;

				hist[bin] += mag * weight;
			}
		}
	}
}


/************************************************************************/
/*        		 		4.3 直方图的极值查找	    				    */
/************************************************************************/
double DominantDirection(vector<double>& hist, int n)
{
	double maxd = hist[0];
	for(int i = 1; i < n; i++)
	{
		if(hist[i] > maxd)
			maxd = hist[i];
	}
	return maxd;
}

//高斯平滑，模板为{0.25, 0.5, 0.25}
void GaussSmoothOriHist(vector<double>& hist, int n)
{
	double prev = hist[n-1], temp, h0=hist[0];


	for(int i = 0; i < n; i++)
	{
		temp = hist[i];
		hist[i] = 0.25 * prev + 0.5 * hist[i] + 
			0.25 * (i+1 >= n ? h0 : hist[i+1]);
		prev = temp;
	}
}


/************************************************************************/
/*        		 		4.4 辅助特征点的查找	    				    */
/************************************************************************/
#define Parabola_Interpolate(l, c, r) (0.5*((l)-(r))/((l)-2.0*(c)+(r)))//抛物插值
void CalcOriFeatures(const Keypoint& keypoint, vector<Keypoint>& features, const vector<double>&hist, int n, double mag_thr)
{
	double bin, PI2 = CV_PI * 2.0;
	int l, r;
	for(int i = 0; i < n; i++)
	{
		l = (i == 0) ? n-1 : i -1;
		r = (i+1)%n;//循环查找，l是前一个点，r是后一个点

		//hist[i]是极值
		//对于直方图的每个bin，如果它比前一个bin和后一个bin都大且超过一个阈值：主峰80%能量以上
		//将这个点作为辅助点，并且插值处理获取更精确的值
		//操作：复制当前的feat，将这个bin对应的方向值跟feat做关联，然后压入features，作为新的匹配点
		if(hist[i] > hist[l] && hist[i] > hist[r] && hist[i] >= mag_thr)
		{
			bin = i + Parabola_Interpolate(hist[l], hist[i], hist[r]);//插值拟合，求取精度更好的结果
			bin = (bin < 0) ? (bin + n) : (bin >=n ? (bin -n) : bin);//
			Keypoint new_key;

			{
				new_key.dx = keypoint.dx;
				new_key.dy = keypoint.dy;

				new_key.interval = keypoint.interval;
				new_key.octave = keypoint.octave;
				new_key.octave_scale = keypoint.octave_scale;
				new_key.offset_interval = keypoint.offset_interval;

				new_key.offset_x = keypoint.offset_x;
				new_key.offset_y = keypoint.offset_y;
	
				new_key.ori = ((PI2 * bin)/n) - CV_PI;//重新设定插值求解后的精确方向
				new_key.scale = keypoint.scale;
				new_key.val = keypoint.val;
				new_key.x = keypoint.x;
				new_key.y = keypoint.y;
			}//copy to the new_key
						
			features.push_back(new_key);
		}
	}
}


/************************************************************************/
/*        		 			4 关键点方向分配	    				    */
/************************************************************************/
void OrientationAssignment(vector<Keypoint>& extrema, vector<Keypoint>& features, const vector<vector<Mat>>& gauss_pyr)
{
	int n = extrema.size();
	vector<double> hist;

	for(int i = 0; i < n; i++)
	{
		//对于在DOG金字塔中检测出的关键点点，采集其所在高斯金字塔图像3σ邻域窗口内像素的梯度和方向分布特征
		//梯度的模值m(x,y)按：1.5*sigma 的高斯分布加成，即：ORI_SIGMA_TIMES*extrema[i].octave_scale
		//ORI_HIST_BINS=36 //梯度直方图将0~360度的方向范围分为36个柱(bins)，其中每柱10度
		//ORI_SIGMA_TIME = 1.5
		//ORI_WINDOW_RADIUS = 3.0*ORI_SIGMA_TIME
		CalculateOrientationHistogram((gauss_pyr[extrema[i].octave])[extrema[i].interval],
			extrema[i].x, extrema[i].y, ORI_HIST_BINS, cvRound(ORI_WINDOW_RADIUS*extrema[i].octave_scale), 
			ORI_SIGMA_TIMES*extrema[i].octave_scale, hist);//返回的是36大小的数组指针
			
							
		for(int j = 0; j < ORI_SMOOTH_TIMES; j++)
			GaussSmoothOriHist(hist, ORI_HIST_BINS);
			//随着距中心点越远的领域其对直方图的贡献也响应减小.Lowe论文中还提到要使用高斯函数对直方图进行平滑，减少突变的影响。
			//由于SIFT算法只考虑了尺度和旋转不变性，并没有考虑仿射不变性。
			//通过高斯加权，使特征点附近的梯度幅值有较大的权重，这样可以部分弥补因没有仿射不变性而产生的特征点不稳定的问题。
			
		double highest_peak = DominantDirection(hist, ORI_HIST_BINS);//直方图的峰值代表该特征点处邻域内图像梯度的主方向，也即该特征点的主方向
		
		CalcOriFeatures(extrema[i], features, hist, ORI_HIST_BINS, highest_peak*ORI_PEAK_RATIO);
		//ORI_PEAK_RATIO=0.8
		//当存在一个相当于主峰值能量80%能量的峰值时，则将这个方向认为是该特征点的辅方向。
	}
}


/************************************************************************/
/*        		 5.1 插值计算每个种子点八个方向的梯度  				    */
/************************************************************************/
void InterpHistEntry(double ***hist, double xbin, double ybin, double obin, double mag, int bins, int d)
{
	double d_r, d_c, d_o, v_r, v_c, v_o;
	double** row, * h;
	int r0, c0, o0, rb, cb, ob, r, c, o;

	r0 = cvFloor( ybin );
	c0 = cvFloor( xbin );
	o0 = cvFloor( obin );
	d_r = ybin - r0;
	d_c = xbin - c0;
	d_o = obin - o0;

	/*
		做插值：
		xbin,ybin,obin:种子点所在子窗口的位置和方向
		所有种子点都将落在4*4的窗口中
		r0,c0取不大于xbin，ybin的正整数
		r0,c0只能取到0,1,2
		xbin,ybin在(-1, 2)

		r0取不大于xbin的正整数时。
		r0+0 <= xbin <= r0+1
		mag在区间[r0,r1]上做插值

		obin同理
	*/

	for( r = 0; r <= 1; r++ )
	{
		rb = r0 + r;
		if( rb >= 0  &&  rb < d )
		{
			v_r = mag * ( ( r == 0 )? 1.0 - d_r : d_r );
			row = hist[rb];
			for( c = 0; c <= 1; c++ )
			{
				cb = c0 + c;
				if( cb >= 0  &&  cb < d )
				{
					v_c = v_r * ( ( c == 0 )? 1.0 - d_c : d_c );
					h = row[cb];
					for( o = 0; o <= 1; o++ )
					{
						ob = ( o0 + o ) % bins;
						v_o = v_c * ( ( o == 0 )? 1.0 - d_o : d_o );
						h[ob] += v_o;
					}
				}
			}
		}
	}
}//好吧，这段代码没有仔细看。。。

/************************************************************************/
/*      5.2 特征向量形成后，去除光照变化的影响，进行归一化处理  		*/
/************************************************************************/
void NormalizeDescr(Keypoint& feat)//归一化处理
{
	double cur, len_inv, len_sq = 0.0;
	int i, d = feat.descr_length;

	for( i = 0; i < d; i++ )
	{
		cur = feat.descriptor[i];
		len_sq += cur*cur;
	}
	len_inv = 1.0 / sqrt( len_sq );
	for( i = 0; i < d; i++ )
		feat.descriptor[i] *= len_inv;
}

void HistToDescriptor(double ***hist, int width, int bins, Keypoint& feature)
{
	int int_val, i, r, c, o, k = 0;

	for( r = 0; r < width; r++ )
		for( c = 0; c < width; c++ )
			for( o = 0; o < bins; o++ )
			{
				feature.descriptor[k++] = hist[r][c][o];//转换为一维向量了
			}

	feature.descr_length = k;//共有k个特征向量
	NormalizeDescr(feature);//特征矢量形成后，为了去除光照变化的影响，需要对它们进行归一化处理。
	for( i = 0; i < k; i++ )
		if( feature.descriptor[i] > DESCR_MAG_THR )//在归一化处理后，对于特征矢量中值大于0.2的要进行截断，即大于0.2的值只取0.2
			feature.descriptor[i] = DESCR_MAG_THR;
	NormalizeDescr(feature);//然后，再进行一次归一化处理，其目的是提高特征的鉴别性。
	
	
	/* convert floating-point descriptor to integer valued descriptor */
	for( i = 0; i < k; i++ )
	{
		int_val = INT_DESCR_FCTR * feature.descriptor[i];//INT_DESCR_FCTR=512.0
		feature.descriptor[i] = min( 255, int_val );//不能超过255
	}
}


/************************************************************************/
/*5.3 对关键点周围图像区域分块，计算块内梯度直方图，生成具有独特性的向量*/
/* 	(1)确定描述子直方图的邻域的半径;									*/
/*	(2)将坐标轴旋转到关键点方向;										*/
/*	(3)将邻域内的采样点分配到对应的子区域内，							*/
/*	   将子区域内的梯度值分配到8个方向上，计算其权值。				    */
/************************************************************************/
double*** CalculateDescrHist(const Mat& gauss, int x, int y, double octave_scale, double ori, int bins, int width)
{
	double ***hist = new double**[width];

	//申请空间并初始化
	for(int i = 0; i < width; i++)
	{
		hist[i] = new double*[width];
		for(int j = 0; j < width; j++)
		{
			hist[i][j] = new double[bins];
		}
	}
	
	for(int r = 0; r < width; r++ )
		for(int c = 0; c < width; c++ )
			for(int o = 0; o < bins; o++ )
				hist[r][c][o]=0.0;


	double cos_ori = cos(ori);
	double sin_ori = sin(ori);
	
	double sigma = 0.5 * width;//Lowe建议子区域的像素的梯度大小按子窗口宽度的一半进行高斯加权计算

	double conste = -1.0/(2*sigma*sigma);
	double PI2 = CV_PI * 2;
	double sub_hist_width = DESCR_SCALE_ADJUST * octave_scale;//DESCR_SCALE_ADJUST=3，即3sigma半径

	//实际计算领域半径时，需要采用双线性插值，所需图像窗口边长为3*sigma*(d+1)
	//在考虑到旋转因素(方便下一步将坐标轴旋转到关键点的方向)，半径变为:r*(sqrt(2.0)/2);
	//实际计算所需的图像区域半径为：(3sigma*(d+1)*pow(2,0.5))/2
	int radius = (sub_hist_width*sqrt(2.0)*(width+1))/2.0 + 0.5; //+0.5取四舍五入

	double grad_ori, grad_mag;
	for(int i = -radius; i <= radius; i++)
	{
		for(int j =-radius; j <= radius; j++)
		{
			//将坐标轴旋转为关键点的方向，以确保旋转不变性
			double rot_x = (cos_ori * j - sin_ori * i) / sub_hist_width;
			double rot_y = (sin_ori * j + cos_ori * i) / sub_hist_width;


			//xbin,ybin为落在4*4窗口中的下标值
			double xbin = rot_x + width/2 - 0.5;
			double ybin = rot_y + width/2 - 0.5;

			//
			if(xbin > -1.0 && xbin < width && ybin > -1.0 && ybin < width)
			{
				if(CalcGradMagOri(gauss, x+j, y + i, grad_mag, grad_ori))//计算gauss中位于(x+j, y + i)处的权重
				{
					grad_ori = (CV_PI-grad_ori) - ori;
					while(grad_ori < 0.0)
						grad_ori += PI2;
					while(grad_ori >= PI2)
						grad_ori -= PI2;

					double obin = grad_ori * (bins/PI2);

					double weight = exp(conste*(rot_x*rot_x + rot_y * rot_y));

					InterpHistEntry(hist, xbin, ybin, obin, grad_mag*weight, bins, width);//插值计算每个种子点八个方向的梯度
				}
			}
		}
	}
	return hist;
}


/************************************************************************/
/*        		 			5 特征向量的生成	    				    */
/************************************************************************/
//特征描述子与特征点所在的尺度有关，因此，对梯度的求取应在特征点对应的高斯图像上进行
//将关键点附近的邻域划分为d*d(Lowe建议d=4)个子区域，每个子区域做为一个种子点，每个种子点有8个方向
//bins表示8个方向，width表示子区域的半径（默认为4）
//每个子区域的大小与关键点方向分配时的半径取值相同，即对应的sigma相同
void DescriptorRepresentation(vector<Keypoint>& features, const vector<vector<Mat>>& gauss_pyr, int bins, int width)
{
	double ***hist;

	for(int i = 0; i < features.size(); i++)
	{
		hist = CalculateDescrHist((gauss_pyr[features[i].octave])[features[i].interval],
			features[i].x, features[i].y, features[i].octave_scale, features[i].ori, bins, width);
			
		HistToDescriptor(hist, width, bins, features[i]);

		for(int j = 0; j < width; j++)
		{
			
			for(int k = 0; k < width; k++)
			{
				delete[] hist[j][k];
			}
			delete[] hist[j];
		}
		delete[] hist;
	}
}


/************************************************************************/
/*        		 			6 外部调用函数		    				    */
/************************************************************************/
void Sift(const Mat &src, vector<Keypoint>& features, double sigma, int intervals)
{
	Mat init_gray;
	CreateInitSmoothGray(src, init_gray, sigma);//已经将原始图像扩大二倍
	int octaves = log((double)min(init_gray.rows, init_gray.cols))/log(2.0) - 2;
	cout << "rows="<<init_gray.rows<< " cols="<<init_gray.cols<<" octaves="<<octaves<<endl;

	
	cout <<"building gaussian pyramid ..."<<endl;
	vector<vector<Mat>> gauss_pyr;
	GaussPyramid(init_gray, gauss_pyr, octaves , intervals, sigma);//传入整理好的图像，并且模糊
	write_pyr(gauss_pyr, "GaussPyramid");//将建立的GaussPyramid保存
	
	
	cout <<"building difference of gaussian pyramid..."<<endl;
	vector<vector<Mat>> dog_pyr;
	DogPyramid(gauss_pyr, dog_pyr, octaves, intervals);//需要计算每一组中不同层的sigma
	write_pyr(dog_pyr, "DOGPyramid");//将建立的DOG保存

	
	cout << "detecting the local extrema..."<<endl;
	vector<Keypoint> extrema;
	DetectionLocalExtrema(dog_pyr, extrema, octaves, intervals);//在DOG的上下三层中，寻找空间极值点
	//cout << "extrema detection finished."<<endl<<"--please look dir gausspyramid, dogpyramid and extrema.txt.--"<<endl;
	
	
	////这两个函数主要为了确定精确定位点的尺度信息
	CalculateScale(extrema, sigma, intervals);
	HalfFeatures(extrema);

	cout << "orientation assignment..."<<endl;
	OrientationAssignment(extrema, features, gauss_pyr);//返回到高斯空间中统计
	cout << "--features count: "<<features.size()<<" --" <<endl;
	
	DescriptorRepresentation(features, gauss_pyr, DESCR_HIST_BINS, DESCR_WINDOW_WIDTH);

	sort(features.begin(), features.end(), FeatureCmp);

	cout << "finished."<<endl;
}

void write_features(const vector<Keypoint>&features, const char*file)
{
	ofstream dout(file);
	dout <<"keypoints:"<<features.size()<<""<<"      "<<
		"the demain is:"<<FEATURE_ELEMENT_LENGTH<<endl<<endl;
// 	dout <<"检测到的特征点总共有："<<features.size()<<"个"<<"      "<<
// 	"每一个特征点的向量维度是："<<FEATURE_ELEMENT_LENGTH<<endl<<endl;
	for(int i = 0; i < features.size(); i++)
	{
		dout<<"(x,y):"<<"("<<features[i].dx<<", "<<features[i].dy<<")"<<endl;//dout<<"关键点坐标(x,y):"<<"("features[i].dx<<", "<<features[i].dy<<")"<<endl;
		dout<<"scale:"<<features[i].scale<<endl<<"orientation:"<<features[i].ori<<endl;//dout<<"尺度(scale):"<<features[i].scale<<endl<<"关键点的梯度方向(orientation):"<<features[i].ori<<endl;
		dout<<"vector is:"<<endl;//dout<<"16个种子点的8个方向向量的信息共128个信息:"<<endl;
		for(int j = 0; j < FEATURE_ELEMENT_LENGTH; j++)
		{
			if(j % 20 == 0)
				dout<<endl;
			dout << features[i].descriptor[j]<<" "; 
		}
		dout<<endl<<endl<<endl;
	}
	dout.close();
}

bool FeatureCmp(Keypoint& f1, Keypoint& f2)
{
	return f1.scale < f2.scale;
}

void DrawKeyPoints(Mat &src, vector<Keypoint>& keypoints)
{
	int j = 0;
	for(int i = 0; i < keypoints.size(); i++)
	{
		CvScalar color = CvScalar(255,0,0);
		circle(src, Point(keypoints[i].dx, keypoints[i].dy), 3, color);
		j++;
	}
}

void DrawSiftFeature(Mat& src, Keypoint& feat, CvScalar color)
{
	int len, hlen, blen, start_x, start_y, end_x, end_y, h1_x, h1_y, h2_x, h2_y;
	double scl, ori;
	double scale = 5.0;
	double hscale = 0.75;
	CvPoint start, end, h1, h2;

	/* compute points for an arrow scaled and rotated by feat's scl and ori */
	start_x = cvRound( feat.dx );
	start_y = cvRound( feat.dy );
	scl = feat.scale;
	ori = feat.ori;
	len = cvRound( scl * scale );
	hlen = cvRound( scl * hscale );
	blen = len - hlen;
	end_x = cvRound( len *  cos( ori ) ) + start_x;
	end_y = cvRound( len * -sin( ori ) ) + start_y;
	h1_x = cvRound( blen *  cos( ori + CV_PI / 18.0 ) ) + start_x;
	h1_y = cvRound( blen * -sin( ori + CV_PI / 18.0 ) ) + start_y;
	h2_x = cvRound( blen *  cos( ori - CV_PI / 18.0 ) ) + start_x;
	h2_y = cvRound( blen * -sin( ori - CV_PI / 18.0 ) ) + start_y;
	start = cvPoint( start_x, start_y );
	end = cvPoint( end_x, end_y );
	h1 = cvPoint( h1_x, h1_y );
	h2 = cvPoint( h2_x, h2_y );

	line( src, start, end, color, 1, 8, 0 );
	line( src, end, h1, color, 1, 8, 0 );
	line( src, end, h2, color, 1, 8, 0 );
}

void DrawSiftFeatures(Mat& src, vector<Keypoint>& features)
{
	CvScalar color = CV_RGB( 0, 255, 0 );
	for(int i = 0; i < features.size(); i++)
	{
		DrawSiftFeature(src, features[i], color);
	}
}

const char* GetFileName(const char* dir, int i)//很实用的函数
{
	char *name = new char[50];
	sprintf(name, "%s\\%d\.jpg",dir, i);
	return name;
}

void cv64f_to_cv8U(const Mat& src, Mat& dst)
{
	double* data = (double*)src.data;
	int step = src.step/sizeof(*data);

	if(!dst.empty())
		return;
	dst.create(src.size(), CV_8U);

	uchar* dst_data = dst.data;

	for(int i = 0, m = 0; i < src.cols; i++, m++)
	{
		for(int j = 0, n = 0; j < src.rows; j++, n++)
		{
			*(dst_data+dst.step*j+i) = (uchar)(*(data+step*j+i)*255);	
		}
	}
}

void write_pyr(const vector<vector<Mat>>& pyr, const char* dir)
{
	for(int i = 0; i < pyr.size(); i++)
	{
		for (int j=0;j<pyr[i].size();j++)
		{
			Mat dst;
			cv64f_to_cv8U((pyr[i])[j], dst);
			char *name = new char[50];
			sprintf(name, "%s\\%d\-\%d\.jpg",dir, i,j);
			imwrite(name, dst);
		}
	}
}