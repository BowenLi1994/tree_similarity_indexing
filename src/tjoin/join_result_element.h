#pragma once

namespace join {

/// This is an element of the join's result set.
struct JoinResultElement {
  /// Tree id of the left-hand tree in the result element.
  int tree_id_1;
  /// Tree id of the right-hand tree in the result element.
  int tree_id_2;
  /// Tree edit distance between tree_id_1 and tree_id_2.
  double ted_value;
  /// Constructor.
  JoinResultElement(int tree_id_1, int tree_id_2, double ted_value);
};

join::JoinResultElement::JoinResultElement(int tree_id_1, int tree_id_2, double ted_value)
    : tree_id_1(tree_id_1), tree_id_2(tree_id_2), ted_value(ted_value) {}

}
