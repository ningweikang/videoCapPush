
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



int mainT(int argc, char* argv[]){
    avcodec_register_all();
    av_register_all();
    avformat_network_init();


    const std::string videoStreamAddress = "rtmp://202.117.124.80:9419/live1/room2";
    //const std::string outputStreamAddress = "rtmp://202.117.124.80:9419/live1/room1";


    cv::VideoCapture vcap;
    if(!vcap.open(videoStreamAddress)){
        std::cout << "Fail: Error opening video stream or file" << std::endl;
        return -1;
    } else {
        cout << "OK: Open video succeed." << endl;
    }




    return 0;


}
