#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <set>
#include <unordered_set>

#include "node.h"
#include "string_label.h"
#include "bracket_notation_parser.h"
#include "t_join_indexing.h"
#include "unit_cost_model.h"
#include "label_dictionary.h"
#include "ted_algorithm.h"
#include "touzet_baseline_tree_index.h"
#include "distance_parser.h"
#include "indexing_parser.h"

std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>> label_distance_collection;
std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>> degree_distance_collection;
int structural_mapping(
    label_feature_set_converter::LabelSetElement& sv_r, 
    label_feature_set_converter::LabelSetElement& sv_s,
    const double distance_threshold);

void get_representive_node(std::set<int>& candidates_node,std::vector<int>& result);
std::vector<std::set<int>> included;

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
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, "/home/bowen/dataset/tree/test2_sorted.bracket");

    parser::IndexingParser ip;
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>> sets_collection;  
    ip.parse_collection_feature(sets_collection,"test2","110");

    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_indexing_algorithm;

    parser::DistanceParser dp;
    dp.parse_collection(label_distance_collection, "/home/bowen/dataset/distance/test2.label_distance");
    dp.parse_collection(label_distance_collection, "/home/bowen/dataset/distance/test2.degree_distance");

    structural_mapping(sets_collection[0].second[0],sets_collection[1].second[0], 5.0);

    // int node_counter=0;
    // for(auto node_set : distance_collection[0][0]){
    //     int ti_counter=0;
    //     for(auto p: node_set){
    //         if(p.second<=5.0)
    //             ti_counter++;
    //     }
    //     for(int i=0;i<node_counter;i++){
    //         if(distance_collection[0][0][i][node_counter].second<=5.0)
    //             ti_counter++;
    //     }
    //     std::cout<<"node: "<<node_counter<<" number of ti: "<<ti_counter<<std::endl;
    //     node_counter++;
    // }

    //ted_join_indexing_algorithm.feature_indexing_online(trees_collection,sets_collection);

  



    return 0;

}

int structural_mapping(
    label_feature_set_converter::LabelSetElement& sv_r, 
    label_feature_set_converter::LabelSetElement& sv_s,
    const double distance_threshold) {

  int tau_valid = 0;
  
  std::cout<<"sv_r weight: "<<sv_r.weight<<" sv_s: "<<sv_s.weight<<std::endl;

  //label_distance_collection[0][0]
  std::vector<std::pair<int, std::vector<int>>> node_group;
  node_group.resize(label_distance_collection[0][0].size());



 included.resize(label_distance_collection[0][0].size());
 for(size_t i=0;i<label_distance_collection[0][0].size();i++){
   for(auto pari_set:label_distance_collection[0][0][i]){
       if(pari_set.second<=distance_threshold){
           included[i].insert(pari_set.first);
           included[pari_set.first].insert(i);
       }
    }  
 }
for(size_t i=0;i<included.size();i++){
    std::cout<<"node["<<i+1<<"]: ";
    for(auto node: included[i]){
        std::cout<<node+1<<" ";
    } 
    std::cout<<std::endl;
}

    // std::set<int> checked_node;
    std::set<int> node_set;
    for(int i=0;i<label_distance_collection[0][0].size();i++){
        node_set.insert(i);
    }
    std::set<int> candidate_sets=node_set; 
    std::vector<int> result;

    while(!candidate_sets.empty()){
        while(!node_set.empty()){
            get_representive_node(node_set,result);
        }
        for(auto r: result){
            candidate_sets.erase(r);
        }
        node_set=candidate_sets;
        std::cout<<"result: ";
        for(auto r: result){
            std::cout<<r<<" ";
        }
        std::cout<<std::endl;
    }

    
   

  
    // while(!node_set.empty()){
    //     int max_weight=-1;
    //     int max_index=-1;
    //     for(auto index:node_set){
    //         if(included[index].size()>max_weight){
    //             max_weight=included[index].size();
    //             max_index=index;
    //         }
    //     }
    //     std::unordered_set<int> temp_node;
    //     temp_node.insert(max_index);
    //     for(auto index: included[max_index])
    //         temp_node.insert(index);
        
    //     for(auto index: node_set){

    //     }


    // }

  



  return tau_valid;
}

void get_representive_node(std::set<int>& candidates_node,std::vector<int>& result){

    //std::cout<<"jjjj"<<std::endl;

    size_t max_weight=0;
    size_t max_index=0;

    for(auto index:candidates_node){
        //std::cout<<index<<std::endl;
        
        if(included[index].size()>max_weight){
            max_weight=included[index].size();
            max_index=index;
        }
    }
    //std::cout<<"max index"<<max_index<<std::endl;
    result.push_back(max_index);
    candidates_node.erase(max_index);

    for(auto i : included[max_index]){
        candidates_node.erase(i);
    }


    
}