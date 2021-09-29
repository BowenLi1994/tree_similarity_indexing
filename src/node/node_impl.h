/// \file node/node_impl.h
///
/// \details
/// Contains the implementation of the Node class.

#pragma once
using namespace node;

template<class Label>
Node<Label>::Node(ConstReference label) : label_(label) {}

template<class Label>
const typename Node<Label>::SizeType Node<Label>::children_count() const {
  return children_.size();
}

template<class Label>
typename Node<Label>::ConstReference Node<Label>::label() const {
  return label_;
}

template<class Label>
Node<Label>& Node<Label>::add_child(Node<Label> child) {
  children_.push_back(child);
  return children_.back();
}

template<class Label>
const std::vector<Node<Label>>& Node<Label>::get_children() const {
  return children_;
}

template<class Label>
int Node<Label>::get_tree_size() const {
  int size = 1;
  for (const auto& child : children_) {
    size += child.get_tree_size();
  }
  return size;
}

template<class Label>
const std::vector<std::string> Node<Label>::get_all_labels() const {
  std::vector<std::string> labels;
  get_all_labels_recursion(labels);
  return labels;
}
template<class Label>
void Node<Label>::get_all_labels_recursion(std::vector<std::string>& labels) const {
  labels.push_back(label_.to_string());
  for (const auto& child : children_) {
    child.get_all_labels_recursion(labels);
  }
}

template<class Label>
bool Node<Label>::is_leaf() const {
  return children_.size() == 0;
}
