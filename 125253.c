PHP_FUNCTION(imagecreatefromwebp)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_WEBP, "WEBP", gdImageCreateFromWebp, gdImageCreateFromWebpCtx);
}