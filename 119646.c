cd_device_db_remove (CdDeviceDb *ddb,
		     const gchar *device_id,
		     GError  **error)
{
	CdDeviceDbPrivate *priv = GET_PRIVATE (ddb);
	gboolean ret = TRUE;
	gchar *error_msg = NULL;
	gchar *statement1 = NULL;
	gchar *statement2 = NULL;
	gint rc;

	g_return_val_if_fail (CD_IS_DEVICE_DB (ddb), FALSE);
	g_return_val_if_fail (priv->db != NULL, FALSE);

	/* remove the entry */
	g_debug ("CdDeviceDb: remove device %s", device_id);
	statement1 = sqlite3_mprintf ("DELETE FROM devices WHERE "
				     "device_id = '%q';",
				     device_id);
	rc = sqlite3_exec (priv->db, statement1, NULL, NULL, &error_msg);
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
	statement2 = sqlite3_mprintf ("DELETE FROM properties_v2 WHERE "
				     "device_id = '%q';",
				     device_id);
	rc = sqlite3_exec (priv->db, statement2, NULL, NULL, &error_msg);
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
	sqlite3_free (statement1);
	sqlite3_free (statement2);
	return ret;
}