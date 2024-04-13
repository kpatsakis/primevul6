static int isdn_ppp_set_compressor(struct ippp_struct *is, struct isdn_ppp_comp_data *data)
{
	struct isdn_ppp_compressor *ipc = ipc_head;
	int ret;
	void *stat;
	int num = data->num;

	if (is->debug & 0x10)
		printk(KERN_DEBUG "[%d] Set %s type %d\n", is->unit,
		       (data->flags & IPPP_COMP_FLAG_XMIT) ? "compressor" : "decompressor", num);

	/* If is has no valid reset state vector, we cannot allocate a
	   decompressor. The decompressor would cause reset transactions
	   sooner or later, and they need that vector. */

	if (!(data->flags & IPPP_COMP_FLAG_XMIT) && !is->reset) {
		printk(KERN_ERR "ippp_ccp: no reset data structure - can't"
		       " allow decompression.\n");
		return -ENOMEM;
	}

	while (ipc) {
		if (ipc->num == num) {
			stat = ipc->alloc(data);
			if (stat) {
				ret = ipc->init(stat, data, is->unit, 0);
				if (!ret) {
					printk(KERN_ERR "Can't init (de)compression!\n");
					ipc->free(stat);
					stat = NULL;
					break;
				}
			}
			else {
				printk(KERN_ERR "Can't alloc (de)compression!\n");
				break;
			}

			if (data->flags & IPPP_COMP_FLAG_XMIT) {
				if (data->flags & IPPP_COMP_FLAG_LINK) {
					if (is->link_comp_stat)
						is->link_compressor->free(is->link_comp_stat);
					is->link_comp_stat = stat;
					is->link_compressor = ipc;
				}
				else {
					if (is->comp_stat)
						is->compressor->free(is->comp_stat);
					is->comp_stat = stat;
					is->compressor = ipc;
				}
			}
			else {
				if (data->flags & IPPP_COMP_FLAG_LINK) {
					if (is->link_decomp_stat)
						is->link_decompressor->free(is->link_decomp_stat);
					is->link_decomp_stat = stat;
					is->link_decompressor = ipc;
				}
				else {
					if (is->decomp_stat)
						is->decompressor->free(is->decomp_stat);
					is->decomp_stat = stat;
					is->decompressor = ipc;
				}
			}
			return 0;
		}
		ipc = ipc->next;
	}
	return -EINVAL;
}