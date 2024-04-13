static void storageDriverUnlock(void)
{
    virMutexUnlock(&driver->lock);
}