static void color_tree_add(ColorTree* tree,
                           unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned index)
{
  int bit;
  for(bit = 0; bit < 8; bit++)
  {
    int i = 8 * ((r >> bit) & 1) + 4 * ((g >> bit) & 1) + 2 * ((b >> bit) & 1) + 1 * ((a >> bit) & 1);
    if(!tree->children[i])
    {
      tree->children[i] = (ColorTree*)malloc(sizeof(ColorTree));
      color_tree_init(tree->children[i]);
    }
    tree = tree->children[i];
  }
  tree->index = (int)index;
}