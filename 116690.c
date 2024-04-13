static int update_info_refs(int force)
{
	char *path0 = git_pathdup("info/refs");
	int len = strlen(path0);
	char *path1 = xmalloc(len + 2);

	strcpy(path1, path0);
	strcpy(path1 + len, "+");

	safe_create_leading_directories(path0);
	info_ref_fp = fopen(path1, "w");
	if (!info_ref_fp)
		return error("unable to update %s", path1);
	for_each_ref(add_info_ref, NULL);
	fclose(info_ref_fp);
	adjust_shared_perm(path1);
	rename(path1, path0);
	free(path0);
	free(path1);
	return 0;
}