static void flock_to_lock(struct flock *flock, struct lock *lock)
{
    memset(lock, 0, sizeof(struct lock));
    lock->type = flock->l_type;
    lock->start = flock->l_start;
    lock->end = flock->l_len ? flock->l_start + flock->l_len - 1 : OFFSET_MAX;
    lock->pid = flock->l_pid;
}