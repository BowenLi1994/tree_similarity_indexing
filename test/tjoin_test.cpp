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

    // std::ofstream resultFile;
    // std::string filepath="/home/bowen/tree_similarity_indexing/result/tjoin_result.txt";
    // resultFile.open(filepath,std::ios_base::app);

    // resultFile<<filename<<"+"<<threshold<<" : "<<duration.count()<<std::endl;
    
    // std::ofstream resultFile;
    // std::string filepath="/home/bowen/tree_similarity_indexing/result/tjoin_candidates.txt";
    // resultFile.open(filepath,std::ios_base::app);
    // resultFile<<filename<<"+"<<threshold<<
    // "  pre-candidates: "<<ted_join_algorithm.pre_candidates<<" candidates: "<<ted_join_algorithm.candidates_num
    // <<" result: "<<ted_join_algorithm.result<<std::endl;



    std::ofstream resultFile;
    std::string filepath="/home/bowen/tree_similarity_indexing/result/tjoin_comparation.txt";
    resultFile.open(filepath,std::ios_base::app);
    resultFile<<filename<<"+"<<threshold<<": "<<ted_join_algorithm.same_label_comparations<<std::endl;

    //std::cout<<"result: "<<join_result.size()<<std::endl;
    // for(auto i: join_result){
    //     std::cout<<"pari: "<<i.tree_id_1<<"&"<<i.tree_id_2<<std::endl;
    // }


    return 0;
}