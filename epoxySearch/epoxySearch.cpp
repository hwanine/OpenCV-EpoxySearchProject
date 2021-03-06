
#include "stdafx.h"
#include<opencv2/opencv.hpp>
#include<math.h>

using namespace cv;
using namespace std;

typedef struct image {           // 영상을 둘로 나눌 때, 반환하기위한 결과 값 구조체
	Mat leftimg;
	Mat rightimg;
}resultimg;

Mat process(Mat);				// 불순물 검출 
Mat resultGrid(Mat, Mat, float, float);			// 그리드 적용 및 결과 출력
resultimg cut(Mat);			// 1차 잘라내기
resultimg cut2(Mat);		// 2차 잘라내기

						
							// 결과로 에폭시 검출 분포를 격자무늬로 출력하는 함수
Mat resultGrid(Mat result, Mat result2, float divisionX, float divisionY) { //파라매터로 백터값을(그리드마다 에폭시 값을 저장하고 있는) 넘겨주어 
																			// 에폭시 값에 따라 색깔을 결정해보자
	Mat result3(result.rows, result.cols, CV_8UC3, Scalar(255, 255, 255));
	float max = 0;
	float max2 = 0;
	int x, y;

	for (float i = 0; i < result.rows; i = i + divisionY) {

		for (float j = 0; j < result.cols; j = j + divisionX) {

			for (y = i; y < i + divisionY; y++) {				// 그리드 내에서 화소값 조사

				for (x = j; x < j + divisionX; x++) {


					max = max + (int)result.at<Vec3b>(y, x)[0];			// 왼쪽 영상 화소값을 다 가져옴
					max2 = max2 + (int)result2.at <Vec3b>(y, x)[0];			// 우측 영상 화소값을 다 가져옴


				}

			}

			if (abs(max - max2) / (max + max2) < 0.1) {					// 화소값 유사도 판별1
				rectangle(result3, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(100, 100, 0), FILLED);
			}
			else if (abs(max - max2) / (max + max2) > 0.9)				// 화소값 유사도 판별2
				rectangle(result3, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(100, 50, 0), FILLED);
			else														// 화소값 유사도 판별3
				rectangle(result3, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(0, 0, 0), 1);
			max = 0;
			max2 = 0;
		}

	}
	return result3;
}


// 결과로 에폭시 검출 분포를 격자무늬로 출력하는 함수
Mat resultGrid2(Mat result, float divisionX, float divisionY) { //파라매터로 백터값을(그리드마다 에폭시 값을 저장하고 있는) 넘겨주어 
																// 에폭시 값에 따라 색깔을 결정해보자
	Mat result3(result.rows, result.cols, CV_8UC3, Scalar(255, 255, 255));
	float max = 0;
	float max2 = 0;
	int x, y;


	for (float i = 0; i < result.rows; i = i + divisionY) {

		for (float j = 0; j < result.cols; j = j + divisionX) {

			for (y = i; y < i + divisionY; y++) {				// 그리드 내에서 화소값 조사

				for (x = j; x < j + divisionX; x++) {

					max = max + (int)result.at<Vec3b>(y, x)[0];			//  화소값을 다 가져옴

				}

			}

			if (max / (255 * divisionX) < 0.1) {					// 화소값 유사도 판별1
				rectangle(result3, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(255, 255, 255), FILLED);
			}
			else if (max / (255 * divisionX) > 0.9)				// 화소값 유사도 판별2
				rectangle(result3, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(180, 30, 0), FILLED);
			else														// 화소값 유사도 판별3
				rectangle(result3, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(250, 150, 0), FILLED);
			max = 0;

		}

	}
	return result3;
}

Mat gridWrite(Mat& dst, float divisionX, float divisionY) { // 그리드(격자무늬 생성함수)
															// 행렬의 가로 세로길이를 divisionX와 divisionY로 나눈다 
															// 0 + 나눈값 마다 포인트객체를 생성해 점과 점을 이어준다 
															//그렇게 격자 무늬를 많이 만드는게 목표
															// 질문1. 안 나눠 떨어지면 마지막 픽셀 크기가 다른 픽셀 크기랑 안맞을거 아니냐 
															// 질문2 행렬 x,y값 어 떻게 알아내더라 = size함수 써라
															//일단 픽셀은 생각안함 

	float div_x = dst.cols / divisionX; //열을 나눈 몫
	float div_y = dst.rows / divisionY; //행을 나눈 몫
										//행렬시작점 알아내봐라 
										/*
										Point zero(0, 0); //초기화 위치
										Point pt1(0, 0);  // 좌상단 맨 시작위치
										Point plusX(div_x, 0); //세로선을 배수적으로 만들기위해 위치 잡기위해 x값에 더하는 피연산자
										Point pt2(0, dst.rows); // 세로선을 귿기 위해 좌하단 꼭지점 위치
										Point pt3(dst.cols, 0); // 가로선을 귿기 위해 우상단 꼭지점 위치
										Point plusY(0, div_y); // 가로선을 배수적으로 만들기위해 y값에 더하는 피연산자
										*/


	Point zero(0, 0); //초기화 위치
	Point pt1(0, 0);  // 좌상단 맨 시작위치
	Point plusX(divisionX, 0); //세로선을 배수적으로 만들기위해 위치 잡기위해 x값에 더하는 피연산자
	Point pt2(0, dst.rows); // 세로선을 귿기 위해 좌하단 꼭지점 위치
	Point pt3(dst.cols, 0); // 가로선을 귿기 위해 우상단 꼭지점 위치
	Point plusY(0, divisionY); // 가로선을 배수적으로 만들기위해 y값에 더하는 피연산자

	for (float i = 0; i<dst.cols; i = i + divisionX) {  // 세로선 귿기 반복문
		line(dst, pt1 = pt1 + plusX, pt2 = pt2 + plusX, Scalar(0), 1, 8, 0);
	}
	pt1 = zero;
	for (float i = 0; i<dst.rows; i = i + divisionY) { // 가로선 귿기 반복문
		line(dst, pt1 = pt1 + plusY, pt3 = pt3 + plusY, Scalar(0), 1, 8, 0);
	}

	/*
	for (float i = 0; i < dst.cols; i = i + div_x) {  // 세로선 귿기 반복문
	line(dst, pt1 = pt1 + plusX, pt2 = pt2 + plusX, Scalar(0), 2, 8, 0);
	}
	pt1 = zero;
	for (float i = 0; i < dst.rows; i = i + div_y) { // 가로선 귿기 반복문
	line(dst, pt1 = pt1 + plusY, pt3 = pt3 + plusY, Scalar(0), 2, 8, 0);
	}
	*/
	return dst;
	//line(Mat& dst, Point pt1, Point pt2, const Scalar& color, int thickness = 1, int lineType = 8, int shift = 0);

}

// 영상 각각 사각형 검출하여 비율 출력
/*Mat resultGrid(Mat result, float divisionX, float divisionY) { //파라매터로 백터값을(그리드마다 에폭시 값을 저장하고 있는) 넘겨주어
// 에폭시 값에 따라 색깔을 결정해보자
Mat result2(result.rows, result.cols, CV_8UC3, Scalar(255, 255, 255));
int max = 0;
int x, y;

for (float i = 0; i < result.rows; i = i + divisionY) {

for (float j = 0; j < result.cols; j = j + divisionX) {

for (y = i; y < i + divisionY; y++) {

for (x = j; x < j + divisionX; x++) {

if ((int)result.at<Vec3b>(y, x)[0] > 0)
max++;

}

}

if (max > (divisionX * divisionY) * 9 / 10) {
rectangle(result2, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(200, 0, 0), FILLED);
}
else if (max < (divisionX * divisionY) * 1 / 10)
rectangle(result2, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(255, 0, 0), 1);
else
rectangle(result2, Point(j, i), Point(j + divisionX, i + divisionY), Scalar(100, 0, 0), FILLED);
max = 0;
}

}
return result2;
}*/


resultimg cut(Mat image) {
	int sum = 0;
	int y1 = 0, y2 = 0, y3 = 0;
	int x1 = 0, x2 = 0, x3 = 0;
	Mat canny;
	Mat grayimg;
	Mat drawing;
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	cvtColor(image, grayimg, 10);									// 1. 이미지를 그레이영상으로 변환
	Canny(grayimg, canny, 0, 300, 3);								// 2. 그레이 영상에 캐니 엣지 적용
	morphologyEx(canny, canny, MORPH_CROSS, kernel);				// 3. 엣지영상을 깔끔하게 하기위해 모폴로지 적용
	findContours(canny, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());				// 외곽선 검출 함수
	cvtColor(canny, drawing, COLOR_GRAY2BGR);

	for (int i = 0; i < contours.size(); i++) {							// 사각형 객체 검출 루프
		RotatedRect rect = minAreaRect(contours[i]);
		if ((rect.size.width / rect.size.height) >= 1.5 && (rect.size.width / rect.size.height) <= 3) {				// 해당 크기의 사각형만 검출해냄 
			drawContours(drawing, contours, i, Scalar(0, 255, 0), 2);												// 외곽선을 초록색으로 칠함
		}
	}



	for (int i = 0; i < drawing.rows; i = i + 1) {												// 상단 잘라내기
		for (int j = 0; j < canny.cols; j++) {													// 화소값을 읽어와서 초록색 화소값이 많은 구간을 체크. 오차를 줄이기위하여 i + 4 구간까지 화소 확인
			if ((int)drawing.at<Vec3b>(i, j)[1] >= 255 || (int)drawing.at<Vec3b>(i + 1, j)[1] >= 255 || (int)drawing.at<Vec3b>(i + 2, j)[1] >= 255 || (int)drawing.at<Vec3b>(i + 3, j)[1] >= 255 || (int)drawing.at<Vec3b>(i + 4, j)[1] >= 255)
				sum++;
			if (sum * 3 > drawing.cols) {		// 3배 많을시 잘라내기
				y1 = i + 4;
				break;
			}
		}
		sum = 0;
		if (y1 >= 1)
			break;
	}


	for (int i = drawing.rows - 1; i > 0; i = i - 1) {											// 하단 잘라내기
		for (int j = 0; j < drawing.cols; j++) {												// 화소값을 읽어와서 초록색 화소값이 많은 구간을 체크. 오차를 줄이기위하여 i - 4 구간까지 화소 확인
			if ((int)drawing.at<Vec3b>(i, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 1, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 2, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 3, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 4, j)[1] >= 255)
				sum++;
			if (sum * 3 > drawing.cols) {		// 3배 많을시 잘라내기
				y2 = i - 4;
				break;
			}
		}
		sum = 0;
		if (y2 >= 1) {
			break;
		}
	}

	y3 = y2 - y1;

	for (int i = 0; i < drawing.cols; i = i + 1) {												// 좌측 잘라내기
		for (int j = y1; j < y2; j++) {															// 화소값을 읽어와서 초록색 화소값이 많은 구간을 체크. 오차를 줄이기위하여 i + 3 구간까지 화소 확인
			if ((int)drawing.at<Vec3b>(j, i)[1] >= 255 || (int)drawing.at<Vec3b>(j, i + 1)[1] >= 250 || (int)drawing.at<Vec3b>(j, i + 2)[1] >= 255 || (int)drawing.at<Vec3b>(j, i + 3)[1] >= 255 || (int)drawing.at<Vec3b>(j, i + 4)[1] >= 255)
				sum++;
			if (sum * 2 > y3) {					// 2배 많을시 잘라내기
				x1 = i + 4;
				break;
			}
		}
		sum = 0;
		if (x1 >= 1)
			break;
	}


	for (int i = drawing.cols - 1; i > 0; i = i - 1) {											// 좌측 잘라내기
		for (int j = y1; j < y2; j++) {															// 화소값을 읽어와서 초록색 화소값이 많은 구간을 체크. 오차를 줄이기위하여 i - 4 구간까지 화소 확인
			if ((int)drawing.at<Vec3b>(j, i)[1] >= 255 || (int)drawing.at<Vec3b>(j, i - 1)[1] >= 255 || (int)drawing.at<Vec3b>(j, i - 2)[1] >= 255 || (int)drawing.at<Vec3b>(j, i - 3)[1] >= 255 || (int)drawing.at<Vec3b>(j, i - 4)[1] >= 255)
				sum++;
			if (sum * 2 > y3) {					// 2배 많을시 잘라내기
				x2 = i - 4;
				break;
			}
		}
		sum = 0;
		if (x2 >= 1)
			break;
	}

	x3 = x2 - x1;
	Rect rect(x1, y1, x3, y3);
	Mat result = image(rect);			// 잘라내기

	return cut2(result);				// 2차 잘라내기 함수 호출
}

resultimg cut2(Mat image) {
	int sum = 0;
	int y1 = 0;
	int x1 = 0, x2 = 0;
	Mat canny;
	Mat grayimg;
	Mat drawing;
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	cvtColor(image, grayimg, 10);
	Canny(grayimg, canny, 50, 250, 3);
	morphologyEx(canny, canny, MORPH_CROSS, kernel);
	findContours(canny, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point());
	cvtColor(canny, drawing, COLOR_GRAY2BGR);
	resultimg result;

	vector<Vec4i> lines;
	HoughLinesP(canny, lines, 1, CV_PI / 30, 30, 40, 8);

	//검출한 직선을 영상에 그려줌 
	for (int i = 0; i < lines.size(); i++)						// 직선 검출 알고리즘
	{
		Vec4i L = lines[i];
		line(drawing, Point(L[0], L[1]), Point(L[2], L[3]),
			Scalar(0, 255, 0), 1, LINE_AA);
	}

	for (int i = 4 * drawing.rows / 5; i > 0; i = i - 1) {
		for (int j = 0; j < drawing.cols; j++) {
			if ((int)drawing.at<Vec3b>(i, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 1, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 2, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 3, j)[1] >= 255 || (int)drawing.at<Vec3b>(i - 4, j)[1] >= 255)
				sum++;
			if (sum * 1.5 > drawing.cols) {
				y1 = i - 4;
				break;
			}
		}
		printf("%d, %d, %d\n", i, sum, drawing.cols);
		sum = 0;

		if (y1 >= 1) {
			break;
		}
	}

	for (int i = drawing.cols / 4; i < drawing.cols; i = i + 1) {
		for (int j = 0; j < y1; j++) {
			if ((int)drawing.at<Vec3b>(j, i)[1] >= 255 || (int)drawing.at<Vec3b>(j, i + 1)[1] >= 255 || (int)drawing.at<Vec3b>(j, i + 2)[1] >= 255)
				sum++;
			if (sum *1.2> y1) {
				x1 = i + 2;
				break;
			}
		}
		sum = 0;
		if (x1 >= 1)
			break;
	}


	for (int i = 3 * drawing.cols / 4; i > 0; i = i - 1) {
		for (int j = 0; j < y1; j++) {
			if ((int)drawing.at<Vec3b>(j, i)[1] >= 255 || (int)drawing.at<Vec3b>(j, i - 1)[1] >= 255 || (int)drawing.at<Vec3b>(j, i - 2)[1] >= 255)
				sum++;

			if (sum * 1.2 > y1) {
				x2 = i - 2;
				break;
			}
		}
		sum = 0;
		if (x2 >= 1)
			break;
	}

	Rect rect1(0, 0, x1, y1);
	Rect rect2(x2, 0, image.cols - x2, y1);
	result.leftimg = image(rect1);
	result.rightimg = image(rect2);
	resize(result.leftimg, result.leftimg, Size(800, 600), 0, 0);
	resize(result.rightimg, result.rightimg, Size(800, 600), 0, 0);				// 크기 조절
	return result;																// 구조체 결과 영상 반환
}

Mat process(Mat img) {
	Mat img2;
	cvtColor(img, img2, COLOR_BGR2HSV);				// BGR 컬러채널로 변환
	Mat bgr[3];
	split(img2, bgr);

	Mat mask1, mask2;
	double max = 150;			// 최대 화솟값
	double min = 100;			// 최소 화솟값
	threshold(bgr[0], mask1, max, 255, THRESH_BINARY_INV);       // 색상마스크
	threshold(bgr[0], mask2, min, 255, THRESH_BINARY);
	Mat hueMask;
	if (min < max) hueMask = mask1 & mask2;				// 최대와 최소 색상마스크 적용 결과를 합침.
	else hueMask = mask1 | mask2;

	double minSat = 15;				// 최소 채도값
	double maxSat = 255;			// 최대 최소값
	threshold(bgr[1], mask1, maxSat, 255, THRESH_BINARY_INV);   // 채도마스크
	threshold(bgr[1], mask2, minSat, 255, THRESH_BINARY);
	Mat satMask = mask1 & mask2;						// 최대와 최소 채도마스크 적용 결과를 합침.
	Mat result = hueMask & satMask;						// 색상마스크와 채도마스크를 합침 

	Mat result2(img2.size(), CV_8UC3, Scalar(0, 0, 0));
	img.copyTo(result2, result);						// 결과 검출 영상을 RGB 모델로 복사하여 반환
	return result2;



}

int main() {
	Mat image = imread("C:/Users/Owner/Desktop/two_faces.jpg", IMREAD_COLOR);
	Mat result(image.rows, image.cols, CV_8UC3, Scalar(255, 255, 255));//흰색화면에 image와 사이즈 동일;
	Mat result2(image.rows, image.cols, CV_8UC3, Scalar(255, 255, 255));
	CV_Assert(image.data);
	resize(image, image, Size(1400, 900), 0, 0);			// 영상 크기 조절

	resultimg cut_image = cut(image);			// 영상 잘라내기

	imshow("left", cut_image.leftimg);
	imshow("right", cut_image.rightimg);

	Mat pro = process(cut_image.leftimg);		// 불순물 검출 영상
	Mat pro2 = process(cut_image.rightimg);
	imshow("pro", pro);
	imshow("pro2", pro2);
	//result = resultGrid(pro, pro2, 20, 20);		// 그리드 적용
	//image = gridWrite(cut_image.leftimg, 20.0, 20.0);

	result = resultGrid2(pro, 20, 20);
	result2 = resultGrid2(pro2, 20, 20);
	result = gridWrite(result, 20, 20);
	result2 = gridWrite(result2, 20, 20);

	imshow("result1", result);
	imshow("result2", result2);
	waitKey(0);
	return 0;

}