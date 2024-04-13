storageVolCreateXMLFrom(virStoragePoolPtr pool,
                        const char *xmldesc,
                        virStorageVolPtr volsrc,
                        unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStoragePoolObj *objsrc = NULL;
    virStorageBackend *backend;
    virStorageVolDef *voldefsrc = NULL;
    virStorageVolDef *shadowvol = NULL;
    virStorageVolPtr newvol = NULL;
    virStorageVolPtr vol = NULL;
    int buildret;
    g_autoptr(virStorageVolDef) voldef = NULL;

    virCheckFlags(VIR_STORAGE_VOL_CREATE_PREALLOC_METADATA |
                  VIR_STORAGE_VOL_CREATE_REFLINK,
                  NULL);

    obj = virStoragePoolObjFindByUUID(driver->pools, pool->uuid);
    if (obj && STRNEQ(pool->name, volsrc->pool)) {
        virObjectUnlock(obj);
        objsrc = virStoragePoolObjFindByName(driver->pools, volsrc->pool);
        virObjectLock(obj);
    }
    if (!obj) {
        char uuidstr[VIR_UUID_STRING_BUFLEN];
        virUUIDFormat(pool->uuid, uuidstr);
        virReportError(VIR_ERR_NO_STORAGE_POOL,
                       _("no storage pool with matching uuid '%s' (%s)"),
                       uuidstr, pool->name);
        goto cleanup;
    }
    def = virStoragePoolObjGetDef(obj);

    if (STRNEQ(pool->name, volsrc->pool) && !objsrc) {
        virReportError(VIR_ERR_NO_STORAGE_POOL,
                       _("no storage pool with matching name '%s'"),
                       volsrc->pool);
        goto cleanup;
    }

    if (!virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"), def->name);
        goto cleanup;
    }

    if (objsrc && !virStoragePoolObjIsActive(objsrc)) {
        virStoragePoolDef *objsrcdef = virStoragePoolObjGetDef(objsrc);
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"),
                       objsrcdef->name);
        goto cleanup;
    }

    if ((backend = virStorageBackendForType(def->type)) == NULL)
        goto cleanup;

    voldefsrc = virStorageVolDefFindByName(objsrc ?
                                           objsrc : obj, volsrc->name);
    if (!voldefsrc) {
        virReportError(VIR_ERR_NO_STORAGE_VOL,
                       _("no storage vol with matching name '%s'"),
                       volsrc->name);
        goto cleanup;
    }

    voldef = virStorageVolDefParseString(def, xmldesc,
                                         VIR_VOL_XML_PARSE_NO_CAPACITY);
    if (voldef == NULL)
        goto cleanup;

    if (virStorageVolCreateXMLFromEnsureACL(pool->conn, def, voldef) < 0)
        goto cleanup;

    if (virStorageVolDefFindByName(obj, voldef->name)) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("storage volume name '%s' already in use."),
                       voldef->name);
        goto cleanup;
    }

    /* Use the original volume's capacity in case the new capacity
     * is less than that, or it was omitted */
    if (voldef->target.capacity < voldefsrc->target.capacity)
        voldef->target.capacity = voldefsrc->target.capacity;

    /* If the allocation was not provided in the XML, then use capacity
     * as it's specifically documented "If omitted when creating a volume,
     * the  volume will be fully allocated at time of creation.". This
     * is especially important for logical volume creation. */
    if (!voldef->target.has_allocation)
        voldef->target.allocation = voldef->target.capacity;

    if (!backend->buildVolFrom) {
        virReportError(VIR_ERR_NO_SUPPORT,
                       "%s", _("storage pool does not support"
                               " volume creation from an existing volume"));
        goto cleanup;
    }

    if (voldefsrc->building) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("volume '%s' is still being allocated."),
                       voldefsrc->name);
        goto cleanup;
    }

    if (backend->refreshVol &&
        backend->refreshVol(obj, voldefsrc) < 0)
        goto cleanup;

    /* 'Define' the new volume so we get async progress reporting.
     * Wipe any key the user may have suggested, as volume creation
     * will generate the canonical key.  */
    VIR_FREE(voldef->key);
    if (backend->createVol(obj, voldef) < 0)
        goto cleanup;

    /* Make a shallow copy of the 'defined' volume definition, since the
     * original allocation value will change as the user polls 'info',
     * but we only need the initial requested values
     */
    shadowvol = g_new0(virStorageVolDef, 1);

    memcpy(shadowvol, voldef, sizeof(*voldef));

    if (!(newvol = virGetStorageVol(pool->conn, def->name, voldef->name,
                                    voldef->key, NULL, NULL)))
        goto cleanup;

    /* NB: Upon success voldef "owned" by storage pool for deletion purposes */
    if (virStoragePoolObjAddVol(obj, voldef) < 0)
        goto cleanup;

    /* Drop the pool lock during volume allocation */
    virStoragePoolObjIncrAsyncjobs(obj);
    voldef->building = true;
    voldefsrc->in_use++;
    virObjectUnlock(obj);

    if (objsrc) {
        virStoragePoolObjIncrAsyncjobs(objsrc);
        virObjectUnlock(objsrc);
    }

    buildret = backend->buildVolFrom(obj, shadowvol, voldefsrc, flags);

    virObjectLock(obj);
    if (objsrc)
        virObjectLock(objsrc);

    voldefsrc->in_use--;
    voldef->building = false;
    virStoragePoolObjDecrAsyncjobs(obj);

    if (objsrc) {
        virStoragePoolObjDecrAsyncjobs(objsrc);
        virStoragePoolObjEndAPI(&objsrc);
    }

    if (buildret < 0 ||
        (backend->refreshVol &&
         backend->refreshVol(obj, voldef) < 0)) {
        storageVolDeleteInternal(backend, obj, voldef, 0, false);
        voldef = NULL;
        goto cleanup;
    }

    /* Updating pool metadata ignoring the disk backend since
     * it updates the pool values
     */
    if (def->type != VIR_STORAGE_POOL_DISK) {
        def->allocation += voldef->target.allocation;
        def->available -= voldef->target.allocation;
    }

    VIR_INFO("Creating volume '%s' in storage pool '%s'",
             newvol->name, def->name);
    vol = g_steal_pointer(&newvol);
    voldef = NULL;

 cleanup:
    virObjectUnref(newvol);
    VIR_FREE(shadowvol);
    virStoragePoolObjEndAPI(&obj);
    virStoragePoolObjEndAPI(&objsrc);
    return vol;
}