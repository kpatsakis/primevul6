static void lock_to_flock(struct lock *lock, struct flock *flock)
{
    flock->l_type = lock->type;
    flock->l_start = lock->start;
    flock->l_len = (lock->end == OFFSET_MAX) ? 0 : lock->end - lock->start + 1;
    flock->l_pid = lock->pid;
}