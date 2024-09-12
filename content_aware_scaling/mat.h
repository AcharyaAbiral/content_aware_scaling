#pragma once
#include<cstdint>
class Mat
{
private:
	int row;
	int col;
	int channels;
	int stride;
	float* data;

public:
	Mat(int h, int w, int s, int c);

	int get_height();
	int get_width();


	float* at(int i, int j);
	void fill(unsigned char* values);
	
	void remove_seam_pixel(int row, int start);
	
	void gen_img_data(unsigned char* values);
	void update_width();
	void normalize();
	


	void save_as_png(const char* file_name);
	void img_as_png(const char* file_name);

	friend void img_to_lum(Mat& img, Mat& lum);
	friend void lum_to_img(Mat& lum, Mat& img);

	friend void fill_dp_mat(Mat& edge, Mat& dp);

	friend void sobel_operation(Mat& img, int s_x[3][3], int s_y[3][3], Mat& res);
	~Mat();

};

void img_to_lum(Mat& img, Mat& lum);

void lum_to_img(Mat& lum, Mat& img);


void fill_dp_mat(Mat& edge, Mat& dp);
void sobel_operation(Mat& img, int s_x[3][3], int s_y[3][3], Mat& res);