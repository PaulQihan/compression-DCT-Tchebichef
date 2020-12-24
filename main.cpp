#include "include/transform.h"
#include "include/huffman.h"

double psnr(Mat &I1, Mat &I2){
	Mat s1;
	absdiff(I1, I2, s1);
	s1.convertTo(s1, CV_32F);//转换为32位的float类型，8位不能计算平方
	s1 = s1.mul(s1);
	Scalar s = sum(s1);  //计算每个通道的和
	double sse = s.val[0] + s.val[1] + s.val[2];
	if( sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double mse = sse / (double)(I1.channels() * I1.total()); //  sse/(w*h*3)
		double psnr = 10.0 * log10((255*255)/mse);
		return psnr;
	}
	
	
	
 
}
double ssim(Mat &i1, Mat & i2){
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I1_2 = I1.mul(I1);
	Mat I2_2 = I2.mul(I2);
	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11,11), 1.5);
	GaussianBlur(I2, mu2, Size(11,11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigam2_2, sigam12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
 
	GaussianBlur(I2_2, sigam2_2, Size(11, 11), 1.5);
	sigam2_2 -= mu2_2;
 
	GaussianBlur(I1_I2, sigam12, Size(11, 11), 1.5);
	sigam12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigam12 + C2;
	t3 = t1.mul(t2);
 
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigam2_2 + C2;
	t1 = t1.mul(t2);
 
	Mat ssim_map;
	divide(t3, t1, ssim_map);
	Scalar mssim = mean(ssim_map);
 
	double ssim = (mssim.val[0] + mssim.val[1] + mssim.val[2]) /3;
	return ssim;
}
int main()
{

	float msk[8][8] = { {16,11,10,16,24,40,51,61},{12,12,14,19,26,58,60,55},{14,13,16,24,40,57,69,56},{14,17,22,29,51,87,80,62},{18,22,37,56,68,109,103,77},{24,35,55,64,81,104,113,92},{49,64,78,87,103,121,120,101},{72,92,95,98,112,100,103,99} };
	float msk2[8][8] = { {17, 18, 24, 47, 99, 99, 99, 99}, {18, 21, 26, 66, 99, 99, 99, 99}, {24, 26, 56, 99, 99, 99, 99, 99}, {47, 66, 99, 99, 99, 99, 99, 99}, {99, 99, 99, 99, 99, 99, 99, 99}, {99, 99, 99, 99, 99, 99, 99, 99}, {99, 99, 99, 99, 99, 99, 99, 99}, {99, 99, 99, 99, 99, 99, 99, 99} };
	Mat mask(8, 8, CV_32F, msk);
	Mat mask2(8, 8, CV_32F, msk2);

	Mat src = imread("1.bmp");
	int rows = src.rows;
	int cols = src.cols;
	Mat YUVsrc;
	Mat res, res_2;
	cvtColor(src, YUVsrc, CV_BGR2YUV);
	vector<Mat> yuv;
	split(YUVsrc, yuv);
	Mat y, u, v;
	y = yuv[0];
	u = yuv[1];
	v = yuv[2];
	y.convertTo(y, CV_32F, 1.0 / 255);
    u.convertTo(u, CV_32F, 1.0 / 255);
	v.convertTo(v, CV_32F, 1.0 / 255);
	
	int begin = clock();
	Transform d;
	Mat A = d.initDctMat();
	Mat ydct, udct, vdct, yidct, uidct, vidct;
	ydct = d.block(y, A, mask);
	udct = d.block(u, A, mask2);
	vdct = d.block(v, A, mask2);

	HuffmanEncode e;
	int y_num = e.encode(ydct, "ytest");
	int u_num = e.encode(udct, "utest");
	int v_num = e.encode(vdct, "vtest");
	int end = clock();
	cout << "encoding time: " << end - begin << endl;
	cout << "compression ratio: " << float(y_num + u_num + v_num )/ float(rows*cols*3*8) << endl;

	begin = clock();
	HuffmanDecode decoder;
	Mat huf_y = decoder.decode("ytest", rows, cols);
	Mat huf_u = decoder.decode("utest", rows, cols);
	Mat huf_v = decoder.decode("vtest", rows, cols);

	Mat huf_iy = d.iblock(huf_y, A, mask);
	Mat huf_iu = d.iblock(huf_u, A, mask2);
	Mat huf_iv = d.iblock(huf_v, A, mask2);

	vector<Mat> huf_idct = { huf_iy, huf_iu, huf_iv};	
	Mat tmp;
	merge(huf_idct, tmp);
	cvtColor(tmp, res, CV_YUV2BGR);
	end = clock();
	cout << "decoding time: " << end - begin << endl;
	res.convertTo(res, CV_8UC1, 255);
	imwrite("res.bmp", res);		
		
	begin = clock();
	
	Mat A_2 = d.initDttMat();
	Mat ydct_2, udct_2, vdct_2, yidct_2, uidct_2, vidct_2;
	ydct_2 = d.block(y, A_2, mask);
	udct_2 = d.block(u, A_2, mask2);
	vdct_2 = d.block(v, A_2, mask2);

	int y_num_2 = e.encode(ydct_2, "ytest2");
	int u_num_2 = e.encode(udct_2, "utest2");
	int v_num_2 = e.encode(vdct_2, "vtest2");
	end = clock();
	cout << "encoding time: " << end - begin << endl;
	cout << "compression ratio: " << float(y_num_2 + u_num_2 + v_num_2 )/ float(rows*cols*3*8) << endl;

	begin = clock();
	Mat huf_y_2 = decoder.decode("ytest2", rows, cols);
	Mat huf_u_2 = decoder.decode("utest2", rows, cols);
	Mat huf_v_2 = decoder.decode("vtest2", rows, cols);

	Mat huf_iy_2 = d.iblock(huf_y_2, A_2, mask);
	Mat huf_iu_2 = d.iblock(huf_u_2, A_2, mask2);
	Mat huf_iv_2 = d.iblock(huf_v_2, A_2, mask2);

	vector<Mat> huf_idct_2 = { huf_iy_2, huf_iu_2, huf_iv_2};	
	Mat tmp_2;
	merge(huf_idct_2, tmp_2);
	cvtColor(tmp_2, res_2, CV_YUV2BGR);
	end = clock();
	cout << "decoding time: " << end - begin << endl;

	res_2.convertTo(res_2, CV_8UC1, 255);

	cout << "psnr dct: " << psnr(src, res) << endl;
	cout << "ssim dct: " <<ssim(src, res) << endl;
	cout << "psnr dtt: " << psnr(src, res_2) << endl;
	cout << "ssim dtt: " <<ssim(src, res_2) << endl;	
	imwrite("res2.bmp", res_2);
	return 0;
}
