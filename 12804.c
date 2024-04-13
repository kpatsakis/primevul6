static int smack_flags_to_may(int flags)
{
	int may = 0;

	if (flags & S_IRUGO)
		may |= MAY_READ;
	if (flags & S_IWUGO)
		may |= MAY_WRITE;
	if (flags & S_IXUGO)
		may |= MAY_EXEC;

	return may;
}