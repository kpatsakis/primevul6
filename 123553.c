void ContentSettingsObserver::ClearBlockedContentSettings() {
  cached_storage_permissions_.clear();
  cached_script_permissions_.clear();
}