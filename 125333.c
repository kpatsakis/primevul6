PHP_FUNCTION(image2wbmp)
{
	_php_image_output(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_CONVERT_WBM, "WBMP", _php_image_bw_convert);
}