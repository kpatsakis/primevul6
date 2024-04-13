storageDriverAutostartCallback(virStoragePoolObj *obj,
                               const void *opaque G_GNUC_UNUSED)
{
    virStoragePoolDef *def = virStoragePoolObjGetDef(obj);
    virStorageBackend *backend;
    bool started = false;

    if (!(backend = virStorageBackendForType(def->type)))
        return;

    if (virStoragePoolObjIsAutostart(obj) &&
        !virStoragePoolObjIsActive(obj)) {

        virStoragePoolObjSetStarting(obj, true);
        if (backend->startPool &&
            backend->startPool(obj) < 0) {
            virReportError(VIR_ERR_INTERNAL_ERROR,
                           _("Failed to autostart storage pool '%s': %s"),
                           def->name, virGetLastErrorMessage());
            goto cleanup;
        }
        started = true;
    }

    if (started) {
        g_autofree char *stateFile = NULL;

        stateFile = virFileBuildPath(driver->stateDir, def->name, ".xml");
        if (!stateFile ||
            virStoragePoolSaveState(stateFile, def) < 0 ||
            storagePoolRefreshImpl(backend, obj, stateFile) < 0) {
            virReportError(VIR_ERR_INTERNAL_ERROR,
                           _("Failed to autostart storage pool '%s': %s"),
                           def->name, virGetLastErrorMessage());
        } else {
            virStoragePoolObjSetActive(obj, true);
        }
    }

 cleanup:
    if (virStoragePoolObjIsStarting(obj)) {
        if (!virStoragePoolObjIsActive(obj))
            virStoragePoolUpdateInactive(obj);
        virStoragePoolObjSetStarting(obj, false);
    }
}