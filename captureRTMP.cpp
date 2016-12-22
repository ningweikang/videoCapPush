
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




class CaptureFFMPEG{
public:
    CaptureFFMPEG(string inputStreamAddress);
    void readFrame();
    void readFrame(int height, int width, int channels);

private:
    cv::VideoCapture videoCap;
    cv::Mat image;

};

CaptureFFMPEG::CaptureFFMPEG(string inputStreamAddress){
    avcodec_register_all();
    av_register_all();
    avformat_network_init();

    if( !videoCap.open(inputStreamAddress) ){
        std::cout << "Fail: Error opening video stream or file" << std::endl;
    } else {
        std::cout << "OK: Open video succeed." << std::endl;
    }
}

void CaptureFFMPEG::readFrame(){
    std::cout << "read frame" << std::endl;
    if( !videoCap.read(image) ){
            std::cout << "No frame" << std::endl;
    }
}


void CaptureFFMPEG::readFrame(int height, int width, int channels){
    std::cout << "Read frame in C++" << std::endl;
}




int main1(int argc, char* argv[]){
    const std::string videoStreamAddress = "rtmp://202.117.124.80:9419/live1/room2";


    CaptureFFMPEG cap(videoStreamAddress);

    cv::namedWindow("out");
    while(true){
        cap.readFrame();
    }




    return 0;


}
