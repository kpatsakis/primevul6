static char *get_path_name(struct fuse *f, fuse_ino_t nodeid, const char *name)
{
#ifdef __SOLARIS__
    char buf[FUSE_MAX_PATH];
    char *s = buf + FUSE_MAX_PATH - 1;
    struct node *node;

    *s = '\0';

    if (name != NULL) {
        s = add_name(buf, s, name);
        if (s == NULL)
            return NULL;
    }

    pthread_mutex_lock(&f->lock);
    for (node = get_node(f, nodeid); node && node->nodeid != FUSE_ROOT_ID;
         node = node->parent) {
        if (node->name == NULL) {
            s = NULL;
            break;
        }

        s = add_name(buf, s, node->name);
        if (s == NULL)
            break;
    }
    pthread_mutex_unlock(&f->lock);

    if (node == NULL || s == NULL)
        return NULL;
    else if (*s == '\0')
        return strdup("/");
    else
        return strdup(s);

#else /* __SOLARIS__ */

    unsigned bufsize = 256;
    char *buf;
    char *s;
    struct node *node;

    buf = malloc(bufsize);
    if (buf == NULL)
            return NULL;

    s = buf + bufsize - 1;
    *s = '\0';

    if (name != NULL) {
        s = add_name(&buf, &bufsize, s, name);
        if (s == NULL)
            goto out_free;
    }

    pthread_mutex_lock(&f->lock);
    for (node = get_node(f, nodeid); node && node->nodeid != FUSE_ROOT_ID;
         node = node->parent) {
        if (node->name == NULL) {
            s = NULL;
            break;
        }

        s = add_name(&buf, &bufsize, s, node->name);
        if (s == NULL)
            break;
    }
    pthread_mutex_unlock(&f->lock);

    if (node == NULL || s == NULL)
        goto out_free;
    
    if (s[0])
            memmove(buf, s, bufsize - (s - buf));
    else
            strcpy(buf, "/");
    return buf;
    
out_free:
    free(buf);
    return NULL;
#endif /* __SOLARIS__ */
}