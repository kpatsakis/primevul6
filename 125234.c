PHP_FUNCTION(imagecreatefrompng)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_PNG, "PNG", gdImageCreateFromPng, gdImageCreateFromPngCtx);
}