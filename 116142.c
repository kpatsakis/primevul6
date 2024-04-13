void format_arg(Formatter&, ...) {
  FMT_STATIC_ASSERT(FalseType<Formatter>::value,
                    "Cannot format argument. To enable the use of ostream "
                    "operator<< include fmt/ostream.h. Otherwise provide "
                    "an overload of format_arg.");
}