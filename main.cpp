#include <opencv.hpp>
#include <highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <thread>
#include <omp.h>

int main(int argc, char* argv[])
{
    cv::Mat image;
    cv::Mat templ;
    const char* filename = argc >= 2 ? argv[1] : "circle-2.jpg";
//  image = cv::imread(filename,1);

    printf("[i] image: %s\n", filename);

    const char* filename2 = argc >= 3 ? argv[2] : "circle-1.jpg";
    printf("[i] template: %s\n", filename2);

    templ = cv::imread(filename2,1);

    //cv::cvtColor(templ, templ, cv::COLOR_BGR2HSV);
    //cv::Canny(templ, templ, 100, 100);
    //templ = templ < 100;
    //cv::inRange(templ, cv::Scalar(0, 0, 200, 0), cv::Scalar(180, 255, 255, 0), templ);
    cv::threshold(templ, templ, 127, 255, cv::THRESH_BINARY);

    cv::namedWindow("Match", CV_WINDOW_NORMAL);
    cv::namedWindow("Template", CV_WINDOW_NORMAL);
    cv::imshow("Template", templ);


    cv::VideoCapture cap(0);
    if (!cap.open(0)) {
        perror("bad cap");
    }
    cap.set(CV_CAP_PROP_FPS, 1);
    while (1) {
        for (int i = 0; i < 30; i++) {
            cap.grab();
        }
        cap.retrieve(image);
        if(! image.data )
        {
            perror("Could not open or find the image");
            continue;
        }
        //cv::cvtColor(image, image, cv::COLOR_BGR2HSV);
        //cv::Canny(image, image, 100, 100);
        //image = image > 100;
        cv::threshold(image, image, 127, 255, cv::THRESH_BINARY);
        //cv::inRange(image, cv::Scalar(0, 0, 200, 0), cv::Scalar(180, 255, 255, 0), image);
        cv::Mat res;
        int result_cols =  image.cols - templ.cols + 1;
        int result_rows = image.rows - templ.rows + 1;
        res.create( result_rows, result_cols, 0);
        cv::matchTemplate(image, templ, res, CV_TM_SQDIFF_NORMED);

        double min_val, max_val;
        cv::Point min_loc, max_loc;

        cv::minMaxLoc(res, &min_val, &max_val, &min_loc, &max_loc, cv::Mat());

        int templ_cols_res = templ.cols;
        int templ_rows_res = templ.rows;
        #pragma omp parallel for
        for (int scale = 100; scale > 10; scale -= 5) {
            double min_val_res;
            double max_val_res;
            cv::Point min_loc_res;
            cv::Point max_loc_res;
            cv::Mat res;
            cv::Mat resized_template;
            int result_cols =  image.cols - templ.cols + 1;
            int result_rows = image.rows - templ.rows + 1;
            res.create( result_rows, result_cols, 0);
            cv::resize(templ, resized_template, cv::Size(), scale/100.0, scale/100.0, CV_INTER_LINEAR);

            cv::matchTemplate(image,
                              resized_template, res, CV_TM_SQDIFF_NORMED);
            cv::minMaxLoc(res, &min_val_res, &max_val_res, &min_loc_res, &max_loc_res, cv::Mat());
            // For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
            #pragma omp critical
            {
                if (min_val_res < min_val) {
                    min_val = min_val_res;
                    max_val = max_val_res;
                    min_loc = min_loc_res;
                    max_loc = max_loc_res;
                    templ_cols_res = templ.cols * scale / 100.0;
                    templ_rows_res = templ.rows * scale / 100.0;
                }
            }
        }
        printf("Hey!\n");
        cv::Mat img_display;
        image.copyTo( img_display );
        cv::rectangle(img_display, min_loc, cv::Point(min_loc.x + templ_cols_res, min_loc.y + templ_rows_res), cv::Scalar(255, 0, 255), 2, 8, 0);
        cv::imshow("Match", img_display);

        if( cv::waitKey(10) == 27 ) break;
    }

    //cvReleaseImage( &image );
    //cvReleaseImage( &templ );
    //cvReleaseImage( &res );
    cvDestroyAllWindows();
    return 0;
}
