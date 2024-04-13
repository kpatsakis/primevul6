PHP_FUNCTION(imagettfbbox)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_BBOX, 0);
}