cd_device_db_set_property (CdDeviceDb *ddb,
			   const gchar *device_id,
			   const gchar *property,
			   const gchar *value,
			   GError  **error)
{
	CdDeviceDbPrivate *priv = GET_PRIVATE (ddb);
	gboolean ret = TRUE;
	gchar *error_msg = NULL;
	gchar *statement;
	gint rc;

	g_return_val_if_fail (CD_IS_DEVICE_DB (ddb), FALSE);
	g_return_val_if_fail (priv->db != NULL, FALSE);

	g_debug ("CdDeviceDb: add device property %s [%s=%s]",
		 device_id, property, value);
	statement = sqlite3_mprintf ("INSERT OR REPLACE INTO properties_v2 (device_id, property, value) "
				     "VALUES ('%q', '%q', '%q');",
				     device_id, property, value);

	/* insert the entry */
	rc = sqlite3_exec (priv->db, statement, NULL, NULL, &error_msg);
	if (rc != SQLITE_OK) {
		g_set_error (error,
			     CD_CLIENT_ERROR,
			     CD_CLIENT_ERROR_INTERNAL,
			     "SQL error: %s",
			     error_msg);
		sqlite3_free (error_msg);
		ret = FALSE;
		goto out;
	}
out:
	sqlite3_free (statement);
	return ret;
}