cd_device_db_get_property (CdDeviceDb *ddb,
			   const gchar *device_id,
			   const gchar *property,
			   GError  **error)
{
	CdDeviceDbPrivate *priv = GET_PRIVATE (ddb);
	gchar *error_msg = NULL;
	gchar *statement;
	gint rc;
	gchar *value = NULL;
	g_autoptr(GPtrArray) array_tmp = NULL;

	g_return_val_if_fail (CD_IS_DEVICE_DB (ddb), NULL);
	g_return_val_if_fail (priv->db != NULL, NULL);

	g_debug ("CdDeviceDb: get property %s for %s", property, device_id);
	statement = sqlite3_mprintf ("SELECT value FROM properties_v2 WHERE "
				     "device_id = '%q' AND "
				     "property = '%q' LIMIT 1;",
				     device_id, property);

	/* remove the entry */
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

	/* never set */
	if (array_tmp->len == 0) {
		g_set_error (error,
			     CD_CLIENT_ERROR,
			     CD_CLIENT_ERROR_INTERNAL,
			     "no such property %s for %s",
			     property, device_id);
		goto out;
	}

	/* success */
	value = g_strdup (g_ptr_array_index (array_tmp, 0));
out:
	sqlite3_free (statement);
	return value;
}