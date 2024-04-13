virStoragePoolObjFindPoolByUUID(const unsigned char *uuid)
{
    return virStoragePoolObjFindByUUID(driver->pools, uuid);
}