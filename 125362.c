PHP_FUNCTION(imagecreatefromjpeg)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_JPG, "JPEG", gdImageCreateFromJpeg, gdImageCreateFromJpegCtx);
}