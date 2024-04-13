	void createDirectory(const string &path) const {
		// We do not use makeDirTree() here. If an attacker creates a directory
		// just before we do, then we want to abort because we want the directory
		// to have specific permissions.
		if (mkdir(path.c_str(), parseModeString("u=rwx,g=rx,o=rx")) == -1) {
			int e = errno;
			throw FileSystemException("Cannot create server instance directory '" +
				path + "'", e, path);
		}
		// Explicitly chmod the directory in case the umask is interfering.
		if (chmod(path.c_str(), parseModeString("u=rwx,g=rx,o=rx")) == -1) {
			int e = errno;
			throw FileSystemException("Cannot set permissions on server instance directory '" +
				path + "'", e, path);
		}
		// verifyDirectoryPermissions() checks for the owner/group so we must make
		// sure the server instance directory has that owner/group, even when the
		// parent directory has setgid on.
		if (chown(path.c_str(), geteuid(), getegid()) == -1) {
			int e = errno;
			throw FileSystemException("Cannot change the permissions of the server "
				"instance directory '" + path + "'", e, path);
		}
	}