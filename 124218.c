static inline bool authorized_localhost_users_login_disabled(const struct mg_connection *conn) {
  if(ntop->getPrefs()->is_localhost_users_login_disabled()
     && (conn->request_info.remote_ip == 0x7F000001 /* 127.0.0.1 */))
    return true;
  return false;
}