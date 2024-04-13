static void php_ifd_set16u(char *data, unsigned int value, int motorola_intel)
{
	if (motorola_intel) {
		data[0] = (value & 0xFF00) >> 8;
		data[1] = (value & 0x00FF);
	} else {
		data[1] = (value & 0xFF00) >> 8;
		data[0] = (value & 0x00FF);
	}
}