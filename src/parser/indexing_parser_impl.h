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


  void  IndexingParser::parse_single_feature(
    const std::string& index_string, 
    std::pair<int,std::vector<label_feature_set_converter::LabelSetElement>>& set,int flag){

      auto string_begin=index_string.begin();
      auto string_end=index_string.end();


      //format: <tree_size>[label_set][[label_set]]...
      //[label_id,weight,weight_so_far{node_set}{node_set}...]
      //{postorder_id,number_of_up, down, left, right<histogram>,<histogram>....}
      //<(key,value)(key,value)...>
      

      //Get <tree_size>
      if(*string_begin=='<'){
        string_begin++;
        std::string tree_size;
        while(*string_begin!='>'){
          tree_size+=*string_begin;
          string_begin++;
        }

        set.first=std::stoi(tree_size);
      }
      
      //[...][...]
      while (string_begin!=string_end){

        //step into one [label_set]
        if(*string_begin=='['){
          string_begin++;
          std::vector<std::string> label_para={""};
          label_feature_set_converter::LabelSetElement element;

          while(*string_begin!=']'){
            //processing [label_id,weight,weight_so_far...]
            if(*string_begin!='{'){
              if(*string_begin==',')
                label_para.push_back("");
              else
                label_para.back().push_back(*string_begin);
            }
            //step into {node set}
            else{              
              string_begin++;
              std::vector<std::string> node_para={""};
              label_feature_set_converter::StructuralVector node_vc;

              std::unordered_map<std::string,int> l_histograms[4];
              std::unordered_map<int,int> dl_histograms[4];
              int histogram_counter=0;

              // {postorder_id,number_of_up, down, left, right<histogram>,<histogram>....}
              while(*string_begin!='}'){

                 // {postorder_id,number_of_up, down, left, right,...}
                if(*string_begin!='<'){
                  if(*string_begin==',')
                    node_para.push_back("");
                  else
                    node_para.back().push_back(*string_begin);
                }
                
                //step into <hitogram>
                else{

                  string_begin++;

                  while(*string_begin!='>'){
                    
                    //step into (key,value)
                    if(*string_begin=='('){
                      string_begin++;
                      std::string kv[2];
                      int kv_counter=0;
                      while(*string_begin!=')'){
                        if(*string_begin==',')
                          kv_counter++;
                        else
                          kv[kv_counter]+=*string_begin;
                        string_begin++;
                      }
                      if(flag==1){
                        l_histograms[histogram_counter].emplace(kv[0],std::stoi(kv[1]));
                      }
                      else{
                        dl_histograms[histogram_counter].emplace(std::stoi(kv[0]),std::stoi(kv[1]));
                      }
                    }

                    string_begin++;
                  }
                  histogram_counter++;
                }              
                string_begin++;
              }
        
              node_vc.postorder_id=std::stoi(node_para[0]);
              node_vc.number_nodes_ancestor=std::stoi(node_para[1]);
              node_vc.number_nodes_descendant=std::stoi(node_para[2]);
              node_vc.number_nodes_left=std::stoi(node_para[3]);
              node_vc.number_nodes_right=std::stoi(node_para[4]);

              if(flag==1){
                node_vc.label_histogram[0]=l_histograms[0];
                node_vc.label_histogram[1]=l_histograms[1];
                node_vc.label_histogram[2]=l_histograms[2];
                node_vc.label_histogram[3]=l_histograms[3];
              }
              else if(flag==2){
                node_vc.degree_histogram[0]=dl_histograms[0];
                node_vc.degree_histogram[1]=dl_histograms[1];
                node_vc.degree_histogram[2]=dl_histograms[2];
                node_vc.degree_histogram[3]=dl_histograms[3];
              }
              else if(flag==3){
                node_vc.leaf_histogram[0]=dl_histograms[0];
                node_vc.leaf_histogram[1]=dl_histograms[1];
                node_vc.leaf_histogram[2]=dl_histograms[2];
                node_vc.leaf_histogram[3]=dl_histograms[3];
              }


              element.struct_vect.push_back(node_vc);

            }          
            string_begin++;
          }

          element.id=std::stoi(label_para[0]);
          element.weight=stoi(label_para[1]);
          element.weight_so_far=stoi(label_para[2]);

          set.second.push_back(element);                  
        }

        string_begin++;
      }
  }

  void  IndexingParser::parse_single_feature_inserted(
    const std::string& index_string, 
    std::pair<int,std::vector<label_feature_set_converter::LabelSetElement>>& set,
    int flag){

      std::pair<int,std::vector<label_feature_set_converter::LabelSetElement>> set_tmp;
      parse_single_feature(index_string,set_tmp,flag);
      

      //label set size
      for(size_t i=0;i<set.second.size();i++){
        for(size_t j=0; j<set.second[i].struct_vect.size();j++){
            if(flag==2){
              set.second[i].struct_vect[j].degree_histogram[0]=set_tmp.second[i].struct_vect[j].degree_histogram[0];
              set.second[i].struct_vect[j].degree_histogram[1]=set_tmp.second[i].struct_vect[j].degree_histogram[1];
              set.second[i].struct_vect[j].degree_histogram[2]=set_tmp.second[i].struct_vect[j].degree_histogram[2];
              set.second[i].struct_vect[j].degree_histogram[3]=set_tmp.second[i].struct_vect[j].degree_histogram[3];
            }
            else if(flag==3){
              set.second[i].struct_vect[j].leaf_histogram[0]=set_tmp.second[i].struct_vect[j].leaf_histogram[0];
              set.second[i].struct_vect[j].leaf_histogram[1]=set_tmp.second[i].struct_vect[j].leaf_histogram[1];
              set.second[i].struct_vect[j].leaf_histogram[2]=set_tmp.second[i].struct_vect[j].leaf_histogram[2];
              set.second[i].struct_vect[j].leaf_histogram[3]=set_tmp.second[i].struct_vect[j].leaf_histogram[3];         
            }
        }
      }


    }

    void IndexingParser::parse_collection_feature(
    std::vector<std::pair<int,std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
    const std::string& file_name,
    const std::string& flag){

      std::string label_file_path="/home/bowen/dataset/indexing/"+file_name+".label_indexing";
      std::string degree_file_path="/home/bowen/dataset/indexing/"+file_name+".degree_indexing";
      std::string leaf_file_path="/home/bowen/dataset/indexing/"+file_name+".leaf_indexing";


      bool insetred=false;
      
      if(flag[0]=='1'){
        std::ifstream index_file(label_file_path);
        if (!index_file) {
          throw std::runtime_error("ERROR: Problem with opening the label  '" + label_file_path + "' in Indexing::parse_collection_efficient.");
        }
        std::string indexing_string;
        
        while (std::getline(index_file, indexing_string)){
          std::pair<int,std::vector<label_feature_set_converter::LabelSetElement>> set;
          parse_single_feature(indexing_string,set,1);
          sets_collection.push_back(set);
        }

        index_file.close();
        insetred=true;    
      }

      if(flag[1]=='1'){

        std::ifstream index_file(degree_file_path);
        if (!index_file) {
          throw std::runtime_error("ERROR: Problem with opening the label  '" + degree_file_path + "' in Indexing::parse_collection_efficient.");
        }
        std::string indexing_string;

        if(insetred){
          int line_counter=0;
          while (std::getline(index_file, indexing_string)){
            parse_single_feature_inserted(indexing_string,sets_collection[line_counter],2);
            line_counter++;
          }
        }
        else{
          while (std::getline(index_file, indexing_string)){
            std::pair<int,std::vector<label_feature_set_converter::LabelSetElement>> set;
            parse_single_feature(indexing_string,set,2);
            sets_collection.push_back(set);
          }

          index_file.close();
          insetred=true;   
        }
      }

      if(flag[2]=='1'){
        std::ifstream index_file(leaf_file_path);
        if (!index_file) {
          throw std::runtime_error("ERROR: Problem with opening the label  '" + leaf_file_path + "' in Indexing::parse_collection_efficient.");
        }
        std::string indexing_string;

        if(insetred){
          int line_counter=0;
          while (std::getline(index_file, indexing_string)){
            parse_single_feature_inserted(indexing_string,sets_collection[line_counter],3);
            line_counter++;
          }
        }
        else{
          while (std::getline(index_file, indexing_string)){
            std::pair<int,std::vector<label_feature_set_converter::LabelSetElement>> set;
            parse_single_feature(indexing_string,set,3);
            sets_collection.push_back(set);
          }
          index_file.close();
          insetred=true;   
        }
      }
      
    }
