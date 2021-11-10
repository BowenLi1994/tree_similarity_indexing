#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>

#include "node.h"
#include "string_label.h"
#include "bracket_notation_parser.h"
#include "unit_cost_model.h"
#include "label_dictionary.h"
#include "t_join_ti.h"
#include "ted_algorithm.h"
#include "touzet_baseline_tree_index.h"

#include "t_join_indexing.h"
int main(int, char** argv) {

    // Type aliases.
    using Label = label::StringLabel;
    using CostModel = cost_model::UnitCostModelLD<Label>;
    using LabelDictionary = label::LabelDictionary<Label>;

    //std::string tree_string="{1{2{4}{5{7{8}}}{6{9}}}{3}}";


    // Initialise label dictionary - separate dictionary for each test tree becuse it is easier to keep track of label ids.
    LabelDictionary ld;
  
    // Initialise cost model.
    CostModel ucm(ld);
  
    std::vector<node::Node<Label>> trees_collection;



   
    // Parse the dataset.
    parser::BracketNotationParser bnp;
    // node::Node<Label> root= bnp.parse_single(tree_string);
    bnp.parse_collection(trees_collection,"/home/bowen/dataset/tree/test_sorted.bracket");
    
    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;

    for(auto tree: trees_collection){
        std::cout<<"*********"<<std::endl;
        std::vector<node::Node<Label> *> nodes;
        int order=0;
        ted_join_algorithm.postordering(tree,order,nodes);

        for(auto node: nodes){
            std::cout<<"node: "<<node->label().to_string()<<" leaf: "<<node->leaf_dist<<std::endl;
        }
    }

    



   
//     bnp.parse_collection(trees_collection, "/home/bowen/dataset/tree/bolzano_sorted.bracket");

//     // std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;
//     // std::vector<std::pair<int, int>> candidates;
//     // std::vector<join::JoinResultElement> join_result;
//     // join::TJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;

// //     typename VerificationAlgorithm::AlgsCostModel cm(ld_);
// //   VerificationAlgorithm ted_algorithm(cm);
// //   typename VerificationAlgorithm::AlgsTreeIndex ti_1;
// //   typename VerificationAlgorithm::AlgsTreeIndex ti_2;
    
//     //ted::ZhangShashaTreeIndex<ucm,node::TreeIndexZhangShasha> ti;
//     //node::index_tree<ted::ZhangShashaTreeIndex,Label,ucm>()




}
