get_replacement_map (class ipa_node_params *info, tree value, int parm_num)
{
  struct ipa_replace_map *replace_map;


  replace_map = ggc_alloc<ipa_replace_map> ();
  if (dump_file)
    {
      fprintf (dump_file, "    replacing ");
      ipa_dump_param (dump_file, info, parm_num);

      fprintf (dump_file, " with const ");
      print_generic_expr (dump_file, value);
      fprintf (dump_file, "\n");
    }
  replace_map->parm_num = parm_num;
  replace_map->new_tree = value;
  return replace_map;
}