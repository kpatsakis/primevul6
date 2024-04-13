static int locks_insert(struct node *node, struct lock *lock)
{
    struct lock **lp;
    struct lock *newl1 = NULL;
    struct lock *newl2 = NULL;

    if (lock->type != F_UNLCK || lock->start != 0 || lock->end != OFFSET_MAX) {
        newl1 = malloc(sizeof(struct lock));
        newl2 = malloc(sizeof(struct lock));

        if (!newl1 || !newl2) {
            free(newl1);
            free(newl2);
            return -ENOLCK;
        }
    }

    for (lp = &node->locks; *lp;) {
        struct lock *l = *lp;
        if (l->owner != lock->owner)
            goto skip;

        if (lock->type == l->type) {
            if (l->end < lock->start - 1)
                goto skip;
            if (lock->end < l->start - 1)
                break;
            if (l->start <= lock->start && lock->end <= l->end)
                goto out;
            if (l->start < lock->start)
                lock->start = l->start;
            if (lock->end < l->end)
                lock->end = l->end;
            goto delete;
        } else {
            if (l->end < lock->start)
                goto skip;
            if (lock->end < l->start)
                break;
            if (lock->start <= l->start && l->end <= lock->end)
                goto delete;
            if (l->end <= lock->end) {
                l->end = lock->start - 1;
                goto skip;
            }
            if (lock->start <= l->start) {
                l->start = lock->end + 1;
                break;
            }
            *newl2 = *l;
            newl2->start = lock->end + 1;
            l->end = lock->start - 1;
            insert_lock(&l->next, newl2);
            newl2 = NULL;
        }
    skip:
        lp = &l->next;
        continue;

    delete:
        delete_lock(lp);
    }
    if (lock->type != F_UNLCK) {
        *newl1 = *lock;
        insert_lock(lp, newl1);
        newl1 = NULL;
    }
out:
    free(newl1);
    free(newl2);
    return 0;
}