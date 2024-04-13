static int php_zip_status_sys(struct zip *za TSRMLS_DC) /* {{{ */
{
	int zep, syp;

	zip_error_get(za, &zep, &syp);
	return syp;
}