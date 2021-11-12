
#pragma once

#include <vector>
#include <functional>
#include <fstream>

#include "join_result_element.h"
#include "node.h"
#include "lgm_tree_index.h"
#include "candidate_index.h"
#include "feature_candidate_index.h"
#include "label_set_converter.h"
#include "label_set_element.h"
#include "label_feature_set_element.h"
#include "label_feature_set_converter.h"

namespace join {

template <typename Label, typename VerificationAlgorithm>
class TJoin_Indexing {
public:
  TJoin_Indexing();

  void indexing(std::string filename,std::vector<node::Node<Label>>& trees_collection);

  void feature_indexing(std::string filename, std::vector<node::Node<Label>>& trees_collection);

  void feature_indexing_online(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection
  );

  void postordering(node::Node<Label>& tree, int& order,std::vector<node::Node<Label>*>& tree_postorder_collection);

  void preordering(node::Node<Label>& tree, int& order);


  void execute_feature_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
      std::vector<std::pair<int, int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold);



  


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

  
  void convert_trees_to_feature_sets(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection);



      
  // Retrieves candidate pairs from candidate index.
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
  
  void retrive_feature_candidates(
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& candidates,
    const double distance_threshold);


    // void execute_feature_join(
    //     std::vector<node::Node<Label>>& trees_collection,
    //     std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
    //     std::vector<std::pair<int, int>>& candidates,
    //     std::vector<join::JoinResultElement>& join_result,
    //     const double distance_threshold);

      
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
// //   /// Returns the number of precandidates.
// //   /**
// //    * \return pre_candidates_
// //    */
// //   long long int get_number_of_pre_candidates() const;
// //   /// Returns the cumulative number of subproblems of TED executions.
// //   /**
// //    * \return sum_subproblem_counter_
// //    */
// //   long long int get_subproblem_count() const;
// //   /// Returns the number of inverted list lookups.
// //   /**
// //    * \return il_lookups_
// //    */
// //   long long int get_number_of_il_lookups() const;
long long int number_of_labels_;

int candidates_num,pre_candidates,result;
int same_label_compa;

private:
  /// LabelDictionary for using with LGMTreeIndex and VerificationAlgorithm.
  label::LabelDictionary<Label> ld_;
  /// Number of different labels in the trees.
  
  /// Number of precandidates.
  long long int pre_candidates_;
  /// Number of subproblrems encoutered in the verification step.
  long long int sum_subproblem_counter_;
  /// Number of precandidates.
  long long int il_lookups_;
};

// Implementation details.
#include "t_join_indexing_impl.h"

}
