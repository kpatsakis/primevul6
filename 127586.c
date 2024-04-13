extractDirName(const StaticString &path) {
	char *path_copy = strdup(path.c_str());
	char *result = dirname(path_copy);
	string result_string(result);
	free(path_copy);
	return result_string;
}