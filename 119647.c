cd_device_db_get_devices (CdDeviceDb *ddb,
			  GError  **error)
{
	CdDeviceDbPrivate *priv = GET_PRIVATE (ddb);
	gchar *error_msg = NULL;
	gchar *statement;
	gint rc;
	GPtrArray *array = NULL;
	g_autoptr(GPtrArray) array_tmp = NULL;

	g_return_val_if_fail (CD_IS_DEVICE_DB (ddb), NULL);
	g_return_val_if_fail (priv->db != NULL, NULL);

	/* get all the devices */
	g_debug ("CdDeviceDb: get devices");
	statement = sqlite3_mprintf ("SELECT device_id FROM devices;");
	array_tmp = g_ptr_array_new_with_free_func (g_free);
	rc = sqlite3_exec (priv->db,
			   statement,
			   cd_device_db_sqlite_cb,
			   array_tmp,
			   &error_msg);
	if (rc != SQLITE_OK) {
		g_set_error (error,
			     CD_CLIENT_ERROR,
			     CD_CLIENT_ERROR_INTERNAL,
			     "SQL error: %s",
			     error_msg);
		sqlite3_free (error_msg);
		goto out;
	}

	/* success */
	array = g_ptr_array_ref (array_tmp);
out:
	sqlite3_free (statement);
	return array;
}