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

    std::string filename(argv[1]);

    // Initialise label dictionary - separate dictionary for each test tree becuse it is easier to keep track of label ids.
    LabelDictionary ld;
  
    // Initialise cost model.
    CostModel ucm(ld);
  
    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, "/home/bowen/dataset/tree/"+filename+"_sorted.bracket");

    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;
    std::vector<std::pair<int, int>> candidates;
    std::vector<join::JoinResultElement> join_result;
    join::TJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;

    int threshold=std::atoi(argv[2]);

    
    auto begin = std::chrono::high_resolution_clock::now();
    ted_join_algorithm.execute_join(trees_collection,sets_collection, candidates, join_result, threshold);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

    //std::cout<<"threshold: "<<threshold<<" number of result: "<<join_result.size()<<std::endl;
    //std::cout<<"threshold: "<<threshold<<" tau valid mapping: "<<ted_join_algorithm.tau_valid_mapping<<std::endl;
    std::cout<<"threshold: "<<threshold<<" number of conflicts: "<<ted_join_algorithm.conflicts<<std::endl;
    // for(auto r:join_result)
    //     std::cout<<" tree1: "<<r.tree_id_1<<" tree2: "<<r.tree_id_2<<" ted: "<<r.ted_value<<std::endl;

    // for(auto s: sets_collection){
    //     std::cout<<"tree size: "<<s.first<<std::endl;

    //     for(auto lset : s.second){
    //         std::cout<<"label id: "<<lset.id<<" weight: "<<lset.weight<<" weight so far: "<<lset.weight_so_far<<std::endl;
    //         for(auto sv: lset.struct_vect){
    //             sv.
    //         }
    //     }
    // }



    return 0;
}