cd_device_db_new (void)
{
	if (cd_device_db_object != NULL) {
		g_object_ref (cd_device_db_object);
	} else {
		cd_device_db_object = g_object_new (CD_TYPE_DEVICE_DB, NULL);
		g_object_add_weak_pointer (cd_device_db_object, &cd_device_db_object);
	}
	return CD_DEVICE_DB (cd_device_db_object);
}