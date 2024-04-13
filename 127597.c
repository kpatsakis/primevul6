getSystemTempDir() {
	const char *temp_dir = getenv("PASSENGER_TEMP_DIR");
	if (temp_dir == NULL || *temp_dir == '\0') {
		temp_dir = getenv("PASSENGER_TMPDIR");
		if (temp_dir == NULL || *temp_dir == '\0') {
			temp_dir = "/tmp";
		}
	}
	return temp_dir;
}