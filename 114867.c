void thr_info_freeze(struct thr_info *thr)
{
	struct tq_ent *ent, *iter;
	struct thread_q *tq;

	if (!thr)
		return;

	tq = thr->q;
	if (!tq)
		return;

	mutex_lock(&tq->mutex);
	tq->frozen = true;
	list_for_each_entry_safe(ent, iter, &tq->q, q_node) {
		list_del(&ent->q_node);
		free(ent);
	}
	mutex_unlock(&tq->mutex);
}