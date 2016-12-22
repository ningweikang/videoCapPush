
#include<string>
#include<iostream>



#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>



extern "C"{
#include<libavcodec/avcodec.h>
#include<libswscale/swscale.h>
#include<libavformat/avformat.h>
#include<libavutil/avutil.h>
#include<libavutil/time.h>
#include<libavutil/mathematics.h>
}



using namespace std;
using namespace cv;




int main(int argc, char* argv[]){
    Mat img = cv::imread("data/ocean.jpg");


    Mat tmp = Mat::zeros(5, 5, CV_16SC3);
    cout << tmp << endl;


    MatIterator_<Vec3b> it, end;
    for(it=tmp.begin<Vec3b>(), end=tmp.end<Vec3b>(); it!=end; ++it){
            (*it)[0] = 256;
    }
    cout << tmp << endl;



    cv::imshow("output", tmp);
    cv::waitKey();

}
