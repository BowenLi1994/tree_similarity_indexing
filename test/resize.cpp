#include <iostream>
#include <string>
#include <vector>

#include "string_label.h"
#include "node/node.h"
#include "bracket_notation_parser.h"


using Label = label::StringLabel;
void tree_string (const node::Node<Label> &root,std::string& temp);

int main(int argc, char** argv){

    
    std::string input_file_name=std::string(argv[1]);
    std::string input_file_path="/home/bowen/dataset/tree/"+input_file_name+"_sorted.bracket";
    int tree_number=std::stoi(argv[2]);
    int tree_size=std::stoi(argv[3]);

    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, input_file_path);


    std::ofstream outfile("/home/bowen/dataset/tree/syn1_sorted.bracket");
    

    int tree_counter=0;
    for(auto tree: trees_collection){

        if(tree.get_tree_size()>tree_size){
            std::string temp;
            tree_string(tree,temp);
            outfile<<temp<<std::endl;
            tree_counter++;
        }

        if(tree_counter==tree_number)
            break;


    }

    

    outfile.close();

    
    return 0;
}


void tree_string (const node::Node<Label> &root,std::string& temp){
    
    temp+="{";
    temp+=root.label().to_string();
    for(auto& child: root.get_children()){
        tree_string(child,temp);
    }
    temp+="}";
}