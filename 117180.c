static struct lock *locks_conflict(struct node *node, const struct lock *lock)
{
    struct lock *l;

    for (l = node->locks; l; l = l->next)
        if (l->owner != lock->owner &&
            lock->start <= l->end && l->start <= lock->end &&
            (l->type == F_WRLCK || lock->type == F_WRLCK))
            break;

    return l;
}