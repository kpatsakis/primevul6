static void recurse_dirlist(char *indir, char **dirlist, int *entries)
{
	char fname[MAX_PATH_LEN];
	struct stat istat;
	struct dirent *dp;
	DIR *dirp;

	dirp = opendir(indir);
	if (unlikely(!dirp))
		failure("Unable to open directory %s\n", indir);
	while ((dp = readdir(dirp)) != NULL) {
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
			continue;
		sprintf(fname, "%s/%s", indir, dp->d_name);
		if (unlikely(stat(fname, &istat)))
			failure("Unable to stat file %s\n", fname);
		if (S_ISDIR(istat.st_mode)) {
			recurse_dirlist(fname, dirlist, entries);
			continue;
		}
		if (!S_ISREG(istat.st_mode)) {
			print_err("Not regular file %s\n", fname);
			continue;
		}
		print_maxverbose("Added file %s\n", fname);
		*dirlist = realloc(*dirlist, MAX_PATH_LEN * (*entries + 1));
		strcpy(*dirlist + MAX_PATH_LEN * (*entries)++, fname);
	}
	closedir(dirp);
}