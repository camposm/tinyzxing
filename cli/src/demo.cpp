#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "ImageReaderSource.h"
#include <zxing/ZXingConfig.h>
#include <zxing/Binarizer.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/LuminanceSource.h>

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


using namespace zxing;
using namespace zxing::qrcode;


class OpenCVSource : public zxing::LuminanceSource 
{
public:
    void load_frame (const cv::Mat& full_frame, const cv::Rect& area)
    {
        cv::Mat dst = full_frame(area);

        cv::Mat gray;
        cv::cvtColor(dst, gray, cv::COLOR_BGR2GRAY, 0);
        for (int y = 0; y < gray.rows; y++) 
        {
            for (int x = 0; x < gray.cols; x++) 
            {
                image[(y * config::ImageWidth + x)] = gray.at<uint8_t>(y, x);
            }
        }
    }

    const zxing::lum_row_t& getRow (int row_no) const 
    {
        auto data = image.data() + row_no * zxing::config::ImageWidth;
        return *reinterpret_cast<const zxing::lum_row_t*>(data);
    }

    const zxing::lum_matrix_t& getMatrix() const 
    {
        return image;
    }
    
private:
    zxing::lum_matrix_t image;
};


int main(int argc, char** argv) 
{
    cv::VideoCapture cap(0);
    
    if (!cap.isOpened()) 
    {
        std::cout << "e: failed to open camera\n";
        return 0;
    }
    
    cv::Mat frame;
    OpenCVSource source;
    auto global_bin = GlobalHistogramBinarizer(source);
    BinaryBitmap binary(global_bin);
    QRCodeReader qr_reader;

    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int xpos = 100; //(width - config::ImageWidth) / 2;
    int ypos = 100; // (height - config::ImageHeight) / 2;
    
    cv::Rect area(xpos, ypos, config::ImageWidth, config::ImageHeight);
    auto& tmp = binary.getBlackMatrix();
    int mx = tmp.getWidth();
    int my = tmp.getHeight();
    
    // rows, cols
    cv::Mat hist(my, mx, CV_8UC3, cv::Scalar(0,0,0));

    while (true)
    {
        cap.read(frame);
        if (frame.empty()) 
        {
            std::cout << "e: read empty frame\n";
            return 0;
        }

        source.load_frame(frame, area);

        auto& black = binary.getBlackMatrix();
        for (int y = 0; y < my; y++) 
        {
            for (int x = 0; x < mx; x++) 
            {
                hist.at<cv::Vec3b>(y, x) = black.get(x, y) ? cv::Vec3b(255,255,255) : cv::Vec3b(0,0,0);
            }
        }
        
        auto result = qr_reader.decode(binary);
        
        if (result.second != 0)
        {
            std::string text;
            std::cout << "[";
            for (auto n = 0; n < result.second; ++n) 
            {
                std::cout << result.first[n];
                text += result.first[n];
            }
            std::cout << "]\n";
            cv::putText(frame, text, cv::Point(xpos, ypos-10), cv::FONT_HERSHEY_DUPLEX, 1.0, CV_RGB(0, 0, 255), 1, cv::LINE_AA);
        }
        
        cv::rectangle(frame, area, cv::Scalar(0, 255, 0));
        
        int xstart = xpos + config::ImageWidth + 100;
        hist.copyTo(frame.rowRange(ypos, ypos + my).colRange(xstart, xstart + mx));
        
        cv::imshow("tinyzxing", frame);
        cv::waitKey(5);
    }
    
    return 0;
}
