extractDirNameStatic(const StaticString &path) {
	if (path.empty()) {
		return StaticString(".", 1);
	}

	const char *data = path.data();
	const char *end = path.data() + path.size();

	// Ignore trailing '/' characters.
	while (end > data && end[-1] == '/') {
		end--;
	}
	if (end == data) {
		// Apparently the entire path consists of slashes.
		return StaticString("/", 1);
	}

	// Find last '/'.
	end--;
	while (end > data && *end != '/') {
		end--;
	}
	if (end == data) {
		if (*data == '/') {
			// '/' found, but it's the first character in the path.
			return StaticString("/", 1);
		} else {
			// No '/' found in path.
			return StaticString(".", 1);
		}
	} else {
		// '/' found and it's not the first character in path.
		// 'end' points to that '/' character.
		// Skip to first non-'/' character.
		while (end >= data && *end == '/') {
			end--;
		}
		if (end < data) {
			// The entire path consists of '/' characters.
			return StaticString("/", 1);
		} else {
			return StaticString(data, end - data + 1);
		}
	}
}