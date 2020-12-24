#include "../include/transform.h"

Transform::Transform() {
}

Transform::~Transform() {
}

Mat Transform::initDctMat() {
        Mat A(Size(8, 8), CV_32F);
        for (int i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                        float a;
                        if (i == 0)
                                a = sqrt(1.0 / 8.0);
                        else
                                a = sqrt(2.0 / 8.0);
                        A.ptr<float>(i)[j] = a * cos((j + 0.5) * PI * i / 8);
                }
        return A;
}

Mat Transform::initDttMat() {
        Mat A(Size(8, 8), CV_32F);
        double n = 8.0;
        for (double i = 0; i < 8; ++i)
                for (int j = 0; j < 8; ++j)
                {
                        float a;
                        if (i == 0)
                                a = sqrt(1.0 / n);
                        else if (i == 1){
                                a = (2.0 * j + 1 - n) * sqrt(3.0 / (n * (n*n - 1)));
                        }
                        else {
                                double alpha1 = (2.0 / i) * sqrt((4.0 * i*i - 1) / (n*n - i*i));
                                double alpha2 = ((1 - n) / i) * sqrt((4.0 * i*i - 1) / (n*n - i*i));
                                double alpha3 = ((1 - i) / i) * sqrt((2 * i + 1) / (2 * i - 3)) * sqrt((n*n - (i - 1) * (i - 1)) / (n*n - i*i));
                                a = (alpha1 * j + alpha2) * A.ptr<float>(i-1)[j] + alpha3 * A.ptr<float>(i-2)[j];
                        }
                        A.ptr<float>(i)[j] = a;
                }
        return A;        
}

Mat Transform::block(const Mat& src, const Mat& A, const Mat& mask) {
        int rows = src.rows;
        int cols = src.cols;

        Mat res = src.clone();
        for(int i = 0; i < rows; i+= 8)
                for (int j = 0; j < cols; j += 8) {
                        res(Range(i, i + 8), Range(j, j + 8)) = A *res(Range(i, i + 8), Range(j, j + 8)) * A.t(); 
                        res(Range(i, i + 8), Range(j, j + 8)) /= mask; //quantative
                }
        return res;
}

Mat Transform::iblock(const Mat& src, const Mat& A, const Mat& mask) {
        int rows = src.rows;
        int cols = src.cols;

        Mat res = src.clone();
        for (int i = 0; i < rows; i += 8)
                for (int j = 0; j < cols; j += 8) {
                        res(Range(i, i + 8), Range(j, j + 8)) = res(Range(i, i + 8), Range(j, j + 8)).mul(mask); 
                        res(Range(i, i + 8), Range(j, j + 8)) = A.t() * res(Range(i, i + 8), Range(j, j + 8)) * A; 
                }
        return res;
}
