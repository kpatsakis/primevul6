static void storageDriverLock(void)
{
    virMutexLock(&driver->lock);
}