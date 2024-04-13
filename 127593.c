fileExists(const StaticString &filename, CachedFileStat *cstat, unsigned int throttleRate) {
	return getFileType(filename, cstat, throttleRate) == FT_REGULAR;
}