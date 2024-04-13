getGroupName(gid_t gid) {
	struct group *groupEntry;

	groupEntry = getgrgid(gid);
	if (groupEntry == NULL) {
		return toString(gid);
	} else {
		return groupEntry->gr_name;
	}
}