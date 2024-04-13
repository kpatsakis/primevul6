static void fuse_lib_help(void)
{
    fprintf(stderr,
"    -o hard_remove         immediate removal (don't hide files)\n"
"    -o use_ino             let filesystem set inode numbers\n"
"    -o readdir_ino         try to fill in d_ino in readdir\n"
"    -o direct_io           use direct I/O\n"
"    -o kernel_cache        cache files in kernel\n"
#ifdef __SOLARIS__
"    -o [no]auto_cache      enable caching based on modification times (off)\n"
#endif /* __SOLARIS__ */
"    -o umask=M             set file permissions (octal)\n"
"    -o uid=N               set file owner\n"
"    -o gid=N               set file group\n"
"    -o entry_timeout=T     cache timeout for names (1.0s)\n"
"    -o negative_timeout=T  cache timeout for deleted names (0.0s)\n"
"    -o attr_timeout=T      cache timeout for attributes (1.0s)\n"
"    -o ac_attr_timeout=T   auto cache timeout for attributes (attr_timeout)\n"
"    -o intr                allow requests to be interrupted\n"
"    -o intr_signal=NUM     signal to send on interrupt (%i)\n"
#ifdef __SOLARIS__
"    -o modules=M1[:M2...]  names of modules to push onto filesystem stack\n"
#endif /* __SOLARIS__ */
"\n", FUSE_DEFAULT_INTR_SIGNAL);
}