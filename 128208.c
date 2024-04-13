nlist(const char *name, struct nlist *list)
{
	int fd, n;

	if (list == NULL) {
		errno = EINVAL;
		return (-1);
	}
	fd = open(name, O_RDONLY, 0);
	if (fd < 0)
		return (-1);
	n = __fdnlist(fd, list);
	(void)close(fd);
	return (n);
}