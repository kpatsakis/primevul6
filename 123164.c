storagePoolRefreshFailCleanup(virStorageBackend *backend,
                              virStoragePoolObj *obj,
                              const char *stateFile)
{
    virErrorPtr orig_err;

    virErrorPreserveLast(&orig_err);
    virStoragePoolObjClearVols(obj);

    if (stateFile)
        unlink(stateFile);
    if (backend->stopPool)
        backend->stopPool(obj);
    virErrorRestore(&orig_err);
}