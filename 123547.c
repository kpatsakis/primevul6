void ContentSettingsObserver::DidNotAllowScript() {
  DidBlockContentType(CONTENT_SETTINGS_TYPE_JAVASCRIPT);
}