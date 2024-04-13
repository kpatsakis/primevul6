static int fuse_lib_opt_proc(void *data, const char *arg, int key,
                             struct fuse_args *outargs)
{
    (void) arg; (void) outargs;

    if (key == KEY_HELP) {
        struct fuse_config *conf = (struct fuse_config *) data;
        fuse_lib_help();
        conf->help = 1;
    }

    return 1;
}