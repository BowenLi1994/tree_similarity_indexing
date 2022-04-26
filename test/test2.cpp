#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>

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
    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, "../dataset/trees2.txt");

    join::TJoin_Indexing<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_indexing_algorithm;
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;

    ted_join_indexing_algorithm.convert_trees_to_sets(trees_collection,sets_collection);

    for(int i=0;i<trees_collection[0].get_tree_size();i++){
        for(int j=0;j<trees_collection[0].get_tree_size();j++){
            int distance=0;
            distance=std::abs(sets_collection[0].second[0].struct_vect[i].number_nodes_ancestor-sets_collection[0].second[0].struct_vect[j].number_nodes_ancestor)+
                     std::abs(sets_collection[0].second[0].struct_vect[i].number_nodes_descendant-sets_collection[0].second[0].struct_vect[j].number_nodes_descendant)+
                     std::abs(sets_collection[0].second[0].struct_vect[i].number_nodes_left-sets_collection[0].second[0].struct_vect[j].number_nodes_left)+
                     std::abs(sets_collection[0].second[0].struct_vect[i].number_nodes_right-sets_collection[0].second[0].struct_vect[j].number_nodes_right);
            std::cout<<"node["<<i<<"]-->node["<<j<<"]: "<<distance<<std::endl;
            
        }
    }

}