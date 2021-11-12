#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "t_join_indexing.h"
#include "indexing_parser.h"
#include "histogram_distance.h"

int main(int, char** argv) {

    
    std::string filename(argv[1]);
    std::string flag(argv[2]);

    parser::IndexingParser ip;
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>> sets_collection;  
    ip.parse_collection_feature(sets_collection,filename,flag);

    histogram::Histogram_distance hd;
    hd.histogtam_distance_indexing(sets_collection,filename);



    return 0;
}
