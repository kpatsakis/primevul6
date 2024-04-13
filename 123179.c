storageDriverAutostart(void)
{
    virStoragePoolObjListForEach(driver->pools,
                                 storageDriverAutostartCallback,
                                 NULL);
}