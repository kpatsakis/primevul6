ndpi_ptree_t *ndpi_ptree_create(void) {
  ndpi_ptree_t *tree = (ndpi_ptree_t *) ndpi_malloc(sizeof(ndpi_ptree_t));

  if(tree) {
    tree->v4 = ndpi_New_Patricia(32);
    tree->v6 = ndpi_New_Patricia(128);

    if((!tree->v4) || (!tree->v6)) {
      ndpi_ptree_destroy(tree);
      return(NULL);
    }
  }

  return(tree);
}