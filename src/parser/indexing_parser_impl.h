# pragma once
using namespace parser;

void IndexingParser::parse_single(
  const std::string& index_string, 
  std::pair<int,std::vector<label_set_converter::LabelSetElement>>& set){

    auto string_begin=index_string.begin();
    auto string_end=index_string.end();

    if(*string_begin=='<'){
      string_begin++;
      std::string tree_size;
      while(*string_begin!='>'){
        tree_size+=*string_begin;
        string_begin++;
      }

      set.first=std::stoi(tree_size);
    }
    while (string_begin!=string_end){

     

        if(*string_begin=='['){

          string_begin++;

          std::string s[3];
          int counter=0;
          label_set_converter::LabelSetElement element;

          while(*string_begin!=']'){

            //std::cout<<*string_begin<<std::endl;

            if(*string_begin=='{'){
              //std::cout<<"**********************"<<std::endl;
              string_begin++;

              std::vector<std::string> s={""};
              while(*string_begin!='}'){
                if(*string_begin==',')
                  s.push_back("");
                else
                  s.back().push_back(*string_begin);

                string_begin++;
              }

              label_set_converter::StructuralVector sv;
              sv.postorder_id=std::stoi(s[0]);
              sv.number_nodes_ancestor=std::stoi(s[1]);
              sv.number_nodes_descendant=std::stoi(s[2]);
              sv.number_nodes_left=std::stoi(s[3]);
              sv.number_nodes_right=std::stoi(s[4]);

              element.struct_vect.push_back(sv);

            }
            else{

            if(*string_begin==',')
              counter++;
            else
              s[counter]+=*string_begin;

            }



            


            string_begin++;
          }

          //std::cout<<"id : "<<s[0]<< " weight: "<<s[1]<<" weight so far: "<<s[2]<<std::endl;

            element.id=std::stoi(s[0]);
            element.weight=std::stoi(s[1]);
            element.weight_so_far=std::stoi(s[2]);
            set.second.push_back(element);


        }
        



        string_begin++;
    }

    

}


void  IndexingParser::parse_collection(
    std::vector<std::pair<int,std::vector<label_set_converter::LabelSetElement>>> sets_collection,
    const std::string& file_path){

    std::ifstream index_file(file_path);
    if (!index_file) {
      throw std::runtime_error("ERROR: Problem with opening the file '" + file_path + "' in Indexing::parse_collection_efficient.");
    }
  // Read the trees line by line, parse, and move into the container.
  std::string indexing_string;
  int set_count=0;
  while (std::getline(index_file, indexing_string)) {
    std::cout<<"set: "<<set_count<<std::endl;
    std::pair<int,std::vector<label_set_converter::LabelSetElement>> set;
    parse_single(indexing_string,set);
    sets_collection.push_back(set); // -> This invokes a move constructor (due to push_back(<rvalue>)).
    set_count++;
  }
  index_file.close();

  }