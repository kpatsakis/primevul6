getProcessUsername() {
	struct passwd pwd, *result;
	char strings[1024];
	int ret;
	
	result = (struct passwd *) NULL;
	do {
		ret = getpwuid_r(getuid(), &pwd, strings, sizeof(strings), &result);
	} while (ret == -1 && errno == EINTR);
	if (ret == -1) {
		result = (struct passwd *) NULL;
	}
	
	if (result == (struct passwd *) NULL || result->pw_name == NULL || result->pw_name[0] == '\0') {
		snprintf(strings, sizeof(strings), "UID %lld", (long long) getuid());
		strings[sizeof(strings) - 1] = '\0';
		return strings;
	} else {
		return result->pw_name;
	}
}