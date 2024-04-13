static char *hidden_name(struct fuse *f, fuse_ino_t dir, const char *oldname,
                         char *newname, size_t bufsize)
{
    struct stat buf;
    struct node *node;
    struct node *newnode;
    char *newpath;
    int res;
    int failctr = 10;

    do {
        pthread_mutex_lock(&f->lock);
        node = lookup_node(f, dir, oldname);
        if (node == NULL) {
            pthread_mutex_unlock(&f->lock);
            return NULL;
        }
        do {
            f->hidectr ++;
            snprintf(newname, bufsize, ".fuse_hidden%08x%08x",
                     (unsigned int) node->nodeid, f->hidectr);
            newnode = lookup_node(f, dir, newname);
        } while(newnode);
        pthread_mutex_unlock(&f->lock);

        newpath = get_path_name(f, dir, newname);
        if (!newpath)
            break;

        res = fuse_fs_getattr(f->fs, newpath, &buf);
        if (res == -ENOENT)
            break;
        free(newpath);
        newpath = NULL;
    } while(res == 0 && --failctr);

    return newpath;
}