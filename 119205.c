bool time_more(struct timeval *a, struct timeval *b)
{
	return timercmp(a, b, >);
}