static __always_inline void pv_queued_spin_unlock(struct qspinlock *lock)
{
	PVOP_VCALLEE1(lock.queued_spin_unlock, lock);
}