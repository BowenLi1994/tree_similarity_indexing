#pragma once
using namespace label_feature_set_converter;

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::assignFrequencyIdentifiers(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection) {
  // token_map = {(token, tokcnt) -> id}
  typename std::unordered_map<Label, int, labelhash> token_map;
  // token_list = [(#occurrences, data_nr)]
  std::vector<std::pair<int, int>> token_count_list;

  // for each tree in the tree collection
  for(const auto& tree: trees_collection) {
    // record = [tokid]
    std::vector<label_feature_set_converter::LabelSetElement> record;
    // {tokid -> 1}
    std::unordered_map<int, label_feature_set_converter::LabelSetElement> record_labels;
    // number of nodes of the tree
    int tree_size = tree.get_tree_size();

    // postorder id for recursive postorder traversal
    int postorder_id = 0;

    // array of records stored in sets_collection
    create_record(tree, postorder_id, tree_size, token_map, record_labels);

    // reset variables that are needed for positional evaluation
    actual_pre_order_number_ = 0;
    actual_depth_ = 0;

    for (const auto& it : record_labels) 
      record.emplace_back(it.second);

    // add to vector of label sets
    sets_collection.emplace_back(tree_size, record);
  }

  // token_count_list = [tokenfrequency, tokenid]
  for(int i = 0; i < next_token_id_; ++i)
    token_count_list.emplace_back(0, i);
  for(const auto& record: sets_collection)
    for(const auto& token: record.second)
      token_count_list[token.id].first += token.weight;

  // sort token_count_list by tokenfrequency
  std::sort(token_count_list.begin(), token_count_list.end(), pairComparator);

  // tokenmaplist[tokenid] = frequencyID
  std::vector<int> tokenmaplist (token_count_list.size());
  for(std::size_t i = 0; i < token_count_list.size(); ++i)
    tokenmaplist[token_count_list[i].second] = i;

  // substitute the tokenIDs with frequencyIDs
  for(auto& record: sets_collection) {
    for(std::size_t i = 0; i < record.second.size(); ++i)
      record.second[i].id = tokenmaplist[record.second[i].id];

    // sort integers of a record ascending
    std::sort(record.second.begin(), record.second.end(), LabelSetElementComparator);

    // weight of an element in the set up to its position
    int weight_sum = 0;
    for(std::size_t i = 0; i < record.second.size(); ++i) {
      weight_sum += record.second[i].weight;
      record.second[i].weight_so_far = weight_sum;
    }
  }

  // sort all label sets by length (number of elements, not labels)
  // TODO: needed for an unordered input, but messes up the order of an ordered 
  //       input file, that should match in case of a tree similarity join
  // std::sort(sets_collection.begin(), sets_collection.end(), vectorComparator);
}

template<typename Label>
int Converter<Label>::create_record(
    const node::Node<Label>& tree_node, int& postorder_id, int tree_size,
    std::unordered_map<Label, int, labelhash>& token_map, 
    std::unordered_map<int, label_feature_set_converter::LabelSetElement>& record_labels) {

  // number of children = subtree_size - 1
  // subtree_size = 1 -> actual node + sum of children
  int subtree_size = 1;

  // increase depth for children
  ++actual_depth_;

  // do recursively for all children
  for (const auto& child: tree_node.get_children()) {
    subtree_size += create_record(child, postorder_id, tree_size, token_map, record_labels);
  }

  // update postorder id
  ++postorder_id;
  // decrease depth for children
  --actual_depth_;
  // raise pre order number with each node
  ++actual_pre_order_number_;

  // string label of the current node of the node
  Label key = tree_node.label();
  std::string label_str = tree_node.label().to_string();

  // lookup key in token_map
  typename std::unordered_map<Label, int, labelhash>::const_iterator 
                              token_in_map = token_map.find(key);

  //if tokenkey not in map
  if(token_in_map == token_map.end()) {
    // store tokenid in global tokenmap
    token_map.emplace(key, next_token_id_);
    // create new set element
    label_feature_set_converter::LabelSetElement se;
    se.id=next_token_id_;
    se.weight=1;

    label_feature_set_converter::StructuralVector sv;
    sv.postorder_id=postorder_id;
    sv.number_nodes_left=actual_pre_order_number_ - subtree_size;
    sv.number_nodes_right=tree_size - (actual_pre_order_number_ + actual_depth_);
    sv.number_nodes_ancestor=actual_depth_;
    sv.number_nodes_descendant=subtree_size - 1;
    for(int i=0;i<4;i++){
      sv.degree_histogram[i]=tree_node.degree_histogram[i];
      sv.leaf_histogram[i]=tree_node.leaf_histogram[i];
      sv.label_histogram[i]=tree_node.label_histogram[i];
    }

    // add positional information
    se.struct_vect.push_back(sv);
    // append to record (id, weight, left, right, ancestor, descendant)
    record_labels.emplace(next_token_id_, se);
    // update next_token_id_
    next_token_id_ += 1;
  }
  else {
    if(record_labels.find(token_map[key]) == record_labels.end()) {

      label_feature_set_converter::LabelSetElement se;
      se.id=token_map[key];
      se.weight=1;
      
      // add positional information
      label_feature_set_converter::StructuralVector sv;
      sv.postorder_id=postorder_id;
      sv.number_nodes_left=actual_pre_order_number_ - subtree_size;
      sv.number_nodes_right=tree_size - (actual_pre_order_number_ + actual_depth_);
      sv.number_nodes_ancestor=actual_depth_;
      sv.number_nodes_descendant=subtree_size - 1;
      for(int i=0;i<4;i++){
        sv.degree_histogram[i]=tree_node.degree_histogram[i];
        sv.leaf_histogram[i]=tree_node.leaf_histogram[i];
        sv.label_histogram[i]=tree_node.label_histogram[i];
      }
      se.struct_vect.push_back(sv);
      // append to record (id, weight, left, right, ancestor, descendant)
      record_labels.emplace(token_map[key], se);
    } 
    else {
      // increase weight
      ++record_labels[token_map[key]].weight;
      // push position information to its duplicates
      label_feature_set_converter::StructuralVector sv;
      sv.postorder_id=postorder_id;
      sv.number_nodes_left=actual_pre_order_number_ - subtree_size;
      sv.number_nodes_right=tree_size - (actual_pre_order_number_ + actual_depth_);
      sv.number_nodes_ancestor=actual_depth_;
      sv.number_nodes_descendant=subtree_size - 1;
      for(int i=0;i<4;i++){
        sv.degree_histogram[i]=tree_node.degree_histogram[i];
        sv.leaf_histogram[i]=tree_node.leaf_histogram[i];
        sv.label_histogram[i]=tree_node.label_histogram[i];
      }

      record_labels[token_map[key]].struct_vect.push_back(sv);

    }
  }

  return subtree_size;
}

template<typename Label>
int Converter<Label>::get_number_of_labels() const {
  return next_token_id_;
}
