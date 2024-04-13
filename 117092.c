static void open_auto_cache(struct fuse *f, fuse_ino_t ino, const char *path,
                            struct fuse_file_info *fi)
{
    struct node *node;

    pthread_mutex_lock(&f->lock);
    node = get_node(f, ino);
    if (node->cache_valid) {
        struct timespec now;

        curr_time(&now);
        if (diff_timespec(&now, &node->stat_updated) > f->conf.ac_attr_timeout) {
            struct stat stbuf;
            int err;
            pthread_mutex_unlock(&f->lock);
            err = fuse_fs_fgetattr(f->fs, path, &stbuf, fi);
            pthread_mutex_lock(&f->lock);
            if (!err)
                update_stat(node, &stbuf);
            else
                node->cache_valid = 0;
        }
    }
    if (node->cache_valid)
        fi->keep_cache = 1;

    node->cache_valid = 1;
    pthread_mutex_unlock(&f->lock);
}