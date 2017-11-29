#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <thread>
#include <omp.h>
#include <vector>
#include <fcntl.h>
#include <termios.h>
#include <limits.h>


cv::Point get_coord(cv::Mat* image, cv::VideoCapture* cap)
{
    for (int i = 0; i < 1; i++) {
        cap->grab();
    }
    cap->retrieve(*image);
    if(! image->data )
    {
        perror("Could not open or find the image");
    }
    //cv::medianBlur(image, image, 3);
    cv::Mat img_display;
    image->copyTo( img_display );
    cv::cvtColor(*image, *image, cv::COLOR_BGR2HSV);
    cv::Mat lower_red_hue_range;
    cv::Mat upper_red_hue_range;
    cv::inRange(*image, cv::Scalar(0, 130, 70), cv::Scalar(23, 255, 255), lower_red_hue_range);
    cv::inRange(*image, cv::Scalar(170, 110, 110), cv::Scalar(179, 255, 255), upper_red_hue_range);

    cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, *image);
    cv::GaussianBlur(*image, *image, cv::Size(9, 9), 2, 2);
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(*image, circles, CV_HOUGH_GRADIENT, 1, image->rows/8, 100, 20, 0, 0);


    //if(circles.size() == 0) std::exit(-1);
    int radius = 0;
    int x_coord = 0;
    int y_coord = 0;
    for(size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
        radius += std::round(circles[current_circle][2]);
        x_coord += std::round(circles[current_circle][0]);
        y_coord += std::round(circles[current_circle][1]);
    }
    //cv::imshow("Match", img_display);
    if (circles.size() != 0) {
        cv::Point center(std::round(x_coord/circles.size()), std::round(y_coord/circles.size()));
        //cv::circle(img_display, center, std::round(radius/circles.size()), cv::Scalar(0, 255, 0), 5);
        cv::Point landingPoint((std::round(x_coord/circles.size() - image->cols/2.0)),
                               (std::round(y_coord/circles.size() - image->rows/2.0)));
        return landingPoint;
    }
    return cv::Point(INT_MAX, INT_MAX);
}

int main(int argc, char* argv[])
{
    /*
    struct termios serial;
    int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror(argv[1]);
        return -1;
    }
    if (tcgetattr(fd, &serial) < 0) {
        perror("Getting configuration");
        return -1;
    }
    serial.c_iflag = 0;
    serial.c_oflag = 0;
    serial.c_lflag = 0;
    serial.c_cflag = 0;

    serial.c_cc[VMIN] = 0;
    serial.c_cc[VTIME] = 0;

    serial.c_cflag = B115200 | CS8 | CREAD;

    tcsetattr(fd, TCSANOW, &serial);
    */
    cv::Mat image;
    //cv::namedWindow("Match", CV_WINDOW_NORMAL);

    cv::VideoCapture cap(0);
    if (!cap.open(0)) {
        perror("bad cap");
    }
    //cap.set(CV_CAP_PROP_FPS, 1);
    while (1) {
        cv::Point point = get_coord(&image, &cap);
        printf("Hey %d %d!\n", point.x, point.y);
        /*
        char msg[40] = {};
        if (point.x == INT_MAX) {
            sprintf(msg, "not found\n");
        }else {
            sprintf(msg, "%d %d\n", point.x, point.y);
        }
        int wcount = write(fd, msg, strlen(msg));
        if (wcount < 0) {
            perror("Write");
            return -1;
        }
        */
        if( cv::waitKey(10) == 27 ) break;
    }

    //cvReleaseImage( &image );
    //cvReleaseImage( &templ );
    //cvReleaseImage( &res );
    cvDestroyAllWindows();
    return 0;
}
