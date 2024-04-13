extractBaseName(const StaticString &path) {
	char *path_copy = strdup(path.c_str());
	string result_string = basename(path_copy);
	free(path_copy);
	return result_string;
}