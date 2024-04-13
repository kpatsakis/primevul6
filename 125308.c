PHP_FUNCTION(imagexbm)
{
	_php_image_output_ctx(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XBM, "XBM", gdImageXbmCtx);
}