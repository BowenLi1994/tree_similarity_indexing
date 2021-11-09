#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <set>

#include "string_label.h"
#include "node/node.h"
#include "bracket_notation_parser.h"

int main(int, char** argv){

    std::string file_name=std::string(argv[1]);

    using Label = label::StringLabel;

    std::vector<node::Node<Label>> trees_collection;
  

   parser::BracketNotationParser bnp;

   std::string file_path="/home/bowen/dataset/tree/"+file_name+"_sorted.bracket";
   bnp.parse_collection(trees_collection, file_path);

   std::vector<int> tree_size;
   long tree_sum=0;
   int min_tree=INT32_MAX;
   int max_tree=INT32_MIN;

   std::set<std::string> label_set;

   for(size_t i=0;i<trees_collection.size();i++){
       //std::cout<<"tree: "<<i<<std::endl;
       tree_sum+=trees_collection[i].get_tree_size();
       tree_size.push_back(trees_collection[i].get_tree_size());
       min_tree=std::min(min_tree,trees_collection[i].get_tree_size());
       max_tree=std::max(max_tree,trees_collection[i].get_tree_size());

       std::vector<std::string> labels;
       labels=trees_collection[i].get_all_labels();
       for(auto l:labels){
           label_set.insert(l);
       }

   }

   double avg_tree_size=((double)tree_sum)/((double)trees_collection.size());

    double var = 0;
    for( auto size: tree_size )
        var += (size - avg_tree_size) * (size - avg_tree_size);
    var /= tree_size.size();
    double sd = sqrt(var);

    std::cout<<"file_name: "<<file_name<<" standard variance: "<<sd<<std::endl;
    std::cout<<"min tree size: "<<min_tree<<" max tree size: "<<max_tree<<std::endl;
    std::cout<<"avg tree size: "<< avg_tree_size<<std::endl<<" label number: "<<label_set.size()<<std::endl;




}