void notifier_wake(notifier_t fd)
{
	if (fd[1] == INVSOCK)
		return;
#ifdef WIN32
	(void)send(fd[1], "\0", 1, 0);
#else
	(void)write(fd[1], "\0", 1);
#endif
}