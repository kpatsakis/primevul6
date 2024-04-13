PHP_FUNCTION(imagepng)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImagePngCtxEx);
}