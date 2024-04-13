absolutizePath(const StaticString &path, const StaticString &workingDir) {
	vector<string> components;
	if (!startsWith(path, "/")) {
		if (workingDir.empty()) {
			char buffer[PATH_MAX];
			if (getcwd(buffer, sizeof(buffer)) == NULL) {
				int e = errno;
				throw SystemException("Unable to query current working directory", e);
			}
			split(buffer + 1, '/', components);
		} else {
			string absoluteWorkingDir = absolutizePath(workingDir);
			split(StaticString(absoluteWorkingDir.data() + 1, absoluteWorkingDir.size() - 1),
				'/', components);
		}
	}

	const char *begin = path.data();
	const char *end = path.data() + path.size();

	// Skip leading slashes.
	while (begin < end && *begin == '/') {
		begin++;
	}

	while (begin < end) {
		const char *next = (const char *) memchr(begin, '/', end - begin);
		if (next == NULL) {
			next = end;
		}

		StaticString component(begin, next - begin);
		if (component == "..") {
			if (!components.empty()) {
				components.pop_back();
			}
		} else if (component != ".") {
			components.push_back(component);
		}

		// Skip slashes until beginning of next path component.
		begin = next + 1;
		while (begin < end && *begin == '/') {
			begin++;
		}
	}

	string result;
	vector<string>::const_iterator c_it, c_end = components.end();
	for (c_it = components.begin(); c_it != c_end; c_it++) {
		result.append("/");
		result.append(*c_it);
	}
	if (result.empty()) {
		result = "/";
	}
	return result;
}