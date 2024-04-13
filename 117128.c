static int fuse_create_context_key(void)
{
    int err = 0;
    pthread_mutex_lock(&fuse_context_lock);
    if (!fuse_context_ref) {
        err = pthread_key_create(&fuse_context_key, fuse_freecontext);
        if (err) {
            fprintf(stderr, "fuse: failed to create thread specific key: %s\n",
                    strerror(err));
            pthread_mutex_unlock(&fuse_context_lock);
            return -1;
        }
    }
    fuse_context_ref++;
    pthread_mutex_unlock(&fuse_context_lock);
    return 0;
}