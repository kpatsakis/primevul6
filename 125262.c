PHP_FUNCTION(imagecreatefromxpm)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_XPM, "XPM", gdImageCreateFromXpm, NULL);
}