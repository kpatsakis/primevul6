mono_free_verify_list (GSList *list)
{
	MonoVerifyInfoExtended *info;
	GSList *tmp;

	for (tmp = list; tmp; tmp = tmp->next) {
		info = tmp->data;
		g_free (info->info.message);
		g_free (info);
	}
	g_slist_free (list);
}