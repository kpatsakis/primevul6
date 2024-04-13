static void insert_lock(struct lock **pos, struct lock *lock)
{
    lock->next = *pos;
    *pos = lock;
}