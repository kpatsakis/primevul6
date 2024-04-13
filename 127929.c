static unsigned php_ifd_get32u(void *value, int motorola_intel)
{
	return (unsigned)php_ifd_get32s(value, motorola_intel) & 0xffffffff;
}