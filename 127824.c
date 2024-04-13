static void isdn_ppp_mp_print_recv_pkt(int slot, struct sk_buff *skb)
{
	printk(KERN_DEBUG "mp_recv: %d/%d -> %02x %02x %02x %02x %02x %02x\n",
	       slot, (int) skb->len,
	       (int) skb->data[0], (int) skb->data[1], (int) skb->data[2],
	       (int) skb->data[3], (int) skb->data[4], (int) skb->data[5]);
}