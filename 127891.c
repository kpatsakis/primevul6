static int php_ifd_get32s(void *value, int motorola_intel)
{
	if (motorola_intel) {
		return  (((char  *)value)[0] << 24)
			  | (((uchar *)value)[1] << 16)
			  | (((uchar *)value)[2] << 8 )
			  | (((uchar *)value)[3]      );
	} else {
		return  (((char  *)value)[3] << 24)
			  | (((uchar *)value)[2] << 16)
			  | (((uchar *)value)[1] << 8 )
			  | (((uchar *)value)[0]      );
	}
}