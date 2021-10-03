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

    std::string input_file_path="../dataset/test1.tree";

    // int threshold=std::stoi(std::string(argv[1]));
    // Initialise label dictionary - separate dictionary for each test tree becuse it is easier to keep track of label ids.
    LabelDictionary ld;
  
    // Initialise cost model.
    CostModel ucm(ld);

    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, input_file_path);

    // std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;
    // std::vector<std::pair<int, int>> candidates;
    // std::vector<join::JoinResultElement> join_result;

    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_indexing_algorithm;
    
    ted_join_indexing_algorithm.indexing("bolzano",trees_collection);
    // ted_join_algorithm.execute_join(trees_collection,sets_collection, candidates, join_result, (double)threshold);

    // std::cout<<"Threshold: "<<threshold<<" result: "<<join_result.size()<<std::endl;





    return 0;
}