router_clear_status_flags(routerinfo_t *router)
{
  router->is_valid = router->is_running = router->is_hs_dir =
    router->is_fast = router->is_stable =
    router->is_possible_guard = router->is_exit =
    router->is_bad_exit = router->is_bad_directory = 0;
}