cd_device_db_empty (CdDeviceDb *ddb,
		     GError  **error)
{
	CdDeviceDbPrivate *priv = GET_PRIVATE (ddb);
	const gchar *statement;
	gchar *error_msg = NULL;
	gint rc;

	g_return_val_if_fail (CD_IS_DEVICE_DB (ddb), FALSE);
	g_return_val_if_fail (priv->db != NULL, FALSE);

	statement = "DELETE FROM devices;DELETE FROM properties_v2;";
	rc = sqlite3_exec (priv->db, statement,
			   NULL, NULL, &error_msg);
	if (rc != SQLITE_OK) {
		g_set_error (error,
			     CD_CLIENT_ERROR,
			     CD_CLIENT_ERROR_INTERNAL,
			     "SQL error: %s",
			     error_msg);
		sqlite3_free (error_msg);
		return FALSE;
	}
	return TRUE;
}