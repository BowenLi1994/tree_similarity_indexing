#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "label_set_element.h"
#include "label_feature_set_element.h"

namespace parser {

class DistanceParser {
// Types and type aliases

// Member functions
public:
 
//void parse_single(const std::string& index_string, std::vector<std::vector<std::pair<int,int>>& distance_vecotor);

    void parse_single(const std::string& index_string, std::vector<std::vector<std::vector<std::pair<int,double>>>>&);
    void tokenize(const std::string &s, const char delim,
            std::vector<std::string> &out);
  
void parse_collection(
    std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>>& distance_collection,
    const std::string& file_path);

};

// Implementation details
#include "distance_parser_impl.h"

}
