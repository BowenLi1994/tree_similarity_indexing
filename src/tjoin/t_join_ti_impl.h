#pragma once
using namespace join;


template <typename Label, typename VerificationAlgorithm>
TJoinTI<Label, VerificationAlgorithm>::TJoinTI() {
  ld_ = label::LabelDictionary<Label>();
  pre_candidates_ = 0;
  sum_subproblem_counter_ = 0;
  number_of_labels_ = 0;
  il_lookups_ = 0;
  conflicts=0;
}

template <typename Label, typename VerificationAlgorithm>
void TJoinTI<Label, VerificationAlgorithm>::execute_join(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

  // Convert trees to sets and get the result.
  convert_trees_to_sets(trees_collection, sets_collection);

  // Retrieves candidates from the candidate index.
  retrieve_candidates(sets_collection, candidates, distance_threshold);

  candidates_num=candidates.size();
  

  // Use the label guided mapping upper bound to send candidates immediately .
  upperbound(trees_collection, candidates, join_result, distance_threshold);

  // Verify all computed join candidates and return the join result.
  verify_candidates(trees_collection, candidates, join_result, distance_threshold);

  result=join_result.size();
}

template <typename Label, typename VerificationAlgorithm>
void TJoinTI<Label, VerificationAlgorithm>::convert_trees_to_sets(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection) {

  // Convert trees to sets and get the result.
  label_set_converter::Converter<Label> lsc;
  lsc.assignFrequencyIdentifiers(trees_collection, sets_collection);
  number_of_labels_ = lsc.get_number_of_labels();
  //std::cout<<"number of labels: "<<number_of_labels_<<std::endl; 
}

template <typename Label, typename VerificationAlgorithm>
void TJoinTI<Label, VerificationAlgorithm>::retrieve_candidates(
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& candidates,
    const double distance_threshold) {

  // Initialize candidate index.
  candidate_index::CandidateIndex c_index;

  // Retrieve candidates from the candidate index.
  c_index.lookup(sets_collection, candidates, number_of_labels_, distance_threshold);
  // Copy the number of pre-candidates.
  pre_candidates_ = c_index.get_number_of_pre_candidates();
  // Copy the number of inverted list lookups.
  il_lookups_ = c_index.get_number_of_il_lookups();
  same_label_comparations=c_index.same_label_comparation;
  tau_valid_mapping=c_index.tau_valid_mapping;
  conflicts=c_index.conflicts;
}

template <typename Label, typename VerificationAlgorithm>
void TJoinTI<Label, VerificationAlgorithm>::upperbound(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {
  
  typename VerificationAlgorithm::AlgsCostModel cm(ld_);
  ted_ub::LGMTreeIndex<typename VerificationAlgorithm::AlgsCostModel> lgm_algorithm(cm);
  // TODO: Index trees only once for LGM And Verification using a TreeIndex
  //       that is a superset of TreeIndexLGM and VerificationAlgorithm::AlgsTreeIndex.
  node::TreeIndexLGM ti_1;
  node::TreeIndexLGM ti_2;
  
  std::vector<std::pair<int, int>>::iterator it = candidates.begin();
  while(it != candidates.end()) {
    node::index_tree(ti_1, trees_collection[it->first], ld_, cm);
    node::index_tree(ti_2, trees_collection[it->second], ld_, cm);
    double ub_value = lgm_algorithm.ted_k(ti_1, ti_2, distance_threshold);
    if(ub_value <= distance_threshold) {
      join_result.emplace_back(it->first, it->second, ub_value);
      *it = candidates.back();
      candidates.pop_back();
    }
    else {
      ++it;
    }
  }
}

template <typename Label, typename VerificationAlgorithm>
void TJoinTI<Label, VerificationAlgorithm>::verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

  typename VerificationAlgorithm::AlgsCostModel cm(ld_);
  VerificationAlgorithm ted_algorithm(cm);
  typename VerificationAlgorithm::AlgsTreeIndex ti_1;
  typename VerificationAlgorithm::AlgsTreeIndex ti_2;

  // Verify each pair in the candidate set
  for(const auto& pair: candidates) {
    node::index_tree(ti_1, trees_collection[pair.first], ld_, cm);
    node::index_tree(ti_2, trees_collection[pair.second], ld_, cm);
    double ted_value = ted_algorithm.ted_k(ti_1, ti_2, distance_threshold);

    if(ted_value <= distance_threshold)
      join_result.emplace_back(pair.first, pair.second, ted_value);
    
    // Sum up all number of subproblems
    sum_subproblem_counter_ += ted_algorithm.get_subproblem_count();
  }
}

template <typename Label, typename VerificationAlgorithm>
long long int
    TJoinTI<Label, VerificationAlgorithm>::get_number_of_pre_candidates() const {
  return pre_candidates_;
}

template <typename Label, typename VerificationAlgorithm>
long long int TJoinTI<Label, VerificationAlgorithm>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}

template <typename Label, typename VerificationAlgorithm>
long long int
    TJoinTI<Label, VerificationAlgorithm>::get_number_of_il_lookups() const {
  return il_lookups_;
}
