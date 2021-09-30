

#pragma once

#include <vector>

namespace candidate_index {

/// This is an element of the join's result set.
struct InvertedListElement {
  /// Tree id of the left-hand tree in the result element.
  int offset = 0;
  /// Tree id of the right-hand tree in the result element.
  std::vector<std::pair<int, int>> element_list;
  /// Constructor.
  InvertedListElement();
};

candidate_index::InvertedListElement::InvertedListElement() {}

}
