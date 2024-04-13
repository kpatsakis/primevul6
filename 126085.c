static void coalesce_file_region(struct resv_map *resv, struct file_region *rg)
{
	struct file_region *nrg = NULL, *prg = NULL;

	prg = list_prev_entry(rg, link);
	if (&prg->link != &resv->regions && prg->to == rg->from &&
	    has_same_uncharge_info(prg, rg)) {
		prg->to = rg->to;

		list_del(&rg->link);
		put_uncharge_info(rg);
		kfree(rg);

		rg = prg;
	}

	nrg = list_next_entry(rg, link);
	if (&nrg->link != &resv->regions && nrg->from == rg->to &&
	    has_same_uncharge_info(nrg, rg)) {
		nrg->from = rg->from;

		list_del(&rg->link);
		put_uncharge_info(rg);
		kfree(rg);
	}
}