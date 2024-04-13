static void lo_dirp_put(struct lo_dirp **dp)
{
    struct lo_dirp *d = *dp;

    if (!d) {
        return;
    }
    *dp = NULL;

    if (g_atomic_int_dec_and_test(&d->refcount)) {
        closedir(d->dp);
        free(d);
    }
}