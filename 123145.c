storageVolCreateXML(virStoragePoolPtr pool,
                    const char *xmldesc,
                    unsigned int flags)
{
    virStoragePoolObj *obj;
    virStoragePoolDef *def;
    virStorageBackend *backend;
    virStorageVolPtr vol = NULL, newvol = NULL;
    g_autoptr(virStorageVolDef) voldef = NULL;

    virCheckFlags(VIR_STORAGE_VOL_CREATE_PREALLOC_METADATA, NULL);

    if (!(obj = virStoragePoolObjFromStoragePool(pool)))
        return NULL;
    def = virStoragePoolObjGetDef(obj);

    if (!virStoragePoolObjIsActive(obj)) {
        virReportError(VIR_ERR_OPERATION_INVALID,
                       _("storage pool '%s' is not active"), def->name);
        goto cleanup;
    }

    if ((backend = virStorageBackendForType(def->type)) == NULL)
        goto cleanup;

    voldef = virStorageVolDefParseString(def, xmldesc,
                                         VIR_VOL_XML_PARSE_OPT_CAPACITY);
    if (voldef == NULL)
        goto cleanup;

    if (!voldef->target.capacity && !backend->buildVol) {
        virReportError(VIR_ERR_NO_SUPPORT,
                       "%s", _("volume capacity required for this "
                               "storage pool"));
        goto cleanup;
    }

    if (virStorageVolCreateXMLEnsureACL(pool->conn, def, voldef) < 0)
        goto cleanup;

    if (virStorageVolDefFindByName(obj, voldef->name)) {
        virReportError(VIR_ERR_STORAGE_VOL_EXIST,
                       _("'%s'"), voldef->name);
        goto cleanup;
    }

    if (!backend->createVol) {
        virReportError(VIR_ERR_NO_SUPPORT,
                       "%s", _("storage pool does not support volume "
                               "creation"));
        goto cleanup;
    }

    /* Wipe any key the user may have suggested, as volume creation
     * will generate the canonical key.  */
    VIR_FREE(voldef->key);
    if (backend->createVol(obj, voldef) < 0)
        goto cleanup;

    if (!(newvol = virGetStorageVol(pool->conn, def->name, voldef->name,
                                    voldef->key, NULL, NULL)))
        goto cleanup;

    /* NB: Upon success voldef "owned" by storage pool for deletion purposes */
    if (virStoragePoolObjAddVol(obj, voldef) < 0)
        goto cleanup;

    if (backend->buildVol) {
        int buildret;
        virStorageVolDef *buildvoldef = NULL;

        buildvoldef = g_new0(virStorageVolDef, 1);

        /* Make a shallow copy of the 'defined' volume definition, since the
         * original allocation value will change as the user polls 'info',
         * but we only need the initial requested values
         */
        memcpy(buildvoldef, voldef, sizeof(*voldef));

        /* Drop the pool lock during volume allocation */
        virStoragePoolObjIncrAsyncjobs(obj);
        voldef->building = true;
        virObjectUnlock(obj);

        buildret = backend->buildVol(obj, buildvoldef, flags);

        VIR_FREE(buildvoldef);

        virObjectLock(obj);

        voldef->building = false;
        virStoragePoolObjDecrAsyncjobs(obj);

        if (buildret < 0) {
            /* buildVol handles deleting volume on failure */
            virStoragePoolObjRemoveVol(obj, voldef);
            voldef = NULL;
            goto cleanup;
        }

    }

    if (backend->refreshVol &&
        backend->refreshVol(obj, voldef) < 0) {
        storageVolDeleteInternal(backend, obj, voldef,
                                 0, false);
        voldef = NULL;
        goto cleanup;
    }

    /* Update pool metadata ignoring the disk backend since
     * it updates the pool values.
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
    virStoragePoolObjEndAPI(&obj);
    return vol;
}