char *absolute_uri(char *uri, const char *ref)
{
	if (strstr(uri, "://"))
		return strdup(uri);

	char *copy_start, *abs;
	bool need_slash = false;

	copy_start = (uri[0] == '/') ? &uri[1] : uri;
	if (ref[strlen(ref) - 1] != '/')
		need_slash = true;

	abs = malloc(strlen(ref) + strlen(copy_start) + 2);
	if (!abs) {
		applog(LOG_ERR, "Malloc failure in absolute_uri");
		return NULL;
	}

	sprintf(abs, "%s%s%s", ref, need_slash ? "/" : "", copy_start);

	return abs;
}