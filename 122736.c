make_ssl_connection(SSL_CTX *ctx, SSLNetVConnection *netvc)
{
  SSL *ssl;

  if (likely(ssl = SSL_new(ctx))) {
    netvc->ssl = ssl;

    // Only set up the bio stuff for the server side
    if (netvc->get_context() == NET_VCONNECTION_OUT) {
      BIO *bio = BIO_new(const_cast<BIO_METHOD *>(BIO_s_fastopen()));
      BIO_set_fd(bio, netvc->get_socket(), BIO_NOCLOSE);

      if (netvc->options.f_tcp_fastopen) {
        BIO_set_conn_address(bio, netvc->get_remote_addr());
      }

      SSL_set_bio(ssl, bio, bio);
    } else {
      netvc->initialize_handshake_buffers();
      BIO *rbio = BIO_new(BIO_s_mem());
      BIO *wbio = BIO_new_fd(netvc->get_socket(), BIO_NOCLOSE);
      BIO_set_mem_eof_return(wbio, -1);
      SSL_set_bio(ssl, rbio, wbio);
    }

    SSLNetVCAttach(ssl, netvc);
  }

  return ssl;
}