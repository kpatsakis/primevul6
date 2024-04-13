getFileType(const StaticString &filename, CachedFileStat *cstat, unsigned int throttleRate) {
	struct stat buf;
	int ret;
	
	if (cstat != NULL) {
		ret = cstat->stat(filename, &buf, throttleRate);
	} else {
		ret = stat(filename.c_str(), &buf);
	}
	if (ret == 0) {
		if (S_ISREG(buf.st_mode)) {
			return FT_REGULAR;
		} else if (S_ISDIR(buf.st_mode)) {
			return FT_DIRECTORY;
		} else {
			return FT_OTHER;
		}
	} else {
		if (errno == ENOENT) {
			return FT_NONEXISTANT;
		} else {
			int e = errno;
			string message("Cannot stat '");
			message.append(filename);
			message.append("'");
			throw FileSystemException(message, e, filename);
		}
	}
}