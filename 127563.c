createFile(const string &filename, const StaticString &contents, mode_t permissions, uid_t owner,
	gid_t group, bool overwrite)
{
	FileDescriptor fd;
	int ret, e, options;
	
	options = O_WRONLY | O_CREAT | O_TRUNC;
	if (!overwrite) {
		options |= O_EXCL;
	}
	do {
		fd = open(filename.c_str(), options, permissions);
	} while (fd == -1 && errno == EINTR);
	if (fd != -1) {
		FileGuard guard(filename);
		
		// The file permission may not be as expected because of the active
		// umask, so fchmod() it here to ensure correct permissions.
		do {
			ret = fchmod(fd, permissions);
		} while (ret == -1 && errno == EINTR);
		if (ret == -1) {
			e = errno;
			throw FileSystemException("Cannot set permissions on " + filename,
				e, filename);
		}
		
		if (owner != USER_NOT_GIVEN && group != GROUP_NOT_GIVEN) {
			if (owner == USER_NOT_GIVEN) {
				owner = (uid_t) -1; // Don't let fchown change file owner.
			}
			if (group == GROUP_NOT_GIVEN) {
				group = (gid_t) -1; // Don't let fchown change file group.
			}
			do {
				ret = fchown(fd, owner, group);
			} while (ret == -1 && errno == EINTR);
			if (ret == -1) {
				e = errno;
				throw FileSystemException("Cannot set ownership for " + filename,
					e, filename);
			}
		}
		
		try {
			writeExact(fd, contents);
			fd.close();
		} catch (const SystemException &e) {
			throw FileSystemException("Cannot write to file " + filename,
				e.code(), filename);
		}
		guard.commit();
	} else {
		e = errno;
		if (overwrite || e != EEXIST) {
			throw FileSystemException("Cannot create file " + filename,
				e, filename);
		}
	}
}