#ifndef SIFT_H
#define SIFT_H

#include <vector>
using namespace std;

#include <opencv.hpp>//ֻ��Ҫ����һ���Ϳ�����
using namespace cv;

/************************************************************************/
/*                    0 �궨��				   	                        */
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
/*                    1 ��Ƶ����ݽṹ		   	                        */
/************************************************************************/
struct Keypoint
{
	int octave; //�ؼ���������
	int interval;// �ؼ������ڲ�

	double offset_interval;//��ֵ������Ĳ������

	int x; //x,y����,����octave��interval��ȡ�Ĳ���ͼ��
	int y;

	//scale = sigma0*pow(2.0, o+s/S)
	double scale; //�ռ�߶�����

	double dx; //���������꣬�����걻���ų�ԭͼ���С 
	double dy;

	double offset_x;
	double offset_y;

	//��˹���������ڸ���߶����꣬��ͬ�����ͬ���sigmaֵ��ͬ
	//�ؼ�������������ڳ߶�
	double octave_scale; //offset_i;

	double ori;//����

	int descr_length;
	double descriptor[FEATURE_ELEMENT_LENGTH]; //FEATURE_ELEMENT_LENGTHĬ��Ϊ128ά����

	double val;//��ֵ
};


/************************************************************************/
/*                    2.1 �������������ڲ��ӿ�		   	                */
/************************************************************************/
bool GaussKernel1D(vector<double>& gausskernel, double sigma);//һάgaussģ��
bool GaussKernel2D(vector<double>& gausskernel, double sigma);//��άgaussģ��
void GaussianTemplateSmooth(const Mat &src, Mat &dst, double sigma);//ʹ���Խ�ģ��ƽ����δʹ��sigma����Ե�޴���
bool GaussianSmoothSingle(const Mat &src, Mat &dst, double sigma, bool isOpenCV);//ֻ�ܴ���ͨ����ͼ��
bool GaussianSmoothMult(const Mat &src, Mat &dst, double sigma, bool isOpenCV);//�����ͨ����ͼ��
void ConvertToGray(const Mat& src, Mat& dst);
void UpSample(const Mat &src, Mat &dst);//������
void CreateInitSmoothGray(const Mat &src, Mat &dst, double sigma = SIGMA);//����-1���ʼͼ��SIGMA=1.6
double PyrAt(const vector<vector<Mat>>& pyr, int octaves, int intervals, int x, int y);
void DownSample(const Mat& src, Mat& dst);//������
void GaussPyramid(const Mat& src, vector<vector<Mat>>& gausspyriads, int octaves , int intervals, double sigma);
void Sub(const Mat& a, const Mat& b, Mat & c);//ͼ�����
void DogPyramid(const vector<vector<Mat>>& gauss_pyr, vector<vector<Mat>>& dog_pyr, int octaves, int intervals);
bool isExtremum(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y);
void DerivativeOf3D(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double *dx);//һ�׵�������
void Hessian3D(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double *H);//3*3��Hessian����
bool Inverse3D(const double *H, double *H_inve);//3*3��Hessian��������
void GetOffsetX(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, double *offset_x);
double GetFabsDx(const vector<vector<Mat>>& dog_pyr, int octaves, int intervals, int x, int y, const double* offset_x);
Keypoint* InterploationExtremum(const vector<vector<Mat>>& dog_pyr, int octave, int interval, int x, int y, double dxthreshold = DXTHRESHOLD);
bool passEdgeResponse(const vector<vector<Mat>>& pyr, int octaves, int intervals, int x, int y, double r = RATIO);//Ĭ��RATIO = 10
void DetectionLocalExtrema(const vector<vector<Mat>>& dog_pyr, vector<Keypoint>& extrema, int octaves, int intervals=INTERVALS);
void CalculateScale(vector<Keypoint>& features, double sigma = SIGMA, int intervals = INTERVALS);
void HalfFeatures(vector<Keypoint>& features);//���ڼ���Hessian��������ʱ������Taylor��������2����ϵ
bool CalcGradMagOri(const Mat& gauss, int x, int y, double& mag, double& ori);
void CalculateOrientationHistogram(const Mat& gauss, int x, int y, int bins, int radius, double sigma, vector<double>& hist);
double DominantDirection(vector<double>& hist, int n);
void GaussSmoothOriHist(vector<double>& hist, int n);
void CalcOriFeatures(const Keypoint& keypoint, vector<Keypoint>& features, const vector<double>&hist, int n, double mag_thr);
void OrientationAssignment(vector<Keypoint>& extrema, vector<Keypoint>& features, const vector<vector<Mat>>& gauss_pyr);
void InterpHistEntry(double ***hist, double xbin, double ybin, double obin, double mag, int bins, int d);
void NormalizeDescr(Keypoint& feat);//��һ������
void HistToDescriptor(double ***hist, int width, int bins, Keypoint& feature);
double*** CalculateDescrHist(const Mat& gauss, int x, int y, double octave_scale, double ori, int bins, int width);
void DescriptorRepresentation(vector<Keypoint>& features, const vector<vector<Mat>>& gauss_pyr, int bins, int width);


/************************************************************************/
/*                    2.1 �������������ⲿ�ӿ�		   	                */
/************************************************************************/
void Sift(const Mat &src, vector<Keypoint>& features, double sigma, int intervals=INTERVALS);
void write_features(const vector<Keypoint>&features, const char*file);
bool FeatureCmp(Keypoint& f1, Keypoint& f2);
void DrawKeyPoints(Mat &src, vector<Keypoint>& keypoints);
void DrawSiftFeature(Mat& src, Keypoint& feat, CvScalar color);
void DrawSiftFeatures(Mat& src, vector<Keypoint>& features);
const char* GetFileName(const char* dir, int i);//��ʵ�õĺ���
void write_pyr(const vector<vector<Mat>>& pyr, const char* dir);


#endif SIFT_H