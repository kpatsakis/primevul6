libxlDomainStartNew(libxlDriverPrivate *driver,
            virDomainObj *vm,
            bool start_paused)
{
    g_autofree char *managed_save_path = NULL;
    int restore_fd = -1;
    virDomainDef *def = NULL;
    libxlSavefileHeader hdr;
    uint32_t restore_ver = LIBXL_SAVE_VERSION;
    int ret = -1;

    /* If there is a managed saved state restore it instead of starting
     * from scratch. The old state is removed once the restoring succeeded. */
    managed_save_path = libxlDomainManagedSavePath(driver, vm);
    if (managed_save_path == NULL)
        return -1;

    if (virFileExists(managed_save_path)) {
        restore_fd = libxlDomainSaveImageOpen(driver, managed_save_path,
                                              &def, &hdr);
        if (restore_fd < 0)
            goto cleanup;

        restore_ver = hdr.version;

        if (STRNEQ(vm->def->name, def->name) ||
            memcmp(vm->def->uuid, def->uuid, VIR_UUID_BUFLEN)) {
            char vm_uuidstr[VIR_UUID_STRING_BUFLEN];
            char def_uuidstr[VIR_UUID_STRING_BUFLEN];
            virUUIDFormat(vm->def->uuid, vm_uuidstr);
            virUUIDFormat(def->uuid, def_uuidstr);
            virReportError(VIR_ERR_OPERATION_FAILED,
                           _("cannot restore domain '%s' uuid %s from a file which belongs to domain '%s' uuid %s"),
                           vm->def->name, vm_uuidstr, def->name, def_uuidstr);
            goto cleanup;
        }

        virDomainObjAssignDef(vm, &def, true, NULL);

        if (unlink(managed_save_path) < 0)
            VIR_WARN("Failed to remove the managed state %s",
                     managed_save_path);

        vm->hasManagedSave = false;
    }

    ret = libxlDomainStart(driver, vm, start_paused, restore_fd, restore_ver);

 cleanup:
    virDomainDefFree(def);
    VIR_FORCE_CLOSE(restore_fd);
    return ret;
}