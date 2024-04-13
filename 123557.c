void ContentSettingsObserver::DidCommitProvisionalLoad(
    bool is_new_navigation,
    bool is_same_page_navigation) {
  WebFrame* frame = render_frame()->GetWebFrame();
  if (frame->Parent())
    return;  // Not a top-level navigation.

  if (!is_same_page_navigation) {
    ClearBlockedContentSettings();
    temporarily_allowed_plugins_.clear();
  }
}