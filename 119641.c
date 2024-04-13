cd_device_db_finalize (GObject *object)
{
	CdDeviceDb *ddb = CD_DEVICE_DB (object);
	CdDeviceDbPrivate *priv = GET_PRIVATE (ddb);

	/* close the database */
	sqlite3_close (priv->db);

	G_OBJECT_CLASS (cd_device_db_parent_class)->finalize (object);
}