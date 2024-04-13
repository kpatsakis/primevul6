bool ContentSettingsObserver::IsWhitelistedForContentSettings() const {
  // Whitelist ftp directory listings, as they require JavaScript to function
  // properly.
  if (!render_frame()) {
    return false;
  }

  if (render_frame()->IsFTPDirectoryListing())
    return true;

  WebFrame* web_frame = render_frame()->GetWebFrame();

  if (!web_frame) {
    return false;
  }

  return IsWhitelistedForContentSettings(
      web_frame->ToWebLocalFrame()->GetDocument().GetSecurityOrigin(),
      web_frame->ToWebLocalFrame()->GetDocument().Url());
}