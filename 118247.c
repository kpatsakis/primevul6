	__releases(&idprv->handler_mutex)
{
	enum rdma_cm_state state;
	unsigned long flags;

	trace_cm_id_destroy(id_priv);

	/*
	 * Setting the state to destroyed under the handler mutex provides a
	 * fence against calling handler callbacks. If this is invoked due to
	 * the failure of a handler callback then it guarentees that no future
	 * handlers will be called.
	 */
	lockdep_assert_held(&id_priv->handler_mutex);
	spin_lock_irqsave(&id_priv->lock, flags);
	state = id_priv->state;
	id_priv->state = RDMA_CM_DESTROYING;
	spin_unlock_irqrestore(&id_priv->lock, flags);
	mutex_unlock(&id_priv->handler_mutex);
	_destroy_id(id_priv, state);
}