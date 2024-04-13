		void destroy() {
			if (owner) {
				removeDirTree(path);
			}
		}