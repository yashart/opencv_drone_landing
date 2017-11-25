#include <opencv.hpp>
#include <highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sstream>

int main(int argc, char* argv[]) {
    cv::Mat templ;
    cv::Mat resized_template;
    const char* filename2 = argc >= 3 ? argv[2] : "circle-1.jpg";
    printf("[i] template: %s\n", filename2);
    templ = cv::imread(filename2,1);
    for (double scale = 1; scale > 0.1; scale -= 0.05) {
        std::ostringstream templname;
        templname << "templ";
        cv::resize(templ, resized_template, cv::Size(), scale, scale, CV_INTER_LINEAR);
        templname << scale << ".jpg";
        cv::imwrite(templname.str(), templ);
    }
    return 0;
}
