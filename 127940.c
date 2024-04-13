static int php_jpg_get16(void *value)
{
	return (((uchar *)value)[0] << 8) | ((uchar *)value)[1];
}