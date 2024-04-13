bool ContentSettingsObserver::IsWhitelistedForContentSettings(
    const WebSecurityOrigin& origin,
    const GURL& document_url) {
  if (document_url == GURL(content::kUnreachableWebDataURL))
    return true;

  if (origin.IsUnique())
    return false;  // Uninitialized document?

  base::string16 protocol = origin.Protocol().Utf16();
  if (base::EqualsASCII(protocol, content::kChromeUIScheme))
    return true;  // Browser UI elements should still work.

  if (base::EqualsASCII(protocol, content::kChromeDevToolsScheme))
    return true;  // DevTools UI elements should still work.

#if BUILDFLAG(ENABLE_EXTENSIONS)
  if (base::EqualsASCII(protocol, extensions::kExtensionScheme))
    return true;
#endif

  // If the scheme is file:, an empty file name indicates a directory listing,
  // which requires JavaScript to function properly.
  if (base::EqualsASCII(protocol, url::kFileScheme)) {
    return document_url.SchemeIs(url::kFileScheme) &&
           document_url.ExtractFileName().empty();
  }

  return false;
}