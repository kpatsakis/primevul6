method_to_str(unsigned short m) {
  switch (m) {
    case HTTP_DELETE:     return delete_sym;
    case HTTP_GET:        return get_sym;
    case HTTP_HEAD:       return head_sym;
    case HTTP_POST:       return post_sym;
    case HTTP_PUT:        return put_sym;
    case HTTP_CONNECT:    return connect_sym;
    case HTTP_OPTIONS:    return options_sym;
    case HTTP_TRACE:      return trace_sym;
    case HTTP_PATCH:      return patch_sym;
    case HTTP_COPY:       return copy_sym;
    case HTTP_LOCK:       return lock_sym;
    case HTTP_MKCOL:      return mkcol_sym;
    case HTTP_MOVE:       return move_sym;
    case HTTP_PROPFIND:   return propfind_sym;
    case HTTP_PROPPATCH:  return proppatch_sym;
    case HTTP_UNLOCK:     return unlock_sym;
    case HTTP_REPORT:     return report_sym;
    case HTTP_MKACTIVITY: return mkactivity_sym;
    case HTTP_CHECKOUT:   return checkout_sym;
    case HTTP_MERGE:      return merge_sym;
    case HTTP_MSEARCH:    return msearch_sym;
    case HTTP_NOTIFY:     return notify_sym;
    case HTTP_SUBSCRIBE:  return subscribe_sym;
    case HTTP_UNSUBSCRIBE:return unsubscribe_sym;
    default:              return unknown_method_sym;
  }
}