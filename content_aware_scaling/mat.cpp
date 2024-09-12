#include "mat.h"
#include<iostream>
#include<math.h>
#include<cassert>
#include<stb_image_write.h>

Mat::Mat(int h, int w, int s, int c)
{
	row = h;
	col = w;
	stride = s;
	channels = c;
	data = new float[channels * row * col];

}
int Mat::get_height()
{
	return row;
}

int Mat::get_width()
{
	return col;
}
float* Mat::at(int i, int j)
{
	return (data + i * stride + j);
}

void Mat::fill(unsigned char* values)
{
	for (int i = 0; i < channels * row * col; i++)
	{
		data[i] = (float)values[i];
	}
}

void Mat::remove_seam_pixel (int row, int start)
{
	//removing pixels along seam
	memmove(at(row, start*channels), at(row, channels*(start+1)), sizeof(float) * (col - start - 1)*channels);

}

void Mat::gen_img_data(unsigned char* values)
{
	for (int i = 0; i < row * col *channels; i++)
	{
		values[i] = (unsigned char)data[i];
	}
}

void Mat::update_width()
{
	col--;
}

void Mat::normalize()
{
	float min = FLT_MAX;
	float max = FLT_MIN;

	for (int i = 0; i < row * col * channels; i++)
	{
		if (data[i] > max)
		{
			max = data[i];
		}
		if(data[i]<min)
		{
			min = data[i];
		}
	}
	std::cout << "min=" << min<<std::endl;
	std::cout << "max=" << max<<std::endl;

	for (int i = 0; i < row * col * channels; i++)
	{
		data[i] = (data[i]-min) / (max-min);
	}
}


void Mat::save_as_png(const char* file_name)
{
	unsigned char* img_data = new unsigned char[4 * row * col];

	this->normalize();

	Mat img_mat(row, col, 4 * col, 4);

	lum_to_img(*this, img_mat);

	img_mat.gen_img_data(img_data);

	if (!stbi_write_png(file_name, col, row, 4, img_data, col * 4))
	{
		std::cout << "error while generating image" << std::endl;
	}

	delete[] img_data;
}

void Mat::img_as_png(const char* file_name)
{
	unsigned char* img_data = new unsigned char[4 * row * col];

	int index = 0;
	for (int r = 0; r < row; r++)
	{
		for (int c = 0; c < col; c++)
		{
			int index_r = r;
			int index_c = channels * c;
			for (int i = 0; i < channels; i++)
			{
				img_data[index++] = *at(index_r, index_c + i);

			}
		}
	}

	if (!stbi_write_png(file_name, col, row, 4, img_data, col * 4))
	{
		std::cout << "error while generating image" << std::endl;
	}

	delete[] img_data;
}


Mat::~Mat()
{

	delete[] data;

}

void img_to_lum(Mat& img, Mat& lum)
{

	assert(img.row == lum.row);
	assert(img.col == lum.col);
	assert(img.channels == 4);
	for (int i = 0; i < img.row; i++)
	{
		for (int j = 0; j < img.col; j++)
		{
			int index_i = i;
			int index_j = j * img.channels;
			float r = (*(img.at(index_i, index_j + 0))) / 255.0;
			float g = (*(img.at(index_i, index_j + 1))) / 255.0;
			float b = (*(img.at(index_i, index_j + 2))) / 255.0;

			*(lum.at(i, j)) = 0.2126 * r + 0.7152 * g + 0.0722 * b;

		}
	}
}

void lum_to_img(Mat& lum, Mat& img)
{
	assert(lum.row == img.row);
	assert(lum.col == img.col);
	assert(img.channels == 4);
	for (int i = 0; i < img.row; i++)
	{
		for (int j = 0; j < img.col; j++)
		{
			int index_i = i;
			int index_j = j * img.channels;
			
			float val = (*(lum.at(i, j))) * 255;

			*(img.at(index_i, index_j + 0)) = val;
			*(img.at(index_i, index_j + 1)) = val;
			*(img.at(index_i, index_j + 2)) = val;
			*(img.at(index_i, index_j + 3)) = 255;
			
		}
	}
}

void sobel_operation(Mat& img, int s_x[3][3], int s_y[3][3], Mat& res)
{

	assert(img.row == res.row);
	assert(img.col == res.col);
	assert(img.channels = res.channels);
	
	for (int i = 0; i < res.row; i++)
	{
		for (int j = 0; j < res.col; j++)
		{
			int cx = j;
			int cy = i;
			float sum_x = 0;
			float sum_y = 0;
							
			for (int y = -1; y < 2; y++)
			{
				for (int x = -1; x < 2; x++)
				{
					if ((cx + x >= 0 && cx + x < res.col) && (cy + y >= 0 && cy + y < res.row))
					{
						sum_x = sum_x + (*(img.at(y + cy, x + cx))) * (s_x[1 + y][1 + x]);
						sum_y = sum_y + (*(img.at(y + cy, x + cx))) * (s_y[1 + y][1 + x]);
					}

				}
			}

			float sum_res = sqrt(pow(sum_x,2)+pow(sum_y,2));
	
			*(res.at(cy, cx)) = sum_res;

		}
	}
}

float min(float a, float b, float c)
{
	if (a <= b && a <= c)
	{
		return a;
	}
	else if (b <= a && b <= c)
	{
		return b;
	}
	else
	{
		return c;
	}
}

void fill_dp_mat(Mat& edge, Mat& dp)
{

	assert(edge.row == dp.row);
	assert(edge.col == dp.col);
	assert(edge.channels == dp.channels);

	int row = edge.row;
	int col = edge.col;

	for (int i = 0; i < col; i++)
	{
		*(dp.at(0, i)) = *(edge.at(0, i));
	}

	for (int r = 1; r<row; r++)
	{

		for (int c = 0; c < col; c++)
		{
			float one = (c - 1) >= 0 ? (*(dp.at(r - 1, c - 1))) : FLT_MAX;
			float two = *(dp.at(r - 1, c));
			float three = (c + 1) <= (col - 1) ? ( * (dp.at(r - 1, c + 1))) : FLT_MAX;
			*(dp.at(r, c)) = *(edge.at(r, c)) + min(one, two, three);
		}

	}
}