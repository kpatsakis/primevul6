gimp_channel_class_init (GimpChannelClass *klass)
{
  GObjectClass      *object_class      = G_OBJECT_CLASS (klass);
  GimpObjectClass   *gimp_object_class = GIMP_OBJECT_CLASS (klass);
  GimpViewableClass *viewable_class    = GIMP_VIEWABLE_CLASS (klass);
  GimpFilterClass   *filter_class      = GIMP_FILTER_CLASS (klass);
  GimpItemClass     *item_class        = GIMP_ITEM_CLASS (klass);
  GimpDrawableClass *drawable_class    = GIMP_DRAWABLE_CLASS (klass);

  channel_signals[COLOR_CHANGED] =
    g_signal_new ("color-changed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (GimpChannelClass, color_changed),
                  NULL, NULL,
                  gimp_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  object_class->finalize            = gimp_channel_finalize;

  gimp_object_class->get_memsize    = gimp_channel_get_memsize;

  viewable_class->get_description   = gimp_channel_get_description;
  viewable_class->default_icon_name = "gimp-channel";

  filter_class->get_node            = gimp_channel_get_node;

  item_class->is_attached          = gimp_channel_is_attached;
  item_class->get_tree             = gimp_channel_get_tree;
  item_class->bounds               = gimp_channel_bounds;
  item_class->duplicate            = gimp_channel_duplicate;
  item_class->convert              = gimp_channel_convert;
  item_class->translate            = gimp_channel_translate;
  item_class->scale                = gimp_channel_scale;
  item_class->resize               = gimp_channel_resize;
  item_class->get_clip             = gimp_channel_get_clip;
  item_class->fill                 = gimp_channel_fill;
  item_class->stroke               = gimp_channel_stroke;
  item_class->to_selection         = gimp_channel_to_selection;
  item_class->default_name         = _("Channel");
  item_class->rename_desc          = C_("undo-type", "Rename Channel");
  item_class->translate_desc       = C_("undo-type", "Move Channel");
  item_class->scale_desc           = C_("undo-type", "Scale Channel");
  item_class->resize_desc          = C_("undo-type", "Resize Channel");
  item_class->flip_desc            = C_("undo-type", "Flip Channel");
  item_class->rotate_desc          = C_("undo-type", "Rotate Channel");
  item_class->transform_desc       = C_("undo-type", "Transform Channel");
  item_class->fill_desc            = C_("undo-type", "Fill Channel");
  item_class->stroke_desc          = C_("undo-type", "Stroke Channel");
  item_class->to_selection_desc    = C_("undo-type", "Channel to Selection");
  item_class->reorder_desc         = C_("undo-type", "Reorder Channel");
  item_class->raise_desc           = C_("undo-type", "Raise Channel");
  item_class->raise_to_top_desc    = C_("undo-type", "Raise Channel to Top");
  item_class->lower_desc           = C_("undo-type", "Lower Channel");
  item_class->lower_to_bottom_desc = C_("undo-type", "Lower Channel to Bottom");
  item_class->raise_failed         = _("Channel cannot be raised higher.");
  item_class->lower_failed         = _("Channel cannot be lowered more.");

  drawable_class->convert_type          = gimp_channel_convert_type;
  drawable_class->invalidate_boundary   = gimp_channel_invalidate_boundary;
  drawable_class->get_active_components = gimp_channel_get_active_components;
  drawable_class->set_buffer            = gimp_channel_set_buffer;

  klass->boundary       = gimp_channel_real_boundary;
  klass->is_empty       = gimp_channel_real_is_empty;
  klass->feather        = gimp_channel_real_feather;
  klass->sharpen        = gimp_channel_real_sharpen;
  klass->clear          = gimp_channel_real_clear;
  klass->all            = gimp_channel_real_all;
  klass->invert         = gimp_channel_real_invert;
  klass->border         = gimp_channel_real_border;
  klass->grow           = gimp_channel_real_grow;
  klass->shrink         = gimp_channel_real_shrink;
  klass->flood          = gimp_channel_real_flood;

  klass->feather_desc   = C_("undo-type", "Feather Channel");
  klass->sharpen_desc   = C_("undo-type", "Sharpen Channel");
  klass->clear_desc     = C_("undo-type", "Clear Channel");
  klass->all_desc       = C_("undo-type", "Fill Channel");
  klass->invert_desc    = C_("undo-type", "Invert Channel");
  klass->border_desc    = C_("undo-type", "Border Channel");
  klass->grow_desc      = C_("undo-type", "Grow Channel");
  klass->shrink_desc    = C_("undo-type", "Shrink Channel");
  klass->flood_desc     = C_("undo-type", "Flood Channel");
}