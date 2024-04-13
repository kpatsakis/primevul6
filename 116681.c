static void fsmDebug(const char *fpath, rpmFileAction action,
		     const struct stat *st)
{
    rpmlog(RPMLOG_DEBUG, "%-10s %06o%3d (%4d,%4d)%6d %s\n",
	   fileActionString(action), (int)st->st_mode,
	   (int)st->st_nlink, (int)st->st_uid,
	   (int)st->st_gid, (int)st->st_size,
	    (fpath ? fpath : ""));
}