
static ssize_t bql_set(const char *buf, const size_t count,
		       unsigned int *pvalue)
{
	unsigned int value;
	int err;

	if (!strcmp(buf, "max") || !strcmp(buf, "max\n")) {
		value = DQL_MAX_LIMIT;
	} else {
		err = kstrtouint(buf, 10, &value);
		if (err < 0)
			return err;
		if (value > DQL_MAX_LIMIT)
			return -EINVAL;
	}

	*pvalue = value;

	return count;