void cgtime(struct timeval *tv)
{
	lldiv_t lidiv;

	decius_time(&lidiv);
	tv->tv_sec = lidiv.quot;
	tv->tv_usec = lidiv.rem / 10;
}