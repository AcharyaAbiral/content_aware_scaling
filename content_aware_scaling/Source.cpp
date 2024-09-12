#include<iostream>
#include<stb_image.h>
#include<stb_image_write.h>
#include<cstdint>
#include<math.h>
#include "mat.h"


int main()
{
	int width, height;

	unsigned char* data = stbi_load("mkkp2.jpg", &width, &height, NULL, 4);

	if (!data)
	{
		std::cout << "could not load image" << std::endl;
		return 1;
	}
	std::cout << "done really" << std::endl;
	std::cout << "width=" << width << std::endl;
	std::cout << "height=" << height << std::endl;

	// compute luminance

	Mat img(height, width, 4 * width, 4);
	img.fill(data);

	img.img_as_png("image_copy.png");

	stbi_image_free(data);



	Mat lum(height, width, width, 1);

	img_to_lum(img, lum);


	lum.save_as_png("lum.png");
	std::cout << "lum written" << std::endl;

	//sobel convolution


	int sobel_x[3][3] = {
		{1,0,-1},
		{2,0,-2},
		{1,0,-1}
	};

	int sobel_y[3][3] =
	{
		{1,2,1},
		{0,0,0},
		{-1,-2,-1}
	};


	Mat edge_mat(height, width, width, 1);

	//sobel_operation(lum, sobel_x, sobel_y, edge_mat);

	sobel_operation(lum, sobel_x, sobel_y, edge_mat);

	edge_mat.save_as_png("edge.png");


	Mat dp_mat(height, width, width, 1);

	for (int tot_seams = 0; tot_seams < 700; tot_seams++)
	{
		height = lum.get_height();
		width = lum.get_width();
		std::cout << "removing seam " << tot_seams << std::endl;
		fill_dp_mat(edge_mat, dp_mat);

		//dp_mat.save_as_png("dp.png");


		int min_col = 0;
		for (int i = 1; i < width; i++)
		{
			if (*(dp_mat.at(height - 1, i)) < (*(dp_mat.at(height - 1, min_col))))
			{
				min_col = i;
			}
		}

		
		//*(lum.at(height - 1, min_col)) = 1;
		lum.remove_seam_pixel(height - 1, min_col);
		//std::cout << "removed from lum" << std::endl;
		edge_mat.remove_seam_pixel(height - 1, min_col);
		//std::cout << "removed from edge mat" << std::endl;
		img.remove_seam_pixel(height - 1, min_col);
		//dp_mat.remove_seam_pixel(height - 1, min_col);
		//std::cout << "removed from img" << std::endl;
		int min_cur;
		for (int r = height - 2; r >= 0; r--)
		{
			float min_val = FLT_MAX;
			for (int i = -1; i < 2; i++)
			{
				if (((min_col + i) < width) && ((min_col + i) >= 0))
				{
					if (*(dp_mat.at(r, min_col + i)) < min_val)
					{
						min_cur = min_col + i;
						min_val = *(dp_mat.at(r, min_cur));
					}
				}
			}
			min_col = min_cur;
			//*lum.at(r, min_col) = 1;
			lum.remove_seam_pixel(r, min_col);
			//std::cout << "removed from lum" << std::endl;
			img.remove_seam_pixel(r, min_col);
			//std::cout << "removed from img" << std::endl;
			edge_mat.remove_seam_pixel(r, min_col);
			//std::cout << "removed from edge" << std::endl;
			//dp_mat.remove_seam_pixel(r, min_col);
		}

		lum.update_width();
		//lum.save_as_png("seam.png");

		img.update_width();
		//img.img_as_png("scaled.png");

		edge_mat.update_width();

		dp_mat.update_width();

	}

	img.img_as_png("mkkp2_scaled.png");

	//std::cout << "hello" << std::endl;

	return 0;
}