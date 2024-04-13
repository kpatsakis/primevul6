cd_device_db_class_init (CdDeviceDbClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = cd_device_db_finalize;
}