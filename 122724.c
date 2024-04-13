ssl_read_from_net(SSLNetVConnection *sslvc, EThread *lthread, int64_t &ret)
{
  NetState *s            = &sslvc->read;
  MIOBufferAccessor &buf = s->vio.buffer;
  int event              = SSL_READ_ERROR_NONE;
  int64_t bytes_read     = 0;
  ssl_error_t sslErr     = SSL_ERROR_NONE;

  bool trace     = sslvc->getSSLTrace();
  int64_t toread = buf.writer()->write_avail();
  ink_release_assert(toread > 0);
  if (toread > s->vio.ntodo()) {
    toread = s->vio.ntodo();
  }

  bytes_read = 0;
  while (sslErr == SSL_ERROR_NONE && bytes_read < toread) {
    int64_t nread             = 0;
    int64_t block_write_avail = buf.writer()->block_write_avail();
    ink_release_assert(block_write_avail > 0);
    int64_t amount_to_read = toread - bytes_read;
    if (amount_to_read > block_write_avail) {
      amount_to_read = block_write_avail;
    }

    Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] b->write_avail()=%" PRId64, amount_to_read);
    char *current_block = buf.writer()->end();
    ink_release_assert(current_block != nullptr);
    sslErr = SSLReadBuffer(sslvc->ssl, current_block, amount_to_read, nread);

    Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] nread=%d", (int)nread);
    if (!sslvc->origin_trace) {
      TraceIn((0 < nread && trace), sslvc->get_remote_addr(), sslvc->get_remote_port(), "WIRE TRACE\tbytes=%d\n%.*s", (int)nread,
              (int)nread, current_block);
    } else {
      char origin_trace_ip[INET6_ADDRSTRLEN];
      ats_ip_ntop(sslvc->origin_trace_addr, origin_trace_ip, sizeof(origin_trace_ip));
      TraceIn((0 < nread && trace), sslvc->get_remote_addr(), sslvc->get_remote_port(), "CLIENT %s:%d\ttbytes=%d\n%.*s",
              origin_trace_ip, sslvc->origin_trace_port, (int)nread, (int)nread, current_block);
    }

    switch (sslErr) {
    case SSL_ERROR_NONE:

#if DEBUG
      SSLDebugBufferPrint("ssl_buff", current_block, nread, "SSL Read");
#endif
      ink_assert(nread);
      bytes_read += nread;
      if (nread > 0) {
        buf.writer()->fill(nread); // Tell the buffer, we've used the bytes
        sslvc->netActivity(lthread);
      }
      break;
    case SSL_ERROR_WANT_WRITE:
      event = SSL_WRITE_WOULD_BLOCK;
      SSL_INCREMENT_DYN_STAT(ssl_error_want_write);
      Debug("ssl.error", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_WOULD_BLOCK(write)");
      break;
    case SSL_ERROR_WANT_READ:
      event = SSL_READ_WOULD_BLOCK;
      SSL_INCREMENT_DYN_STAT(ssl_error_want_read);
      Debug("ssl.error", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_WOULD_BLOCK(read)");
      break;
    case SSL_ERROR_WANT_X509_LOOKUP:
      TraceIn(trace, sslvc->get_remote_addr(), sslvc->get_remote_port(), "Want X509 lookup");
      event = SSL_READ_WOULD_BLOCK;
      SSL_INCREMENT_DYN_STAT(ssl_error_want_x509_lookup);
      Debug("ssl.error", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_WOULD_BLOCK(read/x509 lookup)");
      break;
    case SSL_ERROR_SYSCALL:
      TraceIn(trace, sslvc->get_remote_addr(), sslvc->get_remote_port(), "Syscall Error: %s", strerror(errno));
      SSL_INCREMENT_DYN_STAT(ssl_error_syscall);
      if (nread != 0) {
        // not EOF
        event = SSL_READ_ERROR;
        ret   = errno;
        Debug("ssl.error", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_SYSCALL, underlying IO error: %s", strerror(errno));
        TraceIn(trace, sslvc->get_remote_addr(), sslvc->get_remote_port(), "Underlying IO error: %d", errno);
      } else {
        // then EOF observed, treat it as EOS
        // Error("[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_SYSCALL, EOF observed violating SSL protocol");
        TraceIn(trace, sslvc->get_remote_addr(), sslvc->get_remote_port(), "EOF observed violating SSL protocol");
        event = SSL_READ_EOS;
      }
      break;
    case SSL_ERROR_ZERO_RETURN:
      TraceIn(trace, sslvc->get_remote_addr(), sslvc->get_remote_port(), "Connection closed by peer");
      event = SSL_READ_EOS;
      SSL_INCREMENT_DYN_STAT(ssl_error_zero_return);
      Debug("ssl.error", "[SSL_NetVConnection::ssl_read_from_net] SSL_ERROR_ZERO_RETURN");
      break;
    case SSL_ERROR_SSL:
    default: {
      char buf[512];
      unsigned long e = ERR_peek_last_error();
      ERR_error_string_n(e, buf, sizeof(buf));
      TraceIn(trace, sslvc->get_remote_addr(), sslvc->get_remote_port(), "SSL Error: sslErr=%d, ERR_get_error=%ld (%s) errno=%d",
              sslErr, e, buf, errno);
      event = SSL_READ_ERROR;
      ret   = errno;
      SSL_CLR_ERR_INCR_DYN_STAT(sslvc, ssl_error_ssl, "[SSL_NetVConnection::ssl_read_from_net]: errno=%d", errno);
    } break;
    } // switch
  }   // while

  if (bytes_read > 0) {
    Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] bytes_read=%" PRId64, bytes_read);

    s->vio.ndone += bytes_read;
    sslvc->netActivity(lthread);

    ret = bytes_read;

    // If we read it all, don't worry about the other events and just send read complete
    event = (s->vio.ntodo() <= 0) ? SSL_READ_COMPLETE : SSL_READ_READY;
    if (sslErr == SSL_ERROR_NONE && s->vio.ntodo() > 0) {
      // We stopped with data on the wire (to avoid overbuffering).  Make sure we are triggered
      sslvc->read.triggered = 1;
    }
  } else { // if( bytes_read > 0 )
#if defined(_DEBUG)
    if (bytes_read == 0) {
      Debug("ssl", "[SSL_NetVConnection::ssl_read_from_net] bytes_read == 0");
    }
#endif
  }
  return event;
}