slhc_toss(struct slcompress *comp)
{
  printk(KERN_DEBUG "Called IP function on non IP-system: slhc_toss");
  return -EINVAL;
}