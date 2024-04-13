void ContentSettingsObserver::RequestFileSystemAccessAsync(
        const WebContentSettingCallbacks& callbacks) {
  WebFrame* frame = render_frame()->GetWebFrame();
  WebContentSettingCallbacks permissionCallbacks(callbacks);
  if (frame->GetSecurityOrigin().IsUnique() ||
      frame->Top()->GetSecurityOrigin().IsUnique()) {
      permissionCallbacks.DoDeny();
      return;
  }

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
  if (!allow) {
      DidBlockContentType("filesystem", secondary_url.spec());
      permissionCallbacks.DoDeny();
  } else {
      permissionCallbacks.DoAllow();
  }
}