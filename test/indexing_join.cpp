#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>

#include "node.h"
#include "string_label.h"
#include "bracket_notation_parser.h"
#include "indexing_parser.h"
#include "unit_cost_model.h"
#include "label_dictionary.h"
#include "t_join_ti.h"
#include "ted_algorithm.h"
#include "touzet_baseline_tree_index.h"

#include "t_join_indexing.h"

int main(int, char** argv){

    std::string filename(argv[1]);


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
    bnp.parse_collection(trees_collection, "/home/bowen/dataset/tree/"+filename+"_sorted.bracket");

    std::vector<std::pair<int,std::vector<label_set_converter::LabelSetElement>>> sets_collection;

    //Parse the feature indexing

    parser::IndexingParser ip;
    ip.parse_collection(sets_collection,"/home/bowen/dataset/indexing/"+filename+".indexing");

    //std::cout<<"sets: "<< sets_collection.size()<<std::endl;

    std::vector<std::pair<int, int>> candidates;
    std::vector<join::JoinResultElement> join_result;
    join::TJoin_Indexing<Label,ted::TouzetBaselineTreeIndex<CostModel>> ted_indexing_join_algorithm;


    ted_indexing_join_algorithm.number_of_labels_=std::atoi(argv[2]);
    
    int threshold = std::atoi(argv[3]);

    ted_indexing_join_algorithm.execute_join(trees_collection,sets_collection,candidates,join_result,(double)threshold);

    std::cout<<"join result: "<<join_result.size()<<std::endl;
    



    

    // std::cout<<"set---> tree_size: "<<set.first<<std::endl;
    // for(auto element : set.second){
    //   std::cout<<"id: "<<element.id<<" weight: "<<element.weight<<" weight so far: "<<element.weight_so_far<<std::endl;
    //   for(auto sv: element.struct_vect)
    //     std::cout<<"\t postid: "<<sv.postorder_id<<" up: "<<sv.number_nodes_ancestor<<" down: "<<sv.number_nodes_descendant
    //     <<" left: "<<sv.number_nodes_left<<" right: "<<sv.number_nodes_right<<std::endl;
    // }



}