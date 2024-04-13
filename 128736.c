static int php_zip_status(struct zip *za TSRMLS_DC) /* {{{ */
{
	int zep, syp;

	zip_error_get(za, &zep, &syp);
	return zep;
}