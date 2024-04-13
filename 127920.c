static signed short php_ifd_get16s(void *value, int motorola_intel)
{
	return (signed short)php_ifd_get16u(value, motorola_intel);
}