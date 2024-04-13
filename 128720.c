static PHP_MINFO_FUNCTION(zip)
{
	php_info_print_table_start();

	php_info_print_table_row(2, "Zip", "enabled");
	php_info_print_table_row(2, "Extension Version","$Id$");
	php_info_print_table_row(2, "Zip version", PHP_ZIP_VERSION_STRING);
	php_info_print_table_row(2, "Libzip version", LIBZIP_VERSION);

	php_info_print_table_end();
}