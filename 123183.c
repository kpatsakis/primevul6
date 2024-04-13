virStoragePoolObjBuildTempFilePath(virStoragePoolObj *obj,
                                   virStorageVolDef *voldef)

{
    virStoragePoolDef *def = virStoragePoolObjGetDef(obj);
    return g_strdup_printf("%s/%s.%s.secret.XXXXXX", driver->stateDir,
                           def->name, voldef->name);
}