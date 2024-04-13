static int getTreeInflateFixed(HuffmanTree* tree_ll, HuffmanTree* tree_d)
{
  int rc;

  rc = generateFixedLitLenTree(tree_ll);
  if (rc) return rc;
  return generateFixedDistanceTree(tree_d);
}