static inline zend_long parse_iv(const unsigned char *p)
{
	return parse_iv2(p, NULL);
}