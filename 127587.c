		FileGuard(const string &filename) {
			this->filename = filename;
			committed = false;
		}