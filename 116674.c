static int onChdir(rpmfi fi, void *data)
{
    struct diriter_s *di = data;

    if (di->dirfd >= 0) {
	close(di->dirfd);
	di->dirfd = -1;
    }
    return 0;
}