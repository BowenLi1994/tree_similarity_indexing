
#pragma once

#include <vector>
#include <functional>
#include "join_result_element.h"
#include "node.h"
#include "lgm_tree_index.h"
#include "candidate_index.h"
#include "label_set_converter.h"
#include "label_set_element.h"

namespace join {

template <typename Label, typename VerificationAlgorithm>
class TJoinTI {
public:
  TJoinTI();
  /// Executes TJoin algorithm.
  /**
   * TODO: Upper Bound algorithm could be also templated and not fixed to LGM.
   *
   * Given a collection of trees, the candidates are retrieved by an 
   * efficient and effective candidate index. Next, all candidates are 
   * verified with the label guided mapping upper bound. Last, the 
   * remaining candidates are evaluated by Touzets algorithm.
   *
   * \param trees_collection A vector holding an input collection of trees.
   * \param sets_collection A vector holding the label sets of trees_collection.
   * \param candidates A vector of candidate tree pairs.
   * \param join_result A vector of result tree pairs and their TED value.
   * \param distance_threshold The maximum number of edit operations that
   *                           differs two trees in the join's result set.
   * \return A vector with the join result.
   */
  void execute_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold);
  /// Converts a given collection of trees into a collection of label sets.
  /**
   * \param trees_collection A vector holding an input collection of trees.
   * \return A vector containing the according label sets of the input trees
   *         in trees_collection.
   */
  void convert_trees_to_sets(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection);
  /// Retrieves candidate pairs from candidate index.
  /**
   * \param trees_collection A vector holding an input collection of trees.
   * \param candidates A vector of candidate tree pairs.
   * \param distance_threshold The maximum number of edit operations that
   *                           differs two trees in the join's result set.
   * \return A vector containing pairs of trees ids (candidates) that are 
   *         considered candidates.
   */
  void retrieve_candidates(
      std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
      std::vector<std::pair<int, int>>& candidates,
      const double distance_threshold);
  /// Verifies candidates using Label Guided Mapping upper bound (LGM).
  /**
   * \param trees_collection A vector holding an input collection of trees.
   * \param candidates A vector of candidate tree pairs.
   * \param join_result A vector of result tree pairs and their TED value.
   * \param distance_threshold The maximum number of edit operations that
   *                           differs two trees in the join's result set.
   */
  void upperbound(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result, 
    const double distance_threshold);
  /// Verifies each candidate pair with the tree edit distance.
  /**
   * \param trees_collection A vector holding an input collection of trees.
   * \param candidates A vector of candidate tree pairs.
   * \param join_result A vector of result tree pairs and their TED value.
   * \param distance_threshold The maximum number of edit operations that
   *                           differs two trees in the join's result set.
   * \return A vector containing the join result.
   */
  void verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result, 
    const double distance_threshold);
  /// Returns the number of precandidates.
  /**
   * \return pre_candidates_
   */
  long long int get_number_of_pre_candidates() const;
  /// Returns the cumulative number of subproblems of TED executions.
  /**
   * \return sum_subproblem_counter_
   */
  long long int get_subproblem_count() const;
  /// Returns the number of inverted list lookups.
  /**
   * \return il_lookups_
   */
  long long int get_number_of_il_lookups() const;

  int pre_candidates,candidates_num,result;
  int same_label_comparations;
  int tau_valid_mapping;
  int conflicts;

private:
  /// LabelDictionary for using with LGMTreeIndex and VerificationAlgorithm.
  label::LabelDictionary<Label> ld_;
  /// Number of different labels in the trees.
  long long int number_of_labels_;
  /// Number of precandidates.
  long long int pre_candidates_;
  /// Number of subproblrems encoutered in the verification step.
  long long int sum_subproblem_counter_;
  /// Number of precandidates.
  long long int il_lookups_;
};

// Implementation details.
#include "t_join_ti_impl.h"

}
