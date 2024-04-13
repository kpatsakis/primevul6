static inline void fpu_free(struct fpu *fpu)
{
	if (fpu->state) {
		kmem_cache_free(task_xstate_cachep, fpu->state);
		fpu->state = NULL;
	}
}