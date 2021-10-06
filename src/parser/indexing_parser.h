#pragma once


#include <iostream>
#include <fstream>
#include <string>
#include <regex>

#include "label_set_element.h"

namespace parser {

class IndexingParser {
// Types and type aliases


// Member functions
public:
 
void parse_single(const std::string& index_string, std::pair<int,std::vector<label_set_converter::LabelSetElement>>& set);

  
void parse_collection(
    std::vector<std::pair<int,std::vector<label_set_converter::LabelSetElement>>> sets_collection,
    const std::string& file_path);

//   /// Generates the tokens for the input string.
//   ///
//   /// \param tree_string The string holding the tree in bracket notation.
//   /// \return Vector with all tokens.
//   std::vector<std::string> get_tokens(const std::string& tree_string);

//   /// Validates the bracket notation input.
//   ///
//   /// NOTE: This function could be merged with parse_string but this may
//   ///       decrease readability.
//   ///
//   /// \param tree_string Tree in bracket notation.
//   /// \return True if the input is correct and false otherwise.
//   bool validate_input(const std::string& tree_string) const;
// Member variables
private:

};

// Implementation details
#include "indexing_parser_impl.h"

}
