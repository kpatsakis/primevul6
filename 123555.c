bool ContentSettingsObserver::AllowStorage(bool local) {
  if (IsWhitelistedForContentSettings())
    return true;

  WebFrame* frame = render_frame()->GetWebFrame();
  if (frame->GetSecurityOrigin().IsUnique() ||
      frame->Top()->GetSecurityOrigin().IsUnique())
    return false;

  StoragePermissionsKey key(
      blink::WebStringToGURL(frame->ToWebLocalFrame()->GetDocument().GetSecurityOrigin().ToString()),
      local);
  std::map<StoragePermissionsKey, bool>::const_iterator permissions =
      cached_storage_permissions_.find(key);
  if (permissions != cached_storage_permissions_.end())
    return permissions->second;

  bool allow = true;
  if (content_settings_manager_->content_settings()) {
    allow =
        content_settings_manager_->GetSetting(
          ContentSettingsManager::GetOriginOrURL(frame),
          blink::WebStringToGURL(frame->GetSecurityOrigin().ToString()),
          "cookies",
          allow) != CONTENT_SETTING_BLOCK;
  }

  cached_storage_permissions_[key] = allow;
  if (!allow)
    DidBlockContentType("storage");
  return allow;
}