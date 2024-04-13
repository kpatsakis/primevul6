virStoragePoolUpdateInactive(virStoragePoolObj *obj)
{
    if (!virStoragePoolObjGetConfigFile(obj)) {
        virStoragePoolObjRemove(driver->pools, obj);
    } else if (virStoragePoolObjGetNewDef(obj)) {
        virStoragePoolObjDefUseNewDef(obj);
    }
}