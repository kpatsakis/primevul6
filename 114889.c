void RenameThread(const char* name)
{
#if defined(PR_SET_NAME)
	// Only the first 15 characters are used (16 - NUL terminator)
	prctl(PR_SET_NAME, name, 0, 0, 0);
#elif defined(__APPLE__)
	pthread_setname_np(name);
#elif (defined(__FreeBSD__) || defined(__OpenBSD__))
	pthread_set_name_np(pthread_self(), name);
#else
	// Prevent warnings for unused parameters...
	(void)name;
#endif
}