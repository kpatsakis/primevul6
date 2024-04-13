void notifier_destroy(notifier_t fd)
{
#ifdef WIN32
	closesocket(fd[0]);
	closesocket(fd[1]);
#else
	close(fd[0]);
	close(fd[1]);
#endif
	fd[0] = fd[1] = INVSOCK;
}