closeAllFileDescriptors(int lastToKeepOpen) {
	#if defined(F_CLOSEM)
		int ret;
		do {
			ret = fcntl(lastToKeepOpen + 1, F_CLOSEM);
		} while (ret == -1 && errno == EINTR);
		if (ret != -1) {
			return;
		}
	#elif defined(HAS_CLOSEFROM)
		closefrom(lastToKeepOpen + 1);
		return;
	#endif
	
	for (int i = getHighestFileDescriptor(); i > lastToKeepOpen; i--) {
		/* Even though we normally shouldn't retry on EINTR
		 * (http://news.ycombinator.com/item?id=3363819)
		 * it's okay to do that here because because this function
		 * may only be called in a single-threaded environment.
		 */
		int ret;
		do {
			ret = close(i);
		} while (ret == -1 && errno == EINTR);
	}
}