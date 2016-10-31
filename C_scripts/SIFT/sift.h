#ifndef SIFT_H
#define SIFT_H

#include <vector>
using namespace std;

#include <opencv.hpp>//只需要包含一个就可以了
using namespace cv;

/************************************************************************/
/*                    0 宏定义				   	                        */
/************************************************************************/
#define INIT_SIGMA 0.5

#define SIGMA 1.6

#define INTERVALS 3
 
#define RATIO 10
  
#define MAX_INTERPOLATION_STEPS 5 
  
#define DXTHRESHOLD 0.03

#define ORI_HIST_BINS 36    

#define ORI_SIGMA_TIMES 1.5

#define ORI_WINDOW_RADIUS 3.0 * ORI_SIGMA_TIMES 

#define ORI_SMOOTH_TIMES 2
  
#define ORI_PEAK_RATIO 0.8

#define FEATURE_ELEMENT_LENGTH 128

#define DESCR_HIST_BINS 8

#define IMG_BORDER 5 

#define DESCR_WINDOW_WIDTH 4

#define DESCR_SCALE_ADJUST 3

#define DESCR_MAG_THR 0.2

#define INT_DESCR_FCTR 512.0
  
 
/************************************************************************/
/*                    1 设计的数据结构		   	                        */
/************************************************************************/
struct Keypoint
{
	int octave; //关键点所在组
	int interval;// 关键点所在层

	double offset_interval;//插值调整后的层的增量

	int x; //x,y坐标,根据octave和interval可取的层内图像
	int y;

	//scale = sigma0*pow(2.0, o+s/S)
	double scale; //空间尺度坐标

	double dx; //特征点坐标，该坐标被缩放成原图像大小 
	double dy;

	double offset_x;
	double offset_y;

	//高斯金字塔组内各层尺度坐标，不同组的相同层的sigma值相同
	//关键点所在组的组内尺度
	double octave_scale; //offset_i;

	double ori;//方向

	int descr_length;
	double descriptor[FEATURE_ELEMENT_LENGTH]; //FEATURE_ELEMENT_LENGTH默认为128维向量

	double val;//极值
};


/************************************************************************/
/*                    2.1 函数声明——内部接口		   	                */
/************************************************************************/
bool GaussKernel1D(vector<double>& gausskernel, double sigma);//一维gauss模板
bool GaussKernel2D(vector<double>& gausskernel, double sigma);//二维gauss模板
void GaussianTemplateSmooth(const Mat &src, Mat &dst, double sigma);//使用自建模板平滑，未使用sigma，边缘无处理
bool GaussianSmoothSingle(const Mat &src, Mat &dst, double sigma, bool isOpenCV);//只能处理单通道的图像
bool GaussianSmoothMult(const Mat &src, Mat &dst, double sigma, bool isOpenCV);//处理多通道的图像
void ConvertToGray(const Mat& src, Mat& dst);
void UpSample(const Mat &src, Mat &dst);//升采样
void CreateInitSmoothGray(const Mat &src, Mat &dst, double sigma = SIGMA);//构建-1层初始图像，SIGMA=1.6
double PyrAt(const vector<vector<Mat>>& pyr, int octaves, int intervals, int x, int y);
void DownSample(const Mat& src, Mat& dst);//降采样
void GaussPyramid(const Mat& src, vector<vector<Mat>>& gausspyriads, int octaves , int intervals, double sigma);
void Sub(const Mat& a, const Mat& b, Mat & c);//图像相减
void DogPyramid(const vector<vector<Mat>>& gauss_pyr, vector<vector<Mat>>& dog_pyr, int octaves, int intervals);
bool isExtremum(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y);
void DerivativeOf3D(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double *dx);//一阶导数矩阵
void Hessian3D(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double *H);//3*3阶Hessian矩阵
bool Inverse3D(const double *H, double *H_inve);//3*3阶Hessian矩阵求逆
void GetOffsetX(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double *offset_x);
double GetFabsDx(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, const double* offset_x);
Keypoint* InterploationExtremum(const vector<vector<Mat>>& dog_pyr, int octave, int interval, int x, int y, double dxthreshold = DXTHRESHOLD);
bool passEdgeResponse(const vector<vector<Mat>>& pyr, int octaves, int intervals, int x, int y, double r = RATIO);//默认RATIO = 10
void DetectionLocalExtrema(const vector<vector<Mat>>& dog_pyr, vector<Keypoint>& extrema, int octaves, int intervals=INTERVALS);
void CalculateScale(vector<Keypoint>& features, double sigma = SIGMA, int intervals = INTERVALS);
void HalfFeatures(vector<Keypoint>& features);//由于计算Hessian矩阵导数的时候用了Taylor级数，有2倍关系
bool CalcGradMagOri(const Mat& gauss, int x, int y, double& mag, double& ori);
void CalculateOrientationHistogram(const Mat& gauss, int x, int y, int bins, int radius, double sigma, vector<double>& hist);
double DominantDirection(vector<double>& hist, int n);
void GaussSmoothOriHist(vector<double>& hist, int n);
void CalcOriFeatures(const Keypoint& keypoint, vector<Keypoint>& features, const vector<double>&hist, int n, double mag_thr);
void OrientationAssignment(vector<Keypoint>& extrema, vector<Keypoint>& features, const vector<vector<Mat>>& gauss_pyr);
void InterpHistEntry(double ***hist, double xbin, double ybin, double obin, double mag, int bins, int d);
void NormalizeDescr(Keypoint& feat);//归一化处理
void HistToDescriptor(double ***hist, int width, int bins, Keypoint& feature);
double*** CalculateDescrHist(const Mat& gauss, int x, int y, double octave_scale, double ori, int bins, int width);
void DescriptorRepresentation(vector<Keypoint>& features, const vector<vector<Mat>>& gauss_pyr, int bins, int width);


/************************************************************************/
/*                    2.1 函数声明——外部接口		   	                */
/************************************************************************/
void Sift(const Mat &src, vector<Keypoint>& features, double sigma, int intervals=INTERVALS);
void write_features(const vector<Keypoint>&features, const char*file);
bool FeatureCmp(Keypoint& f1, Keypoint& f2);
void DrawKeyPoints(Mat &src, vector<Keypoint>& keypoints);
void DrawSiftFeature(Mat& src, Keypoint& feat, CvScalar color);
void DrawSiftFeatures(Mat& src, vector<Keypoint>& features);
const char* GetFileName(const char* dir, int i);//很实用的函数
void write_pyr(const vector<vector<Mat>>& pyr, const char* dir);


#endif SIFT_H