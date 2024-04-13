static void HuffmanTree_cleanup(HuffmanTree* tree)
{
  free(tree->tree2d);
  free(tree->tree1d);
  free(tree->lengths);
}