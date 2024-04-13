static int __maybe_unused timespec_to_ms(struct timespec *ts)
{
	return ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
}