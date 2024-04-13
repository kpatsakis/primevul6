flatpak_context_add_shares (FlatpakContext      *context,
                            FlatpakContextShares shares)
{
  context->shares_valid |= shares;
  context->shares |= shares;
}