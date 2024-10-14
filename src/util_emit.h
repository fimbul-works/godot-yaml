#ifndef UTIL_EMIT_H
#define UTIL_EMIT_H

#include "yaml.h"

#include "util_numeric.h"

template <typename T>
void emit_float(ryml::NodeRef node, const T num)
{
  node << float_to_string(num);
}

template <typename T>
void emit_int(ryml::NodeRef node, const T num)
{
  node << int_to_string(num);
}

#endif // UTIL_EMIT_H
