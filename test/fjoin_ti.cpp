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
#include "distance_parser.h"



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
    int threshold=std::atoi(argv[2]);
    // std::string flag(argv[3]);
    int number_of_labels=std::atoi(argv[3]);

    std::vector<node::Node<Label>> trees_collection;
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, "/home/bowen/dataset/tree/"+filename+"_sorted.bracket");

    parser::IndexingParser ip;
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>> sets_collection;  
    ip.parse_collection_feature(sets_collection,filename,"110");


    std::vector<std::pair<int, int>> candidates;
    std::vector<join::JoinResultElement> join_result;
    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_indexing_algorithm;

    std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>> distance_collection;
    parser::DistanceParser dp;
    dp.parse_collection(distance_collection, "/home/bowen/dataset/distance/"+filename+".label_distance");

    // int tree_counter=0;
    // for(auto tree_set: distance_collection){
    //     std::cout<<"tree: "<<tree_counter<<std::endl;
    //     int label_counter=0;
    //     for(auto lable_set: tree_set){
    //         std::cout<<"label: "<<label_counter<<std::endl;
    //         int node_counter=0;
    //         for(auto node_set: lable_set){
    //             std::cout<<"node: "<<node_counter<<std::endl;
    //             for(auto pair_set: node_set)
    //                 std::cout<<"pari: "<<pair_set.first<<"--->"<<pair_set.second<<std::endl;
    //             node_counter++;
    //         }
    //         label_counter++;
    //     }


    //     tree_counter++;
    // }

    ted_join_indexing_algorithm.number_of_labels_=number_of_labels;


    auto begin = std::chrono::high_resolution_clock::now();
    ted_join_indexing_algorithm.execute_feature_join_ti(trees_collection,sets_collection,distance_collection ,candidates,join_result,(double)threshold);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);


    //std::cout<<"same label: "<<ted_join_indexing_algorithm.same_label_compa<<std::endl;

    //std::cout<<"result: "<<join_result.size()<<std::endl;
    std::ofstream resultFile;
    std::string filepath="/home/bowen/tree_similarity_indexing/result/fjoin_compararions.txt";
    resultFile.open(filepath,std::ios_base::app);
    resultFile<<filename<<"+"<<threshold<<" : "<<ted_join_indexing_algorithm.same_label_compa<<std::endl;

    std::ofstream resultFile2;
    std::string filepath2="/home/bowen/tree_similarity_indexing/result/fjoin_time.txt";
    resultFile2.open(filepath2,std::ios_base::app);
    resultFile2<<filename<<"+"<<threshold<<" : "<<duration.count()<<std::endl;


}