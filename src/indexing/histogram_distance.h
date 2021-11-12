#include <iostream>
#include <fstream>

#include "label_feature_set_element.h"
#include "t_join_indexing.h"

namespace histogram {

class Histogram_distance{

public:
    void histogtam_distance_indexing(
        std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
        std::string file_name);

};




#include "histogram_distance_impl.h"

}