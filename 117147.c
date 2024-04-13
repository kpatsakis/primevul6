static int fuse_load_so_name(const char *soname)
{
    struct fusemod_so *so;

    so = calloc(1, sizeof(struct fusemod_so));
    if (!so) {
        fprintf(stderr, "fuse: memory allocation failed\n");
        return -1;
    }

    fuse_current_so = so;
    so->handle = dlopen(soname, RTLD_NOW);
    fuse_current_so = NULL;
    if (!so->handle) {
        fprintf(stderr, "fuse: %s\n", dlerror());
        goto err;
    }
    if (!so->ctr) {
        fprintf(stderr, "fuse: %s did not register any modules", soname);
        goto err;
    }
    return 0;

 err:
    if (so->handle)
        dlclose(so->handle);
    free(so);
    return -1;
}