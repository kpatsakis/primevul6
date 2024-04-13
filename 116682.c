static int cap_set_fileat(int dirfd, const char *path, cap_t fcaps)
{
    int rc = -1;
    int fd = fsmOpenat(dirfd, path, O_RDONLY|O_NOFOLLOW);
    if (fd >= 0) {
	rc = cap_set_fd(fd, fcaps);
	close(fd);
    }
    return rc;
}