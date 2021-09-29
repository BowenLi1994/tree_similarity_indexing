/// \file node/node.h
///
/// \details
/// Contains the declaration of the Node class (represents a node in a tree).

#pragma once

#include <vector>
#include <string>

namespace node {

template <class Label>
class Node {
// Types and type aliases
public:
  using Reference = Label&;
  using ConstReference = const Label&;

  using SizeType = typename std::vector<Node<Label>>::size_type;

// Member functions
public:
  Node(ConstReference label);

  /// Returns the number of children of this node.
  ///
  /// \return Number of children (i.e., entries in children_).
  const SizeType children_count() const;

  /// Retrieves the label.
  ///
  /// \return The label of this node.
  ConstReference label() const;

  /// Adds a child at last position.
  /// TODO: Verify this definition. Can we improve it with & or &&? It is
  ///       used to build the tree in a parser.
  /// TODO: Verify if child is copied or moved.
  ///
  /// \param Node to be added.
  Node<Label>& add_child(Node<Label> child);

  /// Returns the reference to children_ vector.
  ///
  /// \return Reference to children_ vector.
  const std::vector<Node<Label>>& get_children() const;

  /// Calculates the size (number of nodes) in tree rooted at this. It traverses
  /// the entire subtree recursively.
  ///
  /// \return Size of subtree rooted at this.
  int get_tree_size() const;

  /// Traverses the tree and collects string representations of all labels.
  ///
  /// \return Vector with labels of all nodes.
  const std::vector<std::string> get_all_labels() const;
  void get_all_labels_recursion(std::vector<std::string>& labels) const;

  /// Verifies if this node is a leaf.
  ///
  /// \return True if this node is leaf, false otherwise.
  bool is_leaf() const;

// Member variables
private:
  /// All children of this node.
  std::vector<Node<Label>> children_;

  /// Data representing the label of this node. Only this Node object owns it.
  Label label_;
};

// Implementation details
#include "node_impl.h"

}
