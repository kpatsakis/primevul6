static struct fuse_module *fuse_find_module(const char *module)
{
    struct fuse_module *m;
    for (m = fuse_modules; m; m = m->next) {
        if (strcmp(module, m->name) == 0) {
            m->ctr++;
            break;
        }
    }
    return m;
}