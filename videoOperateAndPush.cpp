
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



static const AVPixelFormat sourcePixelFormat = AV_PIX_FMT_BGR24;
static const AVPixelFormat destPixelFormat = AV_PIX_FMT_YUV420P;
static const AVCodecID destCodec = AV_CODEC_ID_H264;




using namespace std;
using namespace cv;






#define __STDC_CONSTANT_MACROS


#define MY_PIX_FMT AV_PIX_FMT_YUV420P



extern "C"
{
#include<libavutil/avutil.h>
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
#include<libavutil/opt.h>
#include<libavutil/imgutils.h>
}



int main(int argc, char* argv[]){
    avcodec_register_all();
    av_register_all();
    avformat_network_init();


    const std::string videoStreamAddress = "rtmp://202.117.124.80:9419/live1/room2";
    //const std::string videoStreamAddress = "1.mp4";
    const std::string outputStreamAddress = "rtmp://202.117.124.80:9419/live1/room1";


    cv::VideoCapture vcap;
    if(!vcap.open(videoStreamAddress)){
        std::cout << "Fail: Error opening video stream or file" << std::endl;
        return -1;
    }


    int width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
    cout << "Frame width: " << width << endl;
    cout << "Frame height: " << height << endl;



    /*******************************/
    AVCodecID codec_id = AV_CODEC_ID_H264;

    AVCodec *pCodec = avcodec_find_encoder(codec_id);
    if(!pCodec){
        cout << "Fail: Codec not found!" << endl;
    }

    AVCodecContext *pCodecCtx = NULL;
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(!pCodecCtx){
        cout << "Fail: Could not allocate video codec context" << endl;
    }

    pCodecCtx->codec_id = codec_id; //!!!!!!!!!!!!!!!!!!!!!!!! must set!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    pCodecCtx->bit_rate = 400000; // put sample parameters

    pCodecCtx->width = width;
    pCodecCtx->height = height;

    pCodecCtx->time_base = (AVRational) {1,25};

    pCodecCtx->gop_size = 250; // emit one intra frame every ten frames
    pCodecCtx->max_b_frames = 1;
    pCodecCtx->thread_count = 1;

    pCodecCtx->pix_fmt = MY_PIX_FMT;

    //H264
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 51; //???

    av_opt_set(pCodecCtx->priv_data, "preset", "slow", 0);

    // Open encoder
    int ret = -2;
    if( (ret = avcodec_open2(pCodecCtx, pCodec, NULL)) < 0 ){
        cout << "Fail: Could not open the encoder!" << endl;
        return -1;
    }//cout << "OK: Open codec OK!" << endl;

    /*******************************/
    // yuv buffer
    int yuv_bufLen = width*height*3/2;
    uint8_t *pYuvBuf = new uint8_t[yuv_bufLen];
    cout << "YUV buffer length: " << yuv_bufLen << endl;
    cout << "calculated yuv size: " << avpicture_get_size(MY_PIX_FMT, width, height) << endl;

    /******************************/
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ofmt_ctx = NULL;

    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", outputStreamAddress.c_str());
    if(!ofmt_ctx){
        cout << "Fail: Could not create output context." << endl;
        ret = AVERROR_UNKNOWN;
    }

    ofmt = ofmt_ctx->oformat;

    // ?????purpose
    AVStream *out_stream = avformat_new_stream(ofmt_ctx, pCodec);
    if(!out_stream){
        cout << "Fail: Allocating output stream failed!" << endl;
    }

    ret = avcodec_copy_context(out_stream->codec, pCodecCtx);
    if(ret < 0){
        cout << "Fail: Failed to copy context from codec context!" << endl;
    }

    out_stream->codec->codec_tag = 0;
    if(ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER){
        out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    // Dump format
    av_dump_format(ofmt_ctx, 0, outputStreamAddress.c_str(), 1);

    // avio_open
    if( !(ofmt->flags & AVFMT_NOFILE) ){
        ret = avio_open(&ofmt_ctx->pb, outputStreamAddress.c_str(), AVIO_FLAG_WRITE);
        if(ret<0){
            cout << "Fail: Could not open output URL: " << outputStreamAddress.c_str() << endl;
        }
    }

    // avformat_write_header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if(ret < 0){
        cout << "Fail: Error occurred when opening output URL." << endl;
    }
    /******************************/






    int64_t start_time = 0;
    start_time = av_gettime();

    // Loooooooooooooop
    cv::Mat image;
    cv::namedWindow("output");

    int frameCnt = 0;
    for(int i=0;;i++)
    {
        if(!vcap.read(image))
        {
            std::cout << "No frame" << std::endl;
            cv::waitKey();
        }
        cv::imshow("output", image);



        /*******************************/
        //BGR to YUV
        cv::Mat yuvImg;
        cvtColor(image, yuvImg, CV_BGR2YUV_I420);
        memcpy(pYuvBuf, yuvImg.data, yuv_bufLen*sizeof(uint8_t)); // pYuvBuf is the YUV data achieved
        /////////////////////////////////
        // YUV to 264
        AVFrame *yuvFrame = av_frame_alloc();
        avpicture_fill((AVPicture*) yuvFrame, pYuvBuf, MY_PIX_FMT, width, height);

        yuvFrame->data[0] = pYuvBuf;      // Y
        yuvFrame->data[1] = pYuvBuf + width*height;       // U
        yuvFrame->data[2] = pYuvBuf + width*height*5/4;   // V

        //yuvFrame->pts = i;
        /////////////////////////////////

        // Encode: YUV frame to h264
        AVPacket avpkt;
        av_new_packet(&avpkt, avpicture_get_size(AV_PIX_FMT_YUV420P, width, height));

        int got_picture;
        ret = avcodec_encode_video2(pCodecCtx, &avpkt, yuvFrame, &got_picture);
        //cout << "ret=" << ret << ";\t got_picture=" << got_picture << endl;
        if(1==got_picture){
            frameCnt++;
            // ????????
        }
        /*******************************/
        if(avpkt.pts==AV_NOPTS_VALUE){
            //cout << "No" << endl;
            avpkt.pts = i;
            avpkt.dts = avpkt.pts;
            avpkt.duration = AV_TIME_BASE;
        }

        // Delay!!!!
        av_usleep(5000);

        ret = av_interleaved_write_frame(ofmt_ctx, &avpkt);
        if(ret < 0){
            cout << "Fail: Error muxing pakket." << endl;
        }



        /******************************/

        av_free_packet(&avpkt);
        /******************************/

        if(cv::waitKey(1) >= 0) break;
        //cout << i << "th frame" << endl;
    }

    /******************************/
    av_write_trailer(ofmt_ctx);




    /******************************/

    return 0;
}
