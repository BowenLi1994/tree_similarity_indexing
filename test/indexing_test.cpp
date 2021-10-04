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

    LabelDictionary ld;
  
    // Initialise cost model.
    CostModel ucm(ld);

    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, input_file_path);


    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_indexing_algorithm;
    
    ted_join_indexing_algorithm.indexing(filename,trees_collection);
   





    return 0;
}