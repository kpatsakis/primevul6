directory_remove_invalid(void)
{
  int i;
  int changed = 0;
  routerlist_t *rl = router_get_routerlist();

  routerlist_assert_ok(rl);

  for (i = 0; i < smartlist_len(rl->routers); ++i) {
    const char *msg;
    routerinfo_t *ent = smartlist_get(rl->routers, i);
    char description[NODE_DESC_BUF_LEN];
    uint32_t r = dirserv_router_get_status(ent, &msg);
    router_get_description(description, ent);
    if (r & FP_REJECT) {
      log_info(LD_DIRSERV, "Router %s is now rejected: %s",
               description, msg?msg:"");
      routerlist_remove(rl, ent, 0, time(NULL));
      i--;
      changed = 1;
      continue;
    }
    if (bool_neq((r & FP_NAMED), ent->is_named)) {
      log_info(LD_DIRSERV,
               "Router %s is now %snamed.", description,
               (r&FP_NAMED)?"":"un");
      ent->is_named = (r&FP_NAMED)?1:0;
      changed = 1;
    }
    if (bool_neq((r & FP_INVALID), !ent->is_valid)) {
      log_info(LD_DIRSERV, "Router '%s' is now %svalid.", description,
               (r&FP_INVALID) ? "in" : "");
      ent->is_valid = (r&FP_INVALID)?0:1;
      changed = 1;
    }
    if (bool_neq((r & FP_BADDIR), ent->is_bad_directory)) {
      log_info(LD_DIRSERV, "Router '%s' is now a %s directory", description,
               (r & FP_BADDIR) ? "bad" : "good");
      ent->is_bad_directory = (r&FP_BADDIR) ? 1: 0;
      changed = 1;
    }
    if (bool_neq((r & FP_BADEXIT), ent->is_bad_exit)) {
      log_info(LD_DIRSERV, "Router '%s' is now a %s exit", description,
               (r & FP_BADEXIT) ? "bad" : "good");
      ent->is_bad_exit = (r&FP_BADEXIT) ? 1: 0;
      changed = 1;
    }
  }
  if (changed)
    directory_set_dirty();

  routerlist_assert_ok(rl);
}