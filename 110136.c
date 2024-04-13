 */
PHPAPI void php_print_info(int flag TSRMLS_DC)
{
	char **env, *tmp1, *tmp2;
	char *php_uname;

	if (!sapi_module.phpinfo_as_text) {
		php_print_info_htmlhead(TSRMLS_C);
	} else {
		php_info_print("phpinfo()\n");
	}

	if (flag & PHP_INFO_GENERAL) {
		char *zend_version = get_zend_version();
		char temp_api[10];

		php_uname = php_get_uname('a');

		if (!sapi_module.phpinfo_as_text) {
			php_info_print_box_start(1);
		}

		if (!sapi_module.phpinfo_as_text) {
	        time_t the_time;
	        struct tm *ta, tmbuf;

	        the_time = time(NULL);
	        ta = php_localtime_r(&the_time, &tmbuf);

            php_info_print("<a href=\"http://www.php.net/\"><img border=\"0\" src=\"");
	        if (ta && (ta->tm_mon==3) && (ta->tm_mday==1)) {
		        php_info_print(PHP_EGG_LOGO_DATA_URI "\" alt=\"PHP logo\" /></a>");
	        } else {
		        php_info_print(PHP_LOGO_DATA_URI "\" alt=\"PHP logo\" /></a>");
			}
		}

		if (!sapi_module.phpinfo_as_text) {
			php_info_printf("<h1 class=\"p\">PHP Version %s</h1>\n", PHP_VERSION);
		} else {
			php_info_print_table_row(2, "PHP Version", PHP_VERSION);
		}
		php_info_print_box_end();
		php_info_print_table_start();
		php_info_print_table_row(2, "System", php_uname );
		php_info_print_table_row(2, "Build Date", __DATE__ " " __TIME__ );
#ifdef COMPILER
		php_info_print_table_row(2, "Compiler", COMPILER);
#endif
#ifdef ARCHITECTURE
		php_info_print_table_row(2, "Architecture", ARCHITECTURE);
#endif
#ifdef CONFIGURE_COMMAND
		php_info_print_table_row(2, "Configure Command", CONFIGURE_COMMAND );
#endif

		if (sapi_module.pretty_name) {
			php_info_print_table_row(2, "Server API", sapi_module.pretty_name );
		}

#ifdef VIRTUAL_DIR
		php_info_print_table_row(2, "Virtual Directory Support", "enabled" );
#else
		php_info_print_table_row(2, "Virtual Directory Support", "disabled" );
#endif

		php_info_print_table_row(2, "Configuration File (php.ini) Path", PHP_CONFIG_FILE_PATH);
		php_info_print_table_row(2, "Loaded Configuration File", php_ini_opened_path ? php_ini_opened_path : "(none)");
		php_info_print_table_row(2, "Scan this dir for additional .ini files", php_ini_scanned_path ? php_ini_scanned_path : "(none)");
		php_info_print_table_row(2, "Additional .ini files parsed", php_ini_scanned_files ? php_ini_scanned_files : "(none)");

		snprintf(temp_api, sizeof(temp_api), "%d", PHP_API_VERSION);
		php_info_print_table_row(2, "PHP API", temp_api);

		snprintf(temp_api, sizeof(temp_api), "%d", ZEND_MODULE_API_NO);
		php_info_print_table_row(2, "PHP Extension", temp_api);

		snprintf(temp_api, sizeof(temp_api), "%d", ZEND_EXTENSION_API_NO);
		php_info_print_table_row(2, "Zend Extension", temp_api);

		php_info_print_table_row(2, "Zend Extension Build", ZEND_EXTENSION_BUILD_ID);
		php_info_print_table_row(2, "PHP Extension Build", ZEND_MODULE_BUILD_ID);

#if ZEND_DEBUG
		php_info_print_table_row(2, "Debug Build", "yes" );
#else
		php_info_print_table_row(2, "Debug Build", "no" );
#endif

#ifdef ZTS
		php_info_print_table_row(2, "Thread Safety", "enabled" );
#else
		php_info_print_table_row(2, "Thread Safety", "disabled" );
#endif

#ifdef ZEND_SIGNALS
		php_info_print_table_row(2, "Zend Signal Handling", "enabled" );
#else
		php_info_print_table_row(2, "Zend Signal Handling", "disabled" );
#endif

		php_info_print_table_row(2, "Zend Memory Manager", is_zend_mm(TSRMLS_C) ? "enabled" : "disabled" );

		{
			const zend_multibyte_functions *functions = zend_multibyte_get_functions(TSRMLS_C);
			char *descr;
			if (functions) {
				spprintf(&descr, 0, "provided by %s", functions->provider_name);
			} else {
				descr = estrdup("disabled");
			}
            php_info_print_table_row(2, "Zend Multibyte Support", descr);
			efree(descr);
		}

#if HAVE_IPV6
		php_info_print_table_row(2, "IPv6 Support", "enabled" );
#else
		php_info_print_table_row(2, "IPv6 Support", "disabled" );
#endif

#if HAVE_DTRACE
		php_info_print_table_row(2, "DTrace Support", "enabled" );
#else
		php_info_print_table_row(2, "DTrace Support", "disabled" );
#endif

		php_info_print_stream_hash("PHP Streams",  php_stream_get_url_stream_wrappers_hash() TSRMLS_CC);
		php_info_print_stream_hash("Stream Socket Transports", php_stream_xport_get_hash() TSRMLS_CC);
		php_info_print_stream_hash("Stream Filters", php_get_stream_filters_hash() TSRMLS_CC);

		php_info_print_table_end();

		/* Zend Engine */
		php_info_print_box_start(0);
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<a href=\"http://www.zend.com/\"><img border=\"0\" src=\"");
			php_info_print(ZEND_LOGO_DATA_URI "\" alt=\"Zend logo\" /></a>\n");
		}
		php_info_print("This program makes use of the Zend Scripting Language Engine:");
		php_info_print(!sapi_module.phpinfo_as_text?"<br />":"\n");
		if (sapi_module.phpinfo_as_text) {
			php_info_print(zend_version);
		} else {
			zend_html_puts(zend_version, strlen(zend_version) TSRMLS_CC);
		}
		php_info_print_box_end();
		efree(php_uname);
	}

	zend_ini_sort_entries(TSRMLS_C);

	if (flag & PHP_INFO_CONFIGURATION) {
		php_info_print_hr();
		if (!sapi_module.phpinfo_as_text) {
			php_info_print("<h1>Configuration</h1>\n");
		} else {
			SECTION("Configuration");
		}
		if (!(flag & PHP_INFO_MODULES)) {
			SECTION("PHP Core");
			display_ini_entries(NULL);
		}
	}

	if (flag & PHP_INFO_MODULES) {
		HashTable sorted_registry;
		zend_module_entry tmp;

		zend_hash_init(&sorted_registry, zend_hash_num_elements(&module_registry), NULL, NULL, 1);
		zend_hash_copy(&sorted_registry, &module_registry, NULL, &tmp, sizeof(zend_module_entry));
		zend_hash_sort(&sorted_registry, zend_qsort, module_name_cmp, 0 TSRMLS_CC);

		zend_hash_apply(&sorted_registry, (apply_func_t) _display_module_info_func TSRMLS_CC);

		SECTION("Additional Modules");
		php_info_print_table_start();
		php_info_print_table_header(1, "Module Name");
		zend_hash_apply(&sorted_registry, (apply_func_t) _display_module_info_def TSRMLS_CC);
		php_info_print_table_end();

		zend_hash_destroy(&sorted_registry);
	}

	if (flag & PHP_INFO_ENVIRONMENT) {
		SECTION("Environment");
		php_info_print_table_start();
		php_info_print_table_header(2, "Variable", "Value");
		for (env=environ; env!=NULL && *env !=NULL; env++) {
			tmp1 = estrdup(*env);
			if (!(tmp2=strchr(tmp1,'='))) { /* malformed entry? */
				efree(tmp1);
				continue;
			}
			*tmp2 = 0;
			tmp2++;
			php_info_print_table_row(2, tmp1, tmp2);
			efree(tmp1);
		}
		php_info_print_table_end();
	}

	if (flag & PHP_INFO_VARIABLES) {
		zval **data;

		SECTION("PHP Variables");

		php_info_print_table_start();
		php_info_print_table_header(2, "Variable", "Value");
		if (zend_hash_find(&EG(symbol_table), "PHP_SELF", sizeof("PHP_SELF"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_SELF", Z_STRVAL_PP(data));
		}
		if (zend_hash_find(&EG(symbol_table), "PHP_AUTH_TYPE", sizeof("PHP_AUTH_TYPE"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_TYPE", Z_STRVAL_PP(data));
		}
		if (zend_hash_find(&EG(symbol_table), "PHP_AUTH_USER", sizeof("PHP_AUTH_USER"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_USER", Z_STRVAL_PP(data));
		}
		if (zend_hash_find(&EG(symbol_table), "PHP_AUTH_PW", sizeof("PHP_AUTH_PW"), (void **) &data) != FAILURE) {
			php_info_print_table_row(2, "PHP_AUTH_PW", Z_STRVAL_PP(data));
		}
		php_print_gpcse_array(ZEND_STRL("_REQUEST") TSRMLS_CC);
		php_print_gpcse_array(ZEND_STRL("_GET") TSRMLS_CC);
		php_print_gpcse_array(ZEND_STRL("_POST") TSRMLS_CC);
		php_print_gpcse_array(ZEND_STRL("_FILES") TSRMLS_CC);
		php_print_gpcse_array(ZEND_STRL("_COOKIE") TSRMLS_CC);
		php_print_gpcse_array(ZEND_STRL("_SERVER") TSRMLS_CC);
		php_print_gpcse_array(ZEND_STRL("_ENV") TSRMLS_CC);
		php_info_print_table_end();
	}


	if ((flag & PHP_INFO_CREDITS) && !sapi_module.phpinfo_as_text) {
		php_info_print_hr();
		php_print_credits(PHP_CREDITS_ALL & ~PHP_CREDITS_FULLPAGE TSRMLS_CC);
	}

	if (flag & PHP_INFO_LICENSE) {
		if (!sapi_module.phpinfo_as_text) {
			SECTION("PHP License");
			php_info_print_box_start(0);
			php_info_print("<p>\n");
			php_info_print("This program is free software; you can redistribute it and/or modify ");
			php_info_print("it under the terms of the PHP License as published by the PHP Group ");
			php_info_print("and included in the distribution in the file:  LICENSE\n");
			php_info_print("</p>\n");
			php_info_print("<p>");
			php_info_print("This program is distributed in the hope that it will be useful, ");
			php_info_print("but WITHOUT ANY WARRANTY; without even the implied warranty of ");
			php_info_print("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
			php_info_print("</p>\n");
			php_info_print("<p>");
			php_info_print("If you did not receive a copy of the PHP license, or have any questions about ");
			php_info_print("PHP licensing, please contact license@php.net.\n");
			php_info_print("</p>\n");
			php_info_print_box_end();
		} else {
			php_info_print("\nPHP License\n");
			php_info_print("This program is free software; you can redistribute it and/or modify\n");
			php_info_print("it under the terms of the PHP License as published by the PHP Group\n");
			php_info_print("and included in the distribution in the file:  LICENSE\n");
			php_info_print("\n");
			php_info_print("This program is distributed in the hope that it will be useful,\n");
			php_info_print("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
			php_info_print("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
			php_info_print("\n");
			php_info_print("If you did not receive a copy of the PHP license, or have any\n");
			php_info_print("questions about PHP licensing, please contact license@php.net.\n");
		}
	}

	if (!sapi_module.phpinfo_as_text) {
		php_info_print("</div></body></html>");
	}