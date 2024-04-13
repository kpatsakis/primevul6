connection_dirserv_finish_spooling(dir_connection_t *conn)
{
  if (conn->zlib_state) {
    connection_write_to_buf_zlib("", 0, conn, 1);
    tor_zlib_free(conn->zlib_state);
    conn->zlib_state = NULL;
  }
  conn->dir_spool_src = DIR_SPOOL_NONE;
  return 0;
}