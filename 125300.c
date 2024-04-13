PHP_FUNCTION(imagecreatefromgd2part)
{
	_php_image_create_from(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_GDIMG_TYPE_GD2PART, "GD2", gdImageCreateFromGd2Part, gdImageCreateFromGd2PartCtx);
}