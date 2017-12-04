#pragma once
#include "CImg.h"
#include <string>

void captureOnce(std::string);

void eigen(cimg_library::CImg<unsigned char>, cimg_library::CImg<unsigned char>);

bool deltaImage(cimg_library::CImg<unsigned char> image, int red, int green, int blue);

void loop(int);

void lightDelay(int);

void setCamera(bool);

int system_no_output(std::string command);

void logging(std::string);