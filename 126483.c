static void w3(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
  LOG(LL_INFO, ("ev %d", ev));
  if (ev == MG_EV_WS_OPEN) {
    mg_ws_send(c, "hi there!", 9, WEBSOCKET_OP_TEXT);
  } else if (ev == MG_EV_WS_MSG) {
    struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
    ASSERT(mg_strcmp(wm->data, mg_str("lebowski")) == 0);
    ((int *) fn_data)[0]++;
  } else if (ev == MG_EV_CLOSE) {
    ((int *) fn_data)[0] += 10;
  }
}