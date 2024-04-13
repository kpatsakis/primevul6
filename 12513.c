cdf_read(const cdf_info_t *info, off_t off, void *buf, size_t len)
{
	size_t siz = (size_t)off + len;

	if ((off_t)(off + len) != (off_t)siz) {
		errno = EINVAL;
		return -1;
	}

	if (info->i_buf != NULL && info->i_len >= siz) {
		(void)memcpy(buf, &info->i_buf[off], len);
		return (ssize_t)len;
	}

	if (info->i_fd == -1)
		return -1;

	if (FINFO_LSEEK_FUNC(info->i_fd, off, SEEK_SET) == (off_t)-1)
		return -1;

	if (FINFO_READ_FUNC(info->i_fd, buf, len) != (ssize_t)len)
		return -1;

	return (ssize_t)len;
}