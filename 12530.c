cdf_print_classid(char *buf, size_t buflen, const cdf_classid_t *id)
{
	return snprintf(buf, buflen, "%.8x-%.4x-%.4x-%.2x%.2x-"
	    "%.2x%.2x%.2x%.2x%.2x%.2x", id->cl_dword, id->cl_word[0],
	    id->cl_word[1], id->cl_two[0], id->cl_two[1], id->cl_six[0],
	    id->cl_six[1], id->cl_six[2], id->cl_six[3], id->cl_six[4],
	    id->cl_six[5]);
}