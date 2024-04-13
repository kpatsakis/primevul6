static __always_inline void pv_queued_spin_lock_slowpath(struct qspinlock *lock,
							u32 val)
{
	PVOP_VCALL2(lock.queued_spin_lock_slowpath, lock, val);
}