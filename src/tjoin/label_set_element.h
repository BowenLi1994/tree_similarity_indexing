#pragma once

#include <vector>

namespace label_set_converter {

/// This is an element holding positional data of a set element.
struct StructuralVector {
  /// Postorder ID of the label in the according tree.
  int postorder_id = 0;
  /// Number of nodes to the left of a node.
  int number_nodes_left = 0;
  /// Number of nodes to the right of a node.
  int number_nodes_right = 0;
  /// Number of ancestors of a node.
  int number_nodes_ancestor = 0;
  /// Number of descendants of a node.
  int number_nodes_descendant = 0;
  /// Constructor.
  StructuralVector();
  /// Constructor.
  StructuralVector(int postorder_id, 
          int number_nodes_left, int number_nodes_right, 
          int number_nodes_ancestor, int number_nodes_descendant);
};

label_set_converter::StructuralVector::StructuralVector() {}

label_set_converter::StructuralVector::StructuralVector(
  int postorder_id, int number_nodes_left, 
  int number_nodes_right, int number_nodes_ancestor, int number_nodes_descendant)
    : postorder_id(postorder_id),
      number_nodes_left(number_nodes_left), 
      number_nodes_right(number_nodes_right),
      number_nodes_ancestor(number_nodes_ancestor), 
      number_nodes_descendant(number_nodes_descendant) {}


/// This is an element holding relevant data of a set.
struct LabelSetElement {
  /// Token ID.
  int id = 0;
  /// Weight of a token.
  int weight = 0;
  /// Weight of a token.
  int weight_so_far = 0;
  /// Overlap so far for a specific set.
  std::vector<label_set_converter::StructuralVector> struct_vect;
  /// Constructor.
  LabelSetElement();
  /// Constructor.
  LabelSetElement(int id, int weight);
  /// Constructor.
  LabelSetElement(int id, int weight, 
      std::vector<label_set_converter::StructuralVector> &&struct_vect);
};

label_set_converter::LabelSetElement::LabelSetElement() {}

label_set_converter::LabelSetElement::LabelSetElement(int id, int weight)
    : id(id), weight(weight) {}

label_set_converter::LabelSetElement::LabelSetElement(int id, int weight, 
      std::vector<label_set_converter::StructuralVector> &&struct_vect)
    : id(id), weight(weight), struct_vect(std::move(struct_vect)) {}
}
