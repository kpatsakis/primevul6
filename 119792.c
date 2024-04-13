static void cleanup_capng(void)
{
    free(cap.saved);
    cap.saved = NULL;
    pthread_mutex_destroy(&cap.mutex);
}