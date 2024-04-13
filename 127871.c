slhc_compress(struct slcompress *comp, unsigned char *icp, int isize,
	unsigned char *ocp, unsigned char **cpp, int compress_cid)
{
  printk(KERN_DEBUG "Called IP function on non IP-system: slhc_compress");
  return -EINVAL;
}