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
#include "indexing_parser.h"
#include "t_join_indexing.h"



int main(int, char** argv) {

    // Type aliases.
    using Label = label::StringLabel;
    using CostModel = cost_model::UnitCostModelLD<Label>;
    using LabelDictionary = label::LabelDictionary<Label>;

    // Initialise label dictionary - separate dictionary for each test tree becuse it is easier to keep track of label ids.
    LabelDictionary ld;
  
    // Initialise cost model.
    CostModel ucm(ld);

    std::string filename(argv[1]);
    std::string flag(argv[2]);

    std::vector<node::Node<Label>> trees_collection;
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, "/home/bowen/dataset/tree/"+filename+"_sorted.bracket");

    parser::IndexingParser ip;
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>> sets_collection;  
    ip.parse_collection_feature(sets_collection,filename,flag);

    std::vector<std::pair<int, int>> candidates;
    std::vector<join::JoinResultElement> join_result;
    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_indexing_algorithm;

    ted_join_indexing_algorithm.execute_feature_join(trees_collection,sets_collection,candidates,join_result,2.0);
    
    




    // for(auto set: sets_collection){
    //     std::cout<<"tree size: "<<set.first<<std::endl;

    //     for(auto lable_elment: set.second){

    //         std::cout<<" label id: "<<lable_elment.id<<" weight: "<<lable_elment.weight<<" weight so far: "<<lable_elment.weight_so_far<<std::endl;

    //         for(auto node_element: lable_elment.struct_vect){
    //             std::cout<<" node post id: "<<node_element.postorder_id<<" node left:  "<<node_element.number_nodes_left<<" node right: "<<node_element.number_nodes_right<<
    //             " node up: "<<node_element.number_nodes_ancestor<<" node down: "<<node_element.number_nodes_descendant<<std::endl;
            
    //             int map_counter=0;
    //             for(auto map:node_element.label_histogram){
               
    //                 std::cout<<" label histogram ["<<map_counter<<"]:"<<std::endl;
    //                 for(auto kv:map){
    //                     std::cout<<kv.first<<"-->"<<kv.second<<" ";
    //                 }
    //                 std::cout<<std::endl;
    //                 map_counter++;
    //             }
            
    //             map_counter=0;
    //             for(auto map:node_element.degree_histogram){
               
    //                 std::cout<<" degree histogram ["<<map_counter<<"]:"<<std::endl;
    //                 for(auto kv:map){
    //                     std::cout<<kv.first<<"-->"<<kv.second<<" ";
    //                 }
    //                 std::cout<<std::endl;
    //                 map_counter++;
    //             }

    //             map_counter=0;
    //             for(auto map:node_element.leaf_histogram){
               
    //                 std::cout<<" leaf histogram ["<<map_counter<<"]:"<<std::endl;
    //                 for(auto kv:map){
    //                     std::cout<<kv.first<<"-->"<<kv.second<<" ";
    //                 }
    //                 std::cout<<std::endl;
    //                 map_counter++;
    //             }

    //         }
    //     }

    // }

}