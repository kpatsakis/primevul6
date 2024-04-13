static int php_zip_get_num_files(struct zip *za TSRMLS_DC) /* {{{ */
{
	return zip_get_num_files(za);
}