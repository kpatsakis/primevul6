check_exit_attribute(const char *name, char *test, char *trimmed, char *untrimmed)
{
    if (VALID_STRING(test) && (trimmed != 0)) {
	if (similar_sgr(-1, trimmed, test) ||
	    similar_sgr(-1, untrimmed, test)) {
	    _nc_warning("%s matches exit_attribute_mode", name);
	}
    }
}