storagePoolUpdateAllState(void)
{
    virStoragePoolObjListForEach(driver->pools,
                                 storagePoolUpdateStateCallback,
                                 NULL);
}