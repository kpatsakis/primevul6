Variant HHVM_FUNCTION(imageaffinematrixget,
                      int64_t type,
                      const Variant& options /* = Array() */) {
  Array ret = Array::Create();
  double affine[6];
  int res = GD_FALSE, i;

  switch((gdAffineStandardMatrix)type) {
    case GD_AFFINE_TRANSLATE:
    case GD_AFFINE_SCALE: {
      double x, y;
      Array aoptions = options.toArray();
      if (aoptions.empty()) {
        raise_warning("imageaffinematrixget(): Array expected as options");
        return false;
      }
      if (aoptions.exists(s_x)) {
        x = aoptions[s_x].toDouble();
      } else {
        raise_warning("imageaffinematrixget(): Missing x position");
        return false;
      }
      if (aoptions.exists(s_y)) {
        y = aoptions[s_y].toDouble();
      } else {
        raise_warning("imageaffinematrixget(): Missing x position");
        return false;
      }

      if (type == GD_AFFINE_TRANSLATE) {
        res = gdAffineTranslate(affine, x, y);
      } else {
        res = gdAffineScale(affine, x, y);
      }
      break;
    }

    case GD_AFFINE_ROTATE:
    case GD_AFFINE_SHEAR_HORIZONTAL:
    case GD_AFFINE_SHEAR_VERTICAL: {
      double angle;
      double doptions = options.toDouble();
      if (!doptions) {
        raise_warning("imageaffinematrixget(): Number is expected as option");
        return false;
      }

      angle = doptions;

      if (type == GD_AFFINE_SHEAR_HORIZONTAL) {
        res = gdAffineShearHorizontal(affine, angle);
      } else if (type == GD_AFFINE_SHEAR_VERTICAL) {
        res = gdAffineShearVertical(affine, angle);
      } else {
        res = gdAffineRotate(affine, angle);
      }
      break;
    }

    default:
      raise_warning("imageaffinematrixget():Invalid type for "
                    "element %" PRId64, type);
      return false;
  }

  if (res == GD_FALSE) {
    return false;
  } else {
    for (i = 0; i < 6; i++) {
      ret.set(String(i, CopyString), affine[i]);
    }
  }
  return ret;
}