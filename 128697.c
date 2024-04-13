static char * php_zipobj_get_zip_comment(struct zip *za, int *len TSRMLS_DC) /* {{{ */
{
	if (za) {
		return (char *)zip_get_archive_comment(za, len, 0);
	}
	return NULL;
}