#pragma once
using namespace join;


template <typename Label, typename VerificationAlgorithm>
TJoin_Indexing<Label, VerificationAlgorithm>::TJoin_Indexing() {
  ld_ = label::LabelDictionary<Label>();
  pre_candidates_ = 0;
  sum_subproblem_counter_ = 0;
  number_of_labels_ = 0;
  il_lookups_ = 0;
}

template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::indexing(std::string filename,std::vector<node::Node<Label>>& trees_collection){

  std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;

  std::ofstream outfile("/home/bowen/dataset/indexing/"+filename+".indexing");

  // Convert trees to sets and get the result.
  convert_trees_to_sets(trees_collection, sets_collection);

  for(auto set : sets_collection){

    std::string set_string;
    set_string+="<"+std::to_string(set.first)+">";

    for(auto element : set.second){
      set_string+="[";
      set_string+=std::to_string(element.id)+",";
      set_string+=std::to_string(element.weight)+",";
      set_string+=std::to_string(element.weight_so_far);
      for(auto node: element.struct_vect){     
        set_string+="{";
        set_string+=std::to_string(node.postorder_id)+",";
        set_string+=std::to_string(node.number_nodes_ancestor)+",";
        set_string+=std::to_string(node.number_nodes_descendant)+",";
        set_string+=std::to_string(node.number_nodes_left)+",";
        set_string+=std::to_string(node.number_nodes_right);
        set_string+="}";
      }
      set_string+="]";
    }

    //std::cout<<set_string<<std::endl;
    outfile<<set_string<<"\n";

  }


}



template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::execute_join(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {


  //std::cout<<"retrive_candidates----->"<<std::endl;
  // Retrieves candidates from the candidate index.
  retrieve_candidates(sets_collection, candidates, distance_threshold);

  //std::cout<<"upperbound----->"<<std::endl;
  //Use the label guided mapping upper bound to send candidates immediately .
  upperbound(trees_collection, candidates, join_result, distance_threshold);

  //std::cout<<"verify------->"<<std::endl;
  // Verify all computed join candidates and return the join result.
  verify_candidates(trees_collection, candidates, join_result, distance_threshold);
}

template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::convert_trees_to_sets(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection) {

  // Convert trees to sets and get the result.
  label_set_converter::Converter<Label> lsc;
  lsc.assignFrequencyIdentifiers(trees_collection, sets_collection);
  number_of_labels_ = lsc.get_number_of_labels();
}

template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::feature_indexing(
  std::string filename, 
  std::vector<node::Node<Label>>& trees_collection){
  
  int tree_counter=0;

  for(auto& tree : trees_collection){

    std::cout<<"tree: "<<tree_counter<<std::endl;
    tree_counter++;
    
    int preorder=0;
    int postorder=0;

    //std::cout<<"postorder"<<std::endl;
    std::vector<node::Node<Label>*> tree_postorder_collection;

    postordering(tree,postorder, tree_postorder_collection);
    preordering (tree,preorder);


    for(int i=0;i<tree_postorder_collection.size();i++){


      int counter=0;
      while(counter<i){
        //std::cout<<"node with fewer post order"<<std::endl;
        //with a node x, for the other nodes x', preorder: x' < x, postorder: x' < x, x' is left to x
        if(tree_postorder_collection[counter]->pre_order < tree_postorder_collection[i]->pre_order){

          int degree=tree_postorder_collection[counter]->children_.size();
          int leaf_dist=tree_postorder_collection[counter]->leaf_dist;
          std::string label=tree_postorder_collection[counter]->label().to_string();

          //degree histogram
          if(tree_postorder_collection[i]->degree_histogram[2][degree]!=-1){
            tree_postorder_collection[i]->degree_histogram[2][degree]++;
          }
          else
            tree_postorder_collection[i]->degree_histogram[2][degree]=1;

          
          //leaf_dist histogram
          if(tree_postorder_collection[i]->leaf_histogram[2][leaf_dist]!=-1){
            tree_postorder_collection[i]->leaf_histogram[2][leaf_dist]++;
          }
          else
            tree_postorder_collection[i]->leaf_histogram[2][leaf_dist]=1;

          //label histogram
          if(tree_postorder_collection[i]->label_histogram[2][label]!=-1){
            tree_postorder_collection[i]->label_histogram[2][label]++;
          }
          else
            tree_postorder_collection[i]->label_histogram[2][label]=1;      
        }
         //with a node x, for the other nodes x', preorder: x' > x, postorder: x' < x, x' is  down to x
        else{

          int degree=tree_postorder_collection[counter]->children_.size();
          int leaf_dist=tree_postorder_collection[counter]->leaf_dist;
          std::string label=tree_postorder_collection[counter]->label().to_string();

          //degree histogram
          if(tree_postorder_collection[i]->degree_histogram[1][degree]!=-1){
            tree_postorder_collection[i]->degree_histogram[1][degree]++;
          }
          else
            tree_postorder_collection[i]->degree_histogram[1][degree]=1;

          
          //leaf_dist histogram
          if(tree_postorder_collection[i]->leaf_histogram[1][leaf_dist]!=-1){
            tree_postorder_collection[i]->leaf_histogram[1][leaf_dist]++;
          }
          else
            tree_postorder_collection[i]->leaf_histogram[1][leaf_dist]=1;

          //label histogram
          if(tree_postorder_collection[i]->label_histogram[1][label]!=-1){
            tree_postorder_collection[i]->label_histogram[1][label]++;
          }
          else
            tree_postorder_collection[i]->label_histogram[1][label]=1;  

        }


        counter++;
     }

      counter=tree_postorder_collection.size()-1;
      //std::cout<<counter<<std::endl;
      while(counter > i){

         //std::cout<<"node with fewer post order"<<std::endl;
         //with a node x, for the other nodes x', preorder: x' < x, postorder: x' > x, x' is on to x
        if(tree_postorder_collection[counter]->pre_order < tree_postorder_collection[i]->pre_order){

          int degree=tree_postorder_collection[counter]->children_.size();
          int leaf_dist=tree_postorder_collection[counter]->leaf_dist;
          std::string label=tree_postorder_collection[counter]->label().to_string();

          //degree histogram
          if(tree_postorder_collection[i]->degree_histogram[0][degree]!=-1){
            tree_postorder_collection[i]->degree_histogram[0][degree]++;
          }
          else
            tree_postorder_collection[i]->degree_histogram[0][degree]=1;

          
          //leaf_dist histogram
          if(tree_postorder_collection[i]->leaf_histogram[0][leaf_dist]!=-1){
            tree_postorder_collection[i]->leaf_histogram[0][leaf_dist]++;
          }
          else
            tree_postorder_collection[i]->leaf_histogram[0][leaf_dist]=1;

          //label histogram
          if(tree_postorder_collection[i]->label_histogram[0][label]!=-1){
            tree_postorder_collection[i]->label_histogram[0][label]++;
          }
          else
            tree_postorder_collection[i]->label_histogram[0][label]=1;      
        }

        //with a node x, for the other nodes x', preorder: x' > x, postorder: x' > x, x' is on to x
        else{
          int degree=tree_postorder_collection[counter]->children_.size();
          int leaf_dist=tree_postorder_collection[counter]->leaf_dist;
          std::string label=tree_postorder_collection[counter]->label().to_string();

          //degree histogram
          if(tree_postorder_collection[i]->degree_histogram[3][degree]!=-1){
            tree_postorder_collection[i]->degree_histogram[3][degree]++;
          }
          else
            tree_postorder_collection[i]->degree_histogram[3][degree]=1;

          
          //leaf_dist histogram
          if(tree_postorder_collection[i]->leaf_histogram[3][leaf_dist]!=-1){
            tree_postorder_collection[i]->leaf_histogram[3][leaf_dist]++;
          }
          else
            tree_postorder_collection[i]->leaf_histogram[3][leaf_dist]=1;

          //label histogram
          if(tree_postorder_collection[i]->label_histogram[3][label]!=-1){
            tree_postorder_collection[i]->label_histogram[3][label]++;
          }
          else
            tree_postorder_collection[i]->label_histogram[3][label]=1;  

        }
        counter--;
      }



    }

  }
  
  std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>> sets_collection;
  convert_trees_to_feature_sets(trees_collection,sets_collection);

  std::ofstream label_outfile("/home/bowen/dataset/indexing/"+filename+".label_indexing");
  std::ofstream degree_outfile("/home/bowen/dataset/indexing/"+filename+".degree_indexing");
  std::ofstream leaf_outfile("/home/bowen/dataset/indexing/"+filename+".leaf_indexing");

  for(auto set : sets_collection){

    std::string set_string;
    set_string+="<"+std::to_string(set.first)+">";

    for(auto element : set.second){
      set_string+="[";
      set_string+=std::to_string(element.id)+",";
      set_string+=std::to_string(element.weight)+",";
      set_string+=std::to_string(element.weight_so_far);
      for(auto node: element.struct_vect){     
        set_string+="{";
        set_string+=std::to_string(node.postorder_id)+",";
        set_string+=std::to_string(node.number_nodes_ancestor)+",";
        set_string+=std::to_string(node.number_nodes_descendant)+",";
        set_string+=std::to_string(node.number_nodes_left)+",";
        set_string+=std::to_string(node.number_nodes_right);
        
        for(size_t i=0;i<4;i++){
          set_string+="<";
          for(auto map: node.label_histogram[i]){
            set_string+="(";
            set_string+=map.first+","+std::to_string(map.second);
            set_string+=")";
          }

          set_string+=">";
        }
        
        set_string+="}";
      }
      set_string+="]";
    }

    //std::cout<<set_string<<std::endl;
    label_outfile<<set_string<<"\n";

  }


  for(auto set : sets_collection){

    std::string set_string;
    set_string+="<"+std::to_string(set.first)+">";

    for(auto element : set.second){
      set_string+="[";
      set_string+=std::to_string(element.id)+",";
      set_string+=std::to_string(element.weight)+",";
      set_string+=std::to_string(element.weight_so_far);
      for(auto node: element.struct_vect){     
        set_string+="{";
        set_string+=std::to_string(node.postorder_id)+",";
        set_string+=std::to_string(node.number_nodes_ancestor)+",";
        set_string+=std::to_string(node.number_nodes_descendant)+",";
        set_string+=std::to_string(node.number_nodes_left)+",";
        set_string+=std::to_string(node.number_nodes_right);
        
        for(size_t i=0;i<4;i++){
          set_string+="<";
          for(auto map: node.degree_histogram[i]){
            set_string+="(";
            set_string+=std::to_string(map.first)+","+std::to_string(map.second);
            set_string+=")";
          }

          set_string+=">";
        }
        
        set_string+="}";
      }
      set_string+="]";
    }

    //std::cout<<set_string<<std::endl;
    degree_outfile<<set_string<<"\n";

  }

  for(auto set : sets_collection){

    std::string set_string;
    set_string+="<"+std::to_string(set.first)+">";

    for(auto element : set.second){
      set_string+="[";
      set_string+=std::to_string(element.id)+",";
      set_string+=std::to_string(element.weight)+",";
      set_string+=std::to_string(element.weight_so_far);
      for(auto node: element.struct_vect){     
        set_string+="{";
        set_string+=std::to_string(node.postorder_id)+",";
        set_string+=std::to_string(node.number_nodes_ancestor)+",";
        set_string+=std::to_string(node.number_nodes_descendant)+",";
        set_string+=std::to_string(node.number_nodes_left)+",";
        set_string+=std::to_string(node.number_nodes_right);
        
        for(size_t i=0;i<4;i++){
          set_string+="<";
          for(auto map: node.leaf_histogram[i]){
            set_string+="(";
            set_string+=std::to_string(map.first)+","+std::to_string(map.second);
            set_string+=")";
          }

          set_string+=">";
        }
        
        set_string+="}";
      }
      set_string+="]";
    }

    //std::cout<<set_string<<std::endl;
    leaf_outfile<<set_string<<"\n";

  }
  









}


template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::postordering(
  node::Node<Label>& tree, int& order,
  std::vector<node::Node<Label>*>& tree_postorder_collection){

  for(auto& child: tree.children_){
    postordering(child,order,tree_postorder_collection);
  }

  tree.post_order=order;
  tree_postorder_collection.push_back(&tree);
  order++;

  if(tree.is_leaf()){
    tree.leaf_dist=0;
  }
  else{
    int max=-1;
    for(auto child: tree.children_){
      if(child.leaf_dist>max)
        max=child.leaf_dist;
    }

    tree.leaf_dist=max+1;
  }
}

template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::preordering(
  node::Node<Label>& tree, int& order){

  tree.pre_order=order;
  order++;

  for(auto& child: tree.children_){
    preordering(child,order);
  }
}


template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::convert_trees_to_feature_sets(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection) {

  // Convert trees to sets and get the result.
  label_feature_set_converter::Converter<Label> lfsc;
  lfsc.assignFrequencyIdentifiers(trees_collection,sets_collection);
  number_of_labels_=lfsc.get_number_of_labels();
  // number_of_labels_ = lsc.get_number_of_labels();
}



template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::retrieve_candidates(
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& candidates,
    const double distance_threshold) {

  // Initialize candidate index.
  candidate_index::CandidateIndex c_index;

  // Retrieve candidates from the candidate index.
  c_index.lookup(sets_collection, candidates, number_of_labels_, distance_threshold);

  // Copy the number of pre-candidates.
  pre_candidates_ = c_index.get_number_of_pre_candidates();
  // Copy the number of inverted list lookups.
  il_lookups_ = c_index.get_number_of_il_lookups();
}

template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::upperbound(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {
  
  typename VerificationAlgorithm::AlgsCostModel cm(ld_);
  ted_ub::LGMTreeIndex<typename VerificationAlgorithm::AlgsCostModel> lgm_algorithm(cm);
  // TODO: Index trees only once for LGM And Verification using a TreeIndex
  //       that is a superset of TreeIndexLGM and VerificationAlgorithm::AlgsTreeIndex.
  node::TreeIndexLGM ti_1;
  node::TreeIndexLGM ti_2;
  
  std::vector<std::pair<int, int>>::iterator it = candidates.begin();
  while(it != candidates.end()) {
    node::index_tree(ti_1, trees_collection[it->first], ld_, cm);
    node::index_tree(ti_2, trees_collection[it->second], ld_, cm);
    double ub_value = lgm_algorithm.ted_k(ti_1, ti_2, distance_threshold);
    if(ub_value <= distance_threshold) {
      join_result.emplace_back(it->first, it->second, ub_value);
      *it = candidates.back();
      candidates.pop_back();
    }
    else {
      ++it;
    }
  }
}

template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

  typename VerificationAlgorithm::AlgsCostModel cm(ld_);
  VerificationAlgorithm ted_algorithm(cm);
  typename VerificationAlgorithm::AlgsTreeIndex ti_1;
  typename VerificationAlgorithm::AlgsTreeIndex ti_2;

  // Verify each pair in the candidate set
  for(const auto& pair: candidates) {
    node::index_tree(ti_1, trees_collection[pair.first], ld_, cm);
    node::index_tree(ti_2, trees_collection[pair.second], ld_, cm);
    double ted_value = ted_algorithm.ted_k(ti_1, ti_2, distance_threshold);
    if(ted_value <= distance_threshold)
      join_result.emplace_back(pair.first, pair.second, ted_value);
    
    // Sum up all number of subproblems
    sum_subproblem_counter_ += ted_algorithm.get_subproblem_count();
  }
}

// template <typename Label, typename VerificationAlgorithm>
// long long int
//     TJoinTI<Label, VerificationAlgorithm>::get_number_of_pre_candidates() const {
//   return pre_candidates_;
// }

// template <typename Label, typename VerificationAlgorithm>
// long long int TJoinTI<Label, VerificationAlgorithm>::get_subproblem_count() const {
//   return sum_subproblem_counter_;
// }

// template <typename Label, typename VerificationAlgorithm>
// long long int
//     TJoinTI<Label, VerificationAlgorithm>::get_number_of_il_lookups() const {
//   return il_lookups_;
// }


template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::execute_feature_join(
  std::vector<node::Node<Label>>& trees_collection,
  std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
  std::vector<std::pair<int, int>>& candidates,
  std::vector<join::JoinResultElement>& join_result,
  const double distance_threshold){

    retrive_feature_candidates(sets_collection,candidates,distance_threshold);

  }


template <typename Label, typename VerificationAlgorithm>
void TJoin_Indexing<Label, VerificationAlgorithm>::retrive_feature_candidates(
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& candidates,
    const double distance_threshold){

    // Initialize candidate index.
    feature_candidate_index::CandidateIndex fc_candidates;

    fc_candidates.lookup(sets_collection,candidates,5,distance_threshold);



    

    }