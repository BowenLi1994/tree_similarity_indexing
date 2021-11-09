#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"

using Label = label::StringLabel;
void tree_string (const node::Node<Label> &root,std::string& temp);
std::unordered_map<std::string,int> label_map;

int main(int argc, char** argv){

    
    std::string input_file_name=std::string(argv[1]);
    std::string input_file_path="/home/bowen/dataset/tree/"+input_file_name+"_sorted.bracket";


    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, input_file_path);


    int pos=0;
    for(auto tree: trees_collection){
        std::vector<std::string> labels;
        labels=tree.get_all_labels();
        for(auto l:labels){
            if(!label_map.count(l)){
                label_map.emplace(l,pos);
                pos++;
            }

        }
    }

    std::ofstream outfile("/home/bowen/dataset/tree/"+input_file_name+"_sorted.bracket");
    

    int tree_counter=0;
    for(auto tree: trees_collection){
        std::string temp;
        tree_string(tree,temp);
        outfile<<temp<<std::endl;  
    }

}

void tree_string (const node::Node<Label> &root,std::string& temp){
    
    temp+="{";
    temp+=std::to_string(label_map[root.label().to_string()]);
    for(auto& child: root.get_children()){
        tree_string(child,temp);
    }
    temp+="}";
}

