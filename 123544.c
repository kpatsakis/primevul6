bool ContentSettingsObserver::AllowDatabase(const WebString& name,
                                          const WebString& display_name,
                                          unsigned estimated_size) {  // NOLINT
  WebFrame* frame = render_frame()->GetWebFrame();
  if (frame->GetSecurityOrigin().IsUnique() ||
      frame->Top()->GetSecurityOrigin().IsUnique())
    return false;

  bool allow = true;
  GURL secondary_url(
      blink::WebStringToGURL(frame->GetSecurityOrigin().ToString()));
  if (content_settings_manager_->content_settings()) {
    allow =
        content_settings_manager_->GetSetting(
          ContentSettingsManager::GetOriginOrURL(frame),
          secondary_url,
          "cookies",
          allow) != CONTENT_SETTING_BLOCK;
  }

  if (!allow)
    DidBlockContentType("database", secondary_url.spec());
  return allow;
}