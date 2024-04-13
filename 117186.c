static void delete_lock(struct lock **lockp)
{
    struct lock *l = *lockp;
    *lockp = l->next;
    free(l);
}