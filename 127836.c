slhc_uncompress(struct slcompress *comp, unsigned char *icp, int isize)
{
  printk(KERN_DEBUG "Called IP function on non IP-system: slhc_uncompress");
  return -EINVAL;
}