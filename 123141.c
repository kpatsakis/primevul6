storagePoolRefreshImpl(virStorageBackend *backend,
                       virStoragePoolObj *obj,
                       const char *stateFile)
{
    virStoragePoolObjClearVols(obj);
    if (backend->refreshPool(obj) < 0) {
        storagePoolRefreshFailCleanup(backend, obj, stateFile);
        return -1;
    }

    return 0;
}