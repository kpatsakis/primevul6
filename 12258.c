testStorageFileGetMetadata(const char *path,
                           int format,
                           uid_t uid, gid_t gid)
{
    struct stat st;
    g_autoptr(virStorageSource) def = NULL;

    if (!(def = virStorageSourceNew()))
        return NULL;

    def->type = VIR_STORAGE_TYPE_FILE;
    def->format = format;

    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            def->type = VIR_STORAGE_TYPE_DIR;
        } else if (S_ISBLK(st.st_mode)) {
            def->type = VIR_STORAGE_TYPE_BLOCK;
        }
    }

    def->path = g_strdup(path);

    if (virStorageFileGetMetadata(def, uid, gid, true) < 0)
        return NULL;

    return g_steal_pointer(&def);
}