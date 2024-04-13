static int fuse_load_so_module(const char *module)
{
    int res;
    char *soname = malloc(strlen(module) + 64);
    if (!soname) {
        fprintf(stderr, "fuse: memory allocation failed\n");
        return -1;
    }
    sprintf(soname, "libfusemod_%s.so", module);
    res = fuse_load_so_name(soname);
    free(soname);
    return res;
}