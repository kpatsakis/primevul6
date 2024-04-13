pull16(unsigned char **cpp)
{
	short rval;

	rval = *(*cpp)++;
	rval <<= 8;
	rval |= *(*cpp)++;
	return rval;
}