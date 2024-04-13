storageConnectFindStoragePoolSources(virConnectPtr conn,
                                     const char *type,
                                     const char *srcSpec,
                                     unsigned int flags)
{
    int backend_type;
    virStorageBackend *backend;

    if (virConnectFindStoragePoolSourcesEnsureACL(conn) < 0)
        return NULL;

    backend_type = virStoragePoolTypeFromString(type);
    if (backend_type < 0) {
        virReportError(VIR_ERR_INTERNAL_ERROR,
                       _("unknown storage pool type %s"), type);
        return NULL;
    }

    backend = virStorageBackendForType(backend_type);
    if (backend == NULL)
        return NULL;

    if (!backend->findPoolSources) {
        virReportError(VIR_ERR_NO_SUPPORT,
                       _("pool type '%s' does not support source "
                         "discovery"), type);
        return NULL;
    }

    return backend->findPoolSources(srcSpec, flags);
}