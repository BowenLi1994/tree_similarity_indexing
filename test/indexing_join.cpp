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


    std::string index_string="<44>[308,1,1{4,0,3,20,0}{6,1,21,13,4}][585,1,2{1,3,0,0,0}][590,1,3{3,1,2,0,0}][592,1,4{2,2,1,0,0}]";
   

    std::vector<std::pair<int,std::vector<label_set_converter::LabelSetElement>>> sets_collection;
    std::string file_path="/home/bowen/dataset/indexing/sentiment.indexing";
    parser::IndexingParser ip;

    ip.parse_collection(sets_collection,file_path);

    

    // std::cout<<"set---> tree_size: "<<set.first<<std::endl;
    // for(auto element : set.second){
    //   std::cout<<"id: "<<element.id<<" weight: "<<element.weight<<" weight so far: "<<element.weight_so_far<<std::endl;
    //   for(auto sv: element.struct_vect)
    //     std::cout<<"\t postid: "<<sv.postorder_id<<" up: "<<sv.number_nodes_ancestor<<" down: "<<sv.number_nodes_descendant
    //     <<" left: "<<sv.number_nodes_left<<" right: "<<sv.number_nodes_right<<std::endl;
    // }



}