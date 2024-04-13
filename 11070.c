gmtime_r(const time_t *t, struct tm *tm)
{
    auto struct tm *tmp = gmtime(t);
    if (tmp)
	*tm = *tmp;
    return tmp;
}