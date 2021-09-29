#pragma once

#include "node.h"
#include "string_label.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

namespace parser {

class BracketNotationParser {
// Types and type aliases
public:
  using Label = label::StringLabel;

// Member functions
public:
  /// Takes the string of a tree in bracket notation, parses it to the Node
  /// structure with StringLabels, and returns the root.
  ///
  /// \param tree_string The string holding the tree in bracket notation.
  ///
  /// \return Root of the parsed tree.
  node::Node<Label> parse_single(const std::string& tree_string);

  /// Takes a file with one tree (in bracket notation) per line and parses it
  /// to a vector of Node objects with StringLabels.
  ///
  /// NOTE: It executes parse_single for every line in the data file.
  /// NOTE: The notation of trees is assumed correct.
  ///
  /// \param trees_collection Container to store all trees.
  /// \param file_path The path to the file with set of trees.
  void parse_collection(std::vector<node::Node<Label>>& trees_collection,
      const std::string& file_path);

  /// Generates the tokens for the input string.
  ///
  /// \param tree_string The string holding the tree in bracket notation.
  /// \return Vector with all tokens.
  std::vector<std::string> get_tokens(const std::string& tree_string);

  /// Validates the bracket notation input.
  ///
  /// NOTE: This function could be merged with parse_string but this may
  ///       decrease readability.
  ///
  /// \param tree_string Tree in bracket notation.
  /// \return True if the input is correct and false otherwise.
  bool validate_input(const std::string& tree_string) const;
// Member variables
private:
  /// A stack to store nodes on a path to the root from the current node in the
  /// parsing process. Needed for maintaining correct parent-child relationships
  /// while parsing.
  std::vector<std::reference_wrapper<node::Node<Label>>> node_stack;

  /// Structure brackets for representing nodes relationships. Could be
  /// modified to other types of paretheses if necessary.
  const std::string kLeftBracket = "{";
  const std::string kRightBracket = "}";

  /// Structure elements of the bracket notation.
  const std::string kStructureElements = "{}";

  /// Escape character.
  const char kEscapeChar = '\\';

  /// A regex string to match left and right brackets.
  const std::string kMatchLeftBracket = "\\" + kLeftBracket;
  const std::string kMatchRightBracket = "\\" + kRightBracket;
};

// Implementation details
#include "bracket_notation_parser_impl.h"

}
