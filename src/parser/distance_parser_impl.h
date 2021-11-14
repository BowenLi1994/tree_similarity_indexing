using namespace parser;

void DistanceParser::parse_single(const std::string& index_string, std::vector<std::vector<std::vector<std::pair<int,double>>>>& distance_vector){

    //std::cout<<index_string<<std::endl;

    std::vector<std::string> label_sets;
    tokenize(index_string,']',label_sets);
    label_sets.pop_back();

    for(auto node_sets: label_sets){

        //std::cout<<"node set: "<<node_sets<<std::endl;
        
        //label-->node pair distance
        std::vector<std::vector<std::pair<int,double>>>node_indexing_distance;

        if(node_sets=="["){
            distance_vector.push_back(node_indexing_distance);
        }
        else{
            node_sets.erase(node_sets.begin());

            std::vector<std::string> node_pairs;
            tokenize(node_sets,')',node_pairs);
            node_pairs.pop_back();

            std::vector<std::string> back_node_info;
            tokenize(node_pairs.back(),',',back_node_info);

            node_indexing_distance.resize(std::stoi(back_node_info[1])+1);

            for(auto pair: node_pairs){
                pair.erase(pair.begin());
                //std::cout<<pair<<std::endl;
                std::vector<std::string> node_info;
                tokenize(pair,',',node_info);
                node_indexing_distance[std::stoi(node_info[0])].emplace_back(std::stoi(node_info[1]),std::stod(node_info[2]));
            }
            distance_vector.push_back(node_indexing_distance);
        }

            

    }
        //
        //

}

void DistanceParser::parse_collection(
    std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>>& distance_collection,
    const std::string& file_path){

        std::ifstream index_file(file_path);
        if (!index_file) {
        throw std::runtime_error("ERROR: Problem with opening the file '" + file_path + "' in Indexing::parse_collection_efficient.");
        }
     // Read the trees line by line, parse, and move into the container.
        std::string indexing_string;
  //int set_count=0;
        while (std::getline(index_file, indexing_string)) {
    //std::cout<<"set: "<<set_count<<std::endl;
            std::vector<std::vector<std::vector<std::pair<int,double>>>> distance_vector;
            parse_single(indexing_string,distance_vector);
            distance_collection.push_back(distance_vector);// -> This invokes a move constructor (due to push_back(<rvalue>)).
    //set_count++;
        }
        index_file.close();
    }





void  DistanceParser::tokenize(const std::string &s, const char delim,std::vector<std::string> &out){

    std::string::size_type beg = 0;
    for (auto end = 0; (end = s.find(delim, end)) != std::string::npos; ++end)
    {
        out.push_back(s.substr(beg, end - beg));
        beg = end + 1;
    }
 
    out.push_back(s.substr(beg));

}