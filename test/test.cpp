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
#include "histogram_distance.h"
#include "distance_parser.h"


int main(int, char** argv) {

    std::string file_path="/home/bowen/dataset/distance/bolzano.degree_distance";

    std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>> distance_collection;

    parser::DistanceParser dp;
    dp.parse_collection(distance_collection,file_path);

}
