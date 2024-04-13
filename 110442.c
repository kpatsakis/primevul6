static int mwifiex_pcie_init_evt_ring(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;
	struct mwifiex_evt_buf_desc *desc;
	struct sk_buff *skb;
	dma_addr_t buf_pa;
	int i;

	for (i = 0; i < MWIFIEX_MAX_EVT_BD; i++) {
		/* Allocate skb here so that firmware can DMA data from it */
		skb = dev_alloc_skb(MAX_EVENT_SIZE);
		if (!skb) {
			mwifiex_dbg(adapter, ERROR,
				    "Unable to allocate skb for EVENT buf.\n");
			kfree(card->evtbd_ring_vbase);
			return -ENOMEM;
		}
		skb_put(skb, MAX_EVENT_SIZE);

		if (mwifiex_map_pci_memory(adapter, skb, MAX_EVENT_SIZE,
					   PCI_DMA_FROMDEVICE))
			return -1;

		buf_pa = MWIFIEX_SKB_DMA_ADDR(skb);

		mwifiex_dbg(adapter, EVENT,
			    "info: EVT ring: skb=%p len=%d data=%p buf_pa=%#x:%x\n",
			    skb, skb->len, skb->data, (u32)buf_pa,
			    (u32)((u64)buf_pa >> 32));

		card->evt_buf_list[i] = skb;
		card->evtbd_ring[i] = (void *)(card->evtbd_ring_vbase +
				      (sizeof(*desc) * i));
		desc = card->evtbd_ring[i];
		desc->paddr = buf_pa;
		desc->len = (u16)skb->len;
		desc->flags = 0;
	}

	return 0;
}