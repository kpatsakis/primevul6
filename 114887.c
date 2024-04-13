void notifier_read(notifier_t fd)
{
	char buf[0x10];
#ifdef WIN32
	(void)recv(fd[0], buf, sizeof(buf), 0);
#else
	(void)read(fd[0], buf, sizeof(buf));
#endif
}