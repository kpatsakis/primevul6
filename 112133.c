getInstallationPaths(const char *argv0)
{
	DIR		   *pdir;

	/* Locate the postgres executable itself */
	if (find_my_exec(argv0, my_exec_path) < 0)
		ereport(FATAL,
				(errmsg("%s: could not locate my own executable path", argv0)));

#ifdef EXEC_BACKEND
	/* Locate executable backend before we change working directory */
	if (find_other_exec(argv0, "postgres", PG_BACKEND_VERSIONSTR,
						postgres_exec_path) < 0)
		ereport(FATAL,
				(errmsg("%s: could not locate matching postgres executable",
						argv0)));
#endif

	/*
	 * Locate the pkglib directory --- this has to be set early in case we try
	 * to load any modules from it in response to postgresql.conf entries.
	 */
	get_pkglib_path(my_exec_path, pkglib_path);

	/*
	 * Verify that there's a readable directory there; otherwise the Postgres
	 * installation is incomplete or corrupt.  (A typical cause of this
	 * failure is that the postgres executable has been moved or hardlinked to
	 * some directory that's not a sibling of the installation lib/
	 * directory.)
	 */
	pdir = AllocateDir(pkglib_path);
	if (pdir == NULL)
		ereport(ERROR,
				(errcode_for_file_access(),
				 errmsg("could not open directory \"%s\": %m",
						pkglib_path),
				 errhint("This may indicate an incomplete PostgreSQL installation, or that the file \"%s\" has been moved away from its proper location.",
						 my_exec_path)));
	FreeDir(pdir);

	/*
	 * XXX is it worth similarly checking the share/ directory?  If the lib/
	 * directory is there, then share/ probably is too.
	 */
}