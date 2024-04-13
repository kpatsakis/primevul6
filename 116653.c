static int fsmOpenat(int dirfd, const char *path, int flags)
{
    struct stat lsb, sb;
    int sflags = flags | O_NOFOLLOW;
    int fd = openat(dirfd, path, sflags);

    /*
     * Only ever follow symlinks by root or target owner. Since we can't
     * open the symlink itself, the order matters: we stat the link *after*
     * opening the target, and if the link ownership changed between the calls
     * it could've only been the link owner or root.
     */
    if (fd < 0 && errno == ELOOP && flags != sflags) {
	int ffd = openat(dirfd, path, flags);
	if (ffd >= 0 && fstatat(dirfd, path, &lsb, AT_SYMLINK_NOFOLLOW) == 0) {
	    if (fstat(ffd, &sb) == 0) {
		if (lsb.st_uid == 0 || lsb.st_uid == sb.st_uid) {
		    fd = ffd;
		} else {
		    close(ffd);
		}
	    }
	}
    }
    return fd;
}