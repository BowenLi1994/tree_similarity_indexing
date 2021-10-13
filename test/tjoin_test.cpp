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


int main(int, char** argv) {

    // Type aliases.
    using Label = label::StringLabel;
    using CostModel = cost_model::UnitCostModelLD<Label>;
    using LabelDictionary = label::LabelDictionary<Label>;

    // Initialise label dictionary - separate dictionary for each test tree becuse it is easier to keep track of label ids.
    LabelDictionary ld;
  
    // Initialise cost model.
    CostModel ucm(ld);
  
    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, "../dataset/test1.tree");

    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;
    std::vector<std::pair<int, int>> candidates;
    std::vector<join::JoinResultElement> join_result;
    join::TJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;

    ted_join_algorithm.execute_join(trees_collection,sets_collection, candidates, join_result, 2.0);



    // int set_counter=0;
    // for(auto set: sets_collection){

    //     std::cout<<"set---> tree_size: "<<set.first<<std::endl;
    //     for(auto element : set.second){
    //         std::cout<<"id: "<<element.id<<" weight: "<<element.weight<<" weight so far: "<<element.weight_so_far<<std::endl;
    //         for(auto sv: element.struct_vect)
    //             std::cout<<"\t postid: "<<sv.postorder_id<<" up: "<<sv.number_nodes_ancestor<<" down: "<<sv.number_nodes_descendant
    //             <<" left: "<<sv.number_nodes_left<<" right: "<<sv.number_nodes_right<<std::endl;
    //     }

    // }






    return 0;
}