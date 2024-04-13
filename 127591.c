makeDirTree(const string &path, const StaticString &mode, uid_t owner, gid_t group) {
	struct stat buf;
	vector<string> paths;
	vector<string>::reverse_iterator rit;
	string current = path;
	mode_t modeBits;
	int ret;
	
	if (stat(path.c_str(), &buf) == 0) {
		return;
	}
	
	modeBits = parseModeString(mode);
	
	/* Create a list of parent paths that don't exist. For example, given
	 * path == "/a/b/c/d/e" and that only /a exists, the list will become
	 * as follows:
	 *
	 * /a/b/c/d
	 * /a/b/c
	 * /a/b
	 */
	while (current != "/" && current != "." && getFileType(current) == FT_NONEXISTANT) {
		paths.push_back(current);
		current = extractDirName(current);
	}
	
	/* Now traverse the list in reverse order and create directories that don't exist. */
	for (rit = paths.rbegin(); rit != paths.rend(); rit++) {
		current = *rit;
		
		do {
			ret = mkdir(current.c_str(), modeBits);
		} while (ret == -1 && errno == EINTR);
		if (ret == -1) {
			if (errno == EEXIST) {
				// Ignore error and don't chmod/chown.
				continue;
			} else {
				int e = errno;
				throw FileSystemException("Cannot create directory '" + current + "'",
					e, current);
			}
		}
		
		/* Chmod in order to override the umask. */
		do {
			ret = chmod(current.c_str(), modeBits);
		} while (ret == -1 && errno == EINTR);
		
		if (owner != USER_NOT_GIVEN && group != GROUP_NOT_GIVEN) {
			if (owner == USER_NOT_GIVEN) {
				owner = (uid_t) -1; // Don't let chown change file owner.
			}
			if (group == GROUP_NOT_GIVEN) {
				group = (gid_t) -1; // Don't let chown change file group.
			}
			do {
				ret = chown(current.c_str(), owner, group);
			} while (ret == -1 && errno == EINTR);
			if (ret == -1) {
				char message[1024];
				int e = errno;
				
				snprintf(message, sizeof(message) - 1,
					"Cannot change the directory '%s' its UID to %lld and GID to %lld",
					current.c_str(), (long long) owner, (long long) group);
				message[sizeof(message) - 1] = '\0';
				throw FileSystemException(message, e, path);
			}
		}
	}
}