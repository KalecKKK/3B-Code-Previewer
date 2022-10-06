#include "Code_3B.h"
using namespace std;
using namespace cv;

enum LineType { L, SR, NR };

Code_3B_line::Code_3B_line(const std::string& str) : _line(str) {
	int i = 0, len = str.length();
	try { // 从行中读取信息
		if (str[0] == 'E') {
			_isEnd = true;
			return;
		} else _isEnd = false;

		while (i < len && str[i] != 'B') ++i; ++i;
		while (i < len && !isdigit(str[i]) && str[i] != 'B') ++i;
		if (i == len) throw "X数据";
		if (str[i] != 'B') sscanf(str.c_str() + i, "%u", &_x);
		else _x = 0;

		while (i < len && str[i] != 'B') ++i; ++i;
		while (i < len && !isdigit(str[i]) && str[i] != 'B') ++i;
		if (i == len) throw "Y数据";
		if (str[i] != 'B') sscanf(str.c_str() + i, "%u", &_y);
		else _y = 0;

		while (i < len && str[i] != 'B') ++i; ++i;
		while (i < len && !isdigit(str[i]) && str[i] != 'B') ++i;
		if (i == len) throw "Z数据";
		if (str[i] != 'B') sscanf(str.c_str() + i, "%u", &_z);
		else _z = 0;

		while (i < len && str[i] != 'G') ++i;
		if (i == len) throw "G数据";
		sscanf(str.c_str() + ++i, "%c", &_g);
		if(toupper(_g) != 'X' && toupper(_g) != 'Y')
			throw "G数据";

		while (i < len && !isalpha(str[++i]));
		if (i == len) throw "";
		if (str[i] == 'L')
			_lineType = LineType::L;
		else if (toupper(str[i++]) == 'S')
			_lineType = LineType::SR;
		else if (toupper(str[i - 1]) == 'N')
			_lineType = LineType::NR;
		else throw "划线方式";
		while (i < len && !isdigit(str[i]) && str[i] != 'B') ++i;
		if (i == len) throw "象限方向缺失";
		if (str[i] != 'B') sscanf(str.c_str() + i, "%u", &_dirc);
		else throw "象限方向缺失";
		if(_dirc <= 0 || _dirc > 4)
			throw "象限方向数字";
	}
	catch (const char* str) {
		printf(str);
		puts("格式错误！");
		throw;
	}
	catch (...) {
		puts("3B代码格式错误！");
		system("pause");
		exit(0);
	}
}

double Point2Angle(const Point2f& point, const Point2f& center) {
	return atan2(point.x - center.x, point.y - center.y);
}
Point2f Angle2Point(double angle, const Point2f& center, double dis) {
	return center + Point2f(dis * sin(angle), dis * cos(angle));
}

std::vector<cv::Point2f> Code_3B_line::GetPoints(const cv::Point2f& startPoint) {
	std::vector<cv::Point2f> res;
	if (_isEnd) return res;

	int x, y;
	switch (_dirc) {
	case 1: x =  (signed)_x, y = -(signed)_y; break;
	case 2: x = -(signed)_x, y = -(signed)_y; break;
	case 3: x = -(signed)_x, y =  (signed)_y; break;
	case 4: x =  (signed)_x, y =  (signed)_y; break;
	default: puts("象限错误！"); system("pause"); exit(0);
	}

	Point2f center = startPoint - Point2f(x, y), pre = startPoint;
	double nowAngle = Point2Angle(startPoint, center);
	double goSum = 0.0, d = sqrt(x * x + y * y);
	switch (_lineType) {
	case L: res.push_back(startPoint + Point2f(x, y)); break;
	case SR: case NR:
		while (goSum < _z) {
			nowAngle += _lineType == LineType::SR ? -0.001 : 0.001;
			//if (nowAngle < -M_PI) nowAngle += M_PI * 2;
			//if (nowAngle >  M_PI) nowAngle -= M_PI * 2;
			Point2f nxt = Angle2Point(nowAngle, center, d);
			if (toupper(_g) == 'X') goSum += fabs(nxt.x - pre.x);
			else goSum += fabs(nxt.y - pre.y);
			res.push_back(nxt);
			pre = nxt;
		}
		break;
	default: puts("划线方式错误！"); system("pause"); exit(0);
	}
	return res;
}

Code_3B::Code_3B(std::istream& fin) {
	string tmp;
	char c;
	bool newLine = false;
	while (!fin.eof()) {
		try {
			fin.read(&c, 1);
			if (isalpha(c)) {
				c = toupper(c);
				if (newLine && c == 'B') {
					_lines.push_back(Code_3B_line(tmp));
					tmp.clear();
					newLine = false;
				}
				else if (!newLine && c == 'G')
					newLine = true;
				else if (c == 'E') {
					_lines.push_back(Code_3B_line(tmp));
					tmp.clear();
					newLine = true;
				}
			}
			tmp.push_back(c);
		}
		catch (...) {
			puts("文件读取错误！");
			printf("错误位置-第%u条命令\n", _lines.size() + 1);
			system("pause");
			exit(0);
		}
	}
	if(!tmp.empty())
		_lines.push_back(Code_3B_line(tmp));
}

Code_3B::Code_3B(const std::vector<std::string> lines) {
	for (auto& line : lines) {
		_lines.push_back(Code_3B_line(line));
	}
}

std::vector<cv::Point> Code_3B::GetPoints(int width, int height) {
	vector<Point> res;
	// 获取点信息
	res.push_back(Point2f()); // 默认起点
	for (auto& line : _lines) {
		vector<Point2f> tmp = move(line.GetPoints(*(res.end() - 1)));
		if (!tmp.empty())
			res.insert(res.end(), tmp.begin(), tmp.end());
	}
	if (res.size() <= 1) {
		puts("无有效的3B代码行！");
		system("pause");
		exit(0);
	}
	// 调整点位置
	Point& startPoint = *res.begin();
	int minx = startPoint.x, maxx = startPoint.x,
		miny = startPoint.y, maxy = startPoint.y;
	for (auto& point : res) {
		if (minx > point.x) minx = point.x;
		if (maxx < point.x) maxx = point.x;
		if (miny > point.y) miny = point.y;
		if (maxy < point.y) maxy = point.y;
	}
	int nowWidth = maxx - minx, nowHeight = maxy - miny;
	// 确定缩放比例
	double ratio = min((double)(width - 5) / nowWidth, (double)(height - 5) / nowHeight);
	// 居中显示确定偏移量
	Point offset(-minx + ((width - nowWidth * ratio) / 2 / ratio),
					-miny + ((height - nowHeight * ratio) / 2 / ratio));
	for (auto& point : res)
		point = (point + offset) * ratio;
	return res;
}
