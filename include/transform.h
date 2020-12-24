#include "head.h"

class Transform {
public:
       Transform();

       Mat initDctMat();
       Mat initDttMat();
       Mat block(const Mat& src, const Mat& A, const Mat& mask);
       Mat iblock(const Mat& src, const Mat& A, const Mat& mask);

       ~Transform();  
};
