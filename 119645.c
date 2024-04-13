cd_device_db_add (CdDeviceDb *ddb,
		  const gchar *device_id,
		  GError  **error)
{
	CdDeviceDbPrivate *priv = GET_PRIVATE (ddb);
	gboolean ret = TRUE;
	gchar *error_msg = NULL;
	gchar *statement;
	gint rc;

	g_return_val_if_fail (CD_IS_DEVICE_DB (ddb), FALSE);
	g_return_val_if_fail (priv->db != NULL, FALSE);

	g_debug ("CdDeviceDb: add device %s", device_id);
	statement = sqlite3_mprintf ("INSERT INTO devices (device_id) "
				     "VALUES ('%q')",
				     device_id);

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