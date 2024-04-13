void ndpi_ptree_destroy(ndpi_ptree_t *tree) {
  if(tree) {
    if(tree->v4)
      ndpi_Destroy_Patricia(tree->v4, free_ptree_data);
    if(tree->v6)
      ndpi_Destroy_Patricia(tree->v6, free_ptree_data);

    ndpi_free(tree);
  }
}