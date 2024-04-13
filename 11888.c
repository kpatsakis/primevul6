valid_db_path(const char *nominal)
{
    struct stat sb;
#if USE_HASHED_DB
    char suffix[] = DBM_SUFFIX;
    size_t need = strlen(nominal) + sizeof(suffix);
    char *result = malloc(need);

    if (result == 0)
	failed("valid_db_path");
    _nc_STRCPY(result, nominal, need);
    if (strcmp(result + need - sizeof(suffix), suffix)) {
	_nc_STRCAT(result, suffix, need);
    }
#else
    char *result = strdup(nominal);
#endif

    DEBUG(1, ("** stat(%s)", result));
    if (stat(result, &sb) >= 0) {
#if USE_HASHED_DB
	if (!S_ISREG(sb.st_mode)
	    || access(result, R_OK | W_OK) != 0) {
	    DEBUG(1, ("...not a writable file"));
	    free(result);
	    result = 0;
	}
#else
	if (!S_ISDIR(sb.st_mode)
	    || access(result, R_OK | W_OK | X_OK) != 0) {
	    DEBUG(1, ("...not a writable directory"));
	    free(result);
	    result = 0;
	}
#endif
    } else {
	/* check if parent is directory and is writable */
	unsigned leaf = _nc_pathlast(result);

	DEBUG(1, ("...not found"));
	if (leaf) {
	    char save = result[leaf];
	    result[leaf] = 0;
	    if (stat(result, &sb) >= 0
		&& S_ISDIR(sb.st_mode)
		&& access(result, R_OK | W_OK | X_OK) == 0) {
		result[leaf] = save;
	    } else {
		DEBUG(1, ("...parent directory %s is not writable", result));
		free(result);
		result = 0;
	    }
	} else {
	    DEBUG(1, ("... no parent directory"));
	    free(result);
	    result = 0;
	}
    }
    return result;
}