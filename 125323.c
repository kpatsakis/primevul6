static void php_image_filter_grayscale(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageGrayScale(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}