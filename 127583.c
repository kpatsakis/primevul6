	GenerationPtr getNewestGeneration() const {
		DIR *dir = opendir(path.c_str());
		struct dirent *entry;
		int result = -1;
		
		if (dir == NULL) {
			int e = errno;
			throw FileSystemException("Cannot open directory " + path,
				e, path);
		}
		while ((entry = readdir(dir)) != NULL) {
			if (isDirectory(path, entry)
			 && strncmp(entry->d_name, "generation-", sizeof("generation-") - 1) == 0) {
				const char *numberString = entry->d_name + sizeof("generation-") - 1;
				int number = atoi(numberString);
				if (number >= 0 && number > result) {
					result = number;
				}
			}
		}
		closedir(dir);
		
		if (result == -1) {
			return GenerationPtr();
		} else {
			return getGeneration(result);
		}
	}