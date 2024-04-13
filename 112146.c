TouchSocketFiles(void)
{
	ListCell   *l;

	/* Loop through all created sockets... */
	foreach(l, sock_paths)
	{
		char	   *sock_path = (char *) lfirst(l);

		/* Ignore errors; there's no point in complaining */
		(void) utime(sock_path, NULL);
	}
}