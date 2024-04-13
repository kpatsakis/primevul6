void subtime(struct timeval *a, struct timeval *b)
{
	timersub(a, b, b);
}