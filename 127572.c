disableMallocDebugging() {
	unsetenv("MALLOC_FILL_SPACE");
	unsetenv("MALLOC_PROTECT_BEFORE");
	unsetenv("MallocGuardEdges");
	unsetenv("MallocScribble");
	unsetenv("MallocPreScribble");
	unsetenv("MallocCheckHeapStart");
	unsetenv("MallocCheckHeapEach");
	unsetenv("MallocCheckHeapAbort");
	unsetenv("MallocBadFreeAbort");
	unsetenv("MALLOC_CHECK_");

	const char *libs = getenv("DYLD_INSERT_LIBRARIES");
	if (libs != NULL && strstr(libs, "/usr/lib/libgmalloc.dylib")) {
		string newLibs = libs;
		string::size_type pos = newLibs.find("/usr/lib/libgmalloc.dylib");
		size_t len = strlen("/usr/lib/libgmalloc.dylib");

		// Erase all leading ':' too.
		while (pos > 0 && newLibs[pos - 1] == ':') {
			pos--;
			len++;
		}
		// Erase all trailing ':' too.
		while (pos + len < newLibs.size() && newLibs[pos + len] == ':') {
			len++;
		}

		newLibs.erase(pos, len);
		if (newLibs.empty()) {
			unsetenv("DYLD_INSERT_LIBRARIES");
		} else {
			setenv("DYLD_INSERT_LIBRARIES", newLibs.c_str(), 1);
		}
	}
}