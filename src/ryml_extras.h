#include <c4/yml/node.hpp>

namespace c4 {
namespace yml {

size_t to_chars(substr buf, const NodeRef& node);
size_t to_chars(substr buf, const ConstNodeRef& node);

} // namespace yml
} // namespace c4
