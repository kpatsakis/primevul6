PHP_FUNCTION(imagefttext)
{
	php_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_DRAW, 1);
}