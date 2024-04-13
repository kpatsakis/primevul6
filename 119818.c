static void setup_capng(void)
{
    /* Note this accesses /proc so has to happen before the sandbox */
    if (capng_get_caps_process()) {
        fuse_log(FUSE_LOG_ERR, "capng_get_caps_process\n");
        exit(1);
    }
    pthread_mutex_init(&cap.mutex, NULL);
    pthread_mutex_lock(&cap.mutex);
    cap.saved = capng_save_state();
    if (!cap.saved) {
        fuse_log(FUSE_LOG_ERR, "capng_save_state\n");
        exit(1);
    }
    pthread_mutex_unlock(&cap.mutex);
}