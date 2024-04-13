lookupGid(const string &groupName) {
	struct group *groupEntry;

	groupEntry = getgrnam(groupName.c_str());
	if (groupEntry == NULL) {
		if (looksLikePositiveNumber(groupName)) {
			return atoi(groupName);
		} else {
			return (gid_t) -1;
		}
	} else {
		return groupEntry->gr_gid;
	}
}