static void php_image_filter_emboss(INTERNAL_FUNCTION_PARAMETERS)
{
	PHP_GD_SINGLE_RES

	if (gdImageEmboss(im_src) == 1) {
		RETURN_TRUE;
	}

	RETURN_FALSE;
}