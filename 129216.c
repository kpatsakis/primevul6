static inline void audit_free_aux(struct audit_context *context)
{
	struct audit_aux_data *aux;

	while ((aux = context->aux)) {
		if (aux->type == AUDIT_AVC_PATH) {
			struct audit_aux_data_path *axi = (void *)aux;
			dput(axi->dentry);
			mntput(axi->mnt);
		}

		context->aux = aux->next;
		kfree(aux);
	}
	while ((aux = context->aux_pids)) {
		context->aux_pids = aux->next;
		kfree(aux);
	}
}