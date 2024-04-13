int fuse_is_lib_option(const char *opt)
{
    return fuse_lowlevel_is_lib_option(opt) ||
        fuse_opt_match(fuse_lib_opts, opt);
}