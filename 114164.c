BGD_DECLARE(void) gdImageSetResolution(gdImagePtr im, const unsigned int res_x, const unsigned int res_y)
{
	if (res_x > 0) im->res_x = res_x;
	if (res_y > 0) im->res_y = res_y;
}