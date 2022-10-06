#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "Code_3B.h"
using namespace std;
using namespace cv;

const int width = 1000, height = 618;

void draw(const char* winName, vector<Point>& points) {
	Mat img(height, width, CV_8UC3, Scalar());
	for (int i = 1; i < points.size(); ++i)
		line(img, points[i - 1], points[i], Scalar(0, 255, 255));
	imshow(winName, img);
	waitKey(0);
}

int main() {
	string fileName;
	puts("请拖放文件至下方并按下 'Enter' 键:");
	getline(cin, fileName);
	while (!isalpha(*fileName.begin()))
		fileName.erase(fileName.begin());
	while (*(fileName.end() - 1) == '\"' || *(fileName.end() - 1) == '\n'
		|| *(fileName.end() - 1) == '\r' || *(fileName.end() - 1) == '\'')
		fileName.erase(fileName.end() - 1);
	ifstream fin(fileName);
	if (!fin.is_open()) {
		puts("打开文件失败！");
		puts("请检查文件名");
		system("pause");
		exit(0);
	}
	Code_3B code(fin);

	vector<Point> points = move(code.GetPoints(width, height));
	draw(fileName.c_str(), points);
	return 0;
}
