PHP_FUNCTION(imagewebp)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WEBP, "WEBP", gdImageWebpCtx);
}