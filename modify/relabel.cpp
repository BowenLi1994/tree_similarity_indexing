#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"


using Label = label::StringLabel;
void tree_string (const node::Node<Label> &root,std::string& temp,std::unordered_map<std::string, int>& label_map, double threshold);
void label_collector(const node::Node<Label> &root,std::unordered_map<std::string, int>& label_map,int& label_counter);

int main(int argc, char** argv){

    std::string input_file_name=std::string(argv[1]);
    std::string input_file_path="/home/bowen/dataset/tree/"+input_file_name+"_sorted.bracket";

    double threshold=std::atof(argv[2]);

    std::vector<node::Node<Label>> trees_collection;

    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, input_file_path);

    int label_counter=0;
    std::unordered_map<std::string,int> label_map;

    for(auto tree: trees_collection){
        label_collector(tree,label_map,label_counter);
    }

    // std::cout<<"label size: "<<label_map.size()<<std::endl;


    // if(threshold<1){
    //     int range=int(label_number*threshold);

    // }
    // else{

    // }


    // std::string outfile_name=input_file_name+"_"+std::string(argv[2]);
    // std::string outfile_path="/home/bowen/dataset/tree/";
    std::ofstream outfile("/home/bowen/dataset/tree/syn4_sorted.bracket");

    for(auto tree: trees_collection){

        std::string temp;
        tree_string(tree,temp,label_map,threshold);
        outfile<<temp<<std::endl;
    }

    outfile.close();


}

void label_collector(const node::Node<Label> &root,std::unordered_map<std::string, int>& label_map,int& label_counter){
    if(!label_map.count(root.label().to_string())){
        label_map.emplace(root.label().to_string(),label_counter);
        label_counter++;
    }

    for(auto child: root.get_children())
        label_collector(child,label_map,label_counter);
}

void tree_string (const node::Node<Label> &root,std::string& temp,std::unordered_map<std::string, int>& label_map, double threshold){

    temp+="{";
    //percentage
    if(threshold<1){

       int new_label=int(label_map[root.label().to_string()]*threshold);
       temp+=std::to_string(new_label);

    }
    else{

        int new_label= (label_map[root.label().to_string()]*threshold)/label_map.size();
        temp+=std::to_string(new_label);
    }

    for(auto& child: root.get_children()){
        tree_string(child,temp,label_map,threshold);
    }
    temp+="}";
}