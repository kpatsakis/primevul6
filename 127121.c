static int add_sdb_include_dir(Sdb *s, const char *incl, int idx) {
	if (!s || !incl)
		return false;
	return sdb_array_set (s, "includedirs", idx, incl, 0);
}