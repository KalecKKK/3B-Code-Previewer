#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#include <string>
#include <iostream>
#include <opencv.hpp>
typedef unsigned int Bval;
typedef char Gtype;

extern enum LineType;

class Code_3B_line {
public:
	std::string _line;
	Bval _x, _y, _z;
	Gtype _g;
	LineType _lineType;
	Bval _dirc;
	bool _isEnd;

	Code_3B_line(const std::string& str);
	std::vector<cv::Point2f> GetPoints(const cv::Point2f& startPoint);
};

class Code_3B {
private:
	std::vector<Code_3B_line> _lines;
public:
	Code_3B(std::istream& fin);
	Code_3B(const std::vector<std::string> lines);

	std::vector<cv::Point> GetPoints(int width, int height);
};

