static void color_tree_init(ColorTree* tree)
{
  int i;
  for(i = 0; i < 16; i++) tree->children[i] = 0;
  tree->index = -1;
}