cd_device_db_sqlite_cb (void *data,
			gint argc,
			gchar **argv,
			gchar **col_name)
{
	GPtrArray *array = (GPtrArray *) data;

	/* should only be one entry */
	g_debug ("CdDeviceDb: got sql result %s", argv[0]);
	g_ptr_array_add (array, g_strdup (argv[0]));
	return 0;
}