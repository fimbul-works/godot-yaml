#include <c4/yml/node.hpp>

namespace c4 {
namespace yml {

size_t to_chars(substr buf, const NodeRef& node) {
    return to_chars(buf, node.val());
}

size_t to_chars(substr buf, const ConstNodeRef& node) {
    return to_chars(buf, node.val());
}

} // namespace yml
} // namespace c4
