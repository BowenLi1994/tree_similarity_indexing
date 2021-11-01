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





int main(int, char** argv){

    // Type aliases.
    using Label = label::StringLabel;
    using CostModel = cost_model::UnitCostModelLD<Label>;
    using LabelDictionary = label::LabelDictionary<Label>;



    std::string filename=std::string(argv[1]);
    std::string input_file_path="/home/bowen/dataset/tree/"+filename+"_sorted.bracket";
    //std::string input_file_path="../dataset/unit.tree";

    LabelDictionary ld;
  
    // Initialise cost model.
    CostModel ucm(ld);

    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, input_file_path);


    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_indexing_algorithm;

    auto begin = std::chrono::high_resolution_clock::now();
    
    ted_join_indexing_algorithm.feature_indexing(filename,trees_collection);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);

    std::ofstream resultFile;
    std::string filepath="feature_indexing_time.txt";
    resultFile.open(filepath,std::ios_base::app);

    resultFile<<filename<<": "<<duration.count()<<std::endl;

   





    return 0;
}