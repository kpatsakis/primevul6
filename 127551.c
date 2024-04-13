	bool isDirectory(const string &dir, struct dirent *entry) const {
		#ifdef DT_DIR
			if (entry->d_type == DT_DIR) {
				return true;
			} else if (entry->d_type != DT_UNKNOWN) {
				return false;
			}
			// If DT_UNKNOWN, use normal check.
		#endif
		string path = dir;
		path.append("/");
		path.append(entry->d_name);
		return getFileType(path) == FT_DIRECTORY;
	}