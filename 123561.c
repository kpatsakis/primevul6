bool ContentSettingsObserver::AllowAutoplay(bool default_value) {
  bool allow = default_value;
  if (content_settings_manager_->content_settings()) {
    WebFrame* frame = render_frame()->GetWebFrame();
    auto origin = frame->ToWebLocalFrame()->GetDocument().GetSecurityOrigin();
    allow =
        content_settings_manager_->GetSetting(
            ContentSettingsManager::GetOriginOrURL(frame),
            blink::WebStringToGURL(origin.ToString()),
            "autoplay",
            allow) != CONTENT_SETTING_BLOCK;
  }

  if (!allow)
    DidBlockContentType(CONTENT_SETTINGS_TYPE_AUTOPLAY);
  return allow;
}