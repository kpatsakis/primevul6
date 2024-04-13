resolveSymlink(const StaticString &path) {
	char buf[PATH_MAX];
	ssize_t size;
	
	size = readlink(path.c_str(), buf, sizeof(buf) - 1);
	if (size == -1) {
		if (errno == EINVAL) {
			return path;
		} else {
			int e = errno;
			string message = "Cannot resolve possible symlink '";
			message.append(path.c_str(), path.size());
			message.append("'");
			throw FileSystemException(message, e, path);
		}
	} else {
		buf[size] = '\0';
		if (buf[0] == '\0') {
			string message = "The file '";
			message.append(path.c_str(), path.size());
			message.append("' is a symlink, and it refers to an empty filename. This is not allowed.");
			throw FileSystemException(message, ENOENT, path);
		} else if (buf[0] == '/') {
			// Symlink points to an absolute path.
			return buf;
		} else {
			return extractDirName(path) + "/" + buf;
		}
	}
}