slhc_remember(struct slcompress *comp, unsigned char *icp, int isize)
{
  printk(KERN_DEBUG "Called IP function on non IP-system: slhc_remember");
  return -EINVAL;
}