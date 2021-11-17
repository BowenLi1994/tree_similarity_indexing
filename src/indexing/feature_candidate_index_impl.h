#pragma once
using namespace feature_candidate_index;

CandidateIndex::CandidateIndex() {
  pre_candidates_ = 0;
  il_lookups_ = 0;
  same_label_compa=0;
}

void CandidateIndex::lookup(
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& join_candidates,
    const int number_of_labels, 
    const double distance_threshold) {

  
  // inverted list index.
  std::vector<candidate_index::InvertedListElement> il_index(number_of_labels);
  // containing specific data of a set. (e.g. actual overlap, index prefix)
  std::vector<candidate_index::SetData> set_data(sets_collection.size());
  // position in label set while processing r
  std::size_t p = 0;

  // iterate through all sets in the given collection
  std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>::iterator r_it = sets_collection.begin();
  for (; r_it != sets_collection.end(); ++r_it) {
    std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>& r_pair = *r_it; // dereference iterator to current set once
    std::vector<label_feature_set_converter::LabelSetElement> r = r_pair.second; // dereference iterator to current set once
    int r_id = r_it - sets_collection.begin(); // identifier for r (line number)
    std::vector<int> M; // holds the set identifiers of the candidate pairs, 
                                 // the overlap is stored in the set_data
    int r_size = sets_collection[r_id].first; // number of elements in r

    // *****************************
    // ** Generate pre candidates **
    // *****************************
    // add all small trees that does not have to share a common label in the prefix
    if(r_size <= distance_threshold){
      //std::cout<<"small tree"<<std::endl; 
      for(int i = 0; i < r_id; ++i) {
        if(set_data[i].overlap == 0) 
          M.push_back(i); // if not, add it to the candidate set M
        ++set_data[i].overlap; // increase overlap for set i
      }
    }

    // iterate through probing prefix elements and extend the candidate set
    p = 0;
    // until tau + 1 nodes of the probing set are processed
    while(p < r.size()) {
      // remove all entries in the inverted list index up to the position where 
      // the size is greater than the lower bound
      
      for(std::size_t s = il_index[r[p].id].offset; s < il_index[r[p].id].element_list.size() &&
          sets_collection[il_index[r[p].id].element_list[s].first].first < r_size - distance_threshold; s++){ 
          ++il_index[r[p].id].offset;
        }
        

      // iterate through all remaining sets for the current token r[p] in the 
      // inverted list index and add them to the candidates

      //std::cout<<" finding candidates"<<std::endl;
      for(std::size_t s = il_index[r[p].id].offset; s < il_index[r[p].id].element_list.size(); s++) {
        int set = il_index[r[p].id].element_list[s].first;
        int pos = il_index[r[p].id].element_list[s].second;
        // increase the number of lookups in the inverted list
        ++il_lookups_;

        int tau_valid = structural_mapping(r[p], sets_collection[set].second[pos], distance_threshold);
        //std::cout<<"tau vaild: "<<tau_valid<<std::endl;
        if(tau_valid != 0 && set_data[set].overlap == 0) 
          M.push_back(set); // if not, add it to the candidate set M
        set_data[set].overlap += tau_valid;
        
      }
      // stop as soon as tau + 1 elements have been discovered
      p++;
      if(r[p-1].weight_so_far > distance_threshold + 1)
        break;
    }

    // store last postition of the index of the label set
    set_data[r_id].prefix = p;
    // count number of precandidates
    pre_candidates_ += M.size();

    
    // add all elements in the prefix of r in the inverted list
    for(int p = 0; p < set_data[r_id].prefix; p++)
      il_index[r[p].id].element_list.push_back(std::make_pair(r_id, p));


    // *****************************
    // *** Verify pre candidates ***
    // *****************************
    // compute structural filter for each candidate (r, s) in M
    //std::cout<<"**<-- Verify pre candidates -->**"<<std::endl;
    for (int m: M) {

      //std::cout<<"m: "<<m<<std::endl;
      std::vector<label_feature_set_converter::LabelSetElement>& s = sets_collection[m].second;
      // prefix positions for sets r and s in the candidate pair
      std::size_t pr = 0, ps = 0;

      // check last prefix position; the smaller one starts at prefix position, 
      // the greater one starts at the overlap
      if (r[set_data[r_id].prefix-1].id > s[set_data[m].prefix-1].id) {

        //std::cout<<" probing T greater than T' in canidate set"<<std::endl;
        for(; r[pr].weight_so_far < set_data[m].overlap && pr < r.size(); ++pr) {}
        ++pr;
        ps = set_data[m].prefix;
      } else {
        //std::cout<<" probing T  smaller than T' in canidate set"<<std::endl;
        pr = set_data[r_id].prefix;
        for(; s[ps].weight_so_far < set_data[m].overlap && ps < s.size(); ++ps) {}
        ++ps;
      }
      //std::cout<<"pr: "<<pr<<" ps: "<<ps<<std::endl;

      int maxr = r_size - r[pr-1].weight_so_far + set_data[m].overlap;
      int maxs = sets_collection[m].first - s[ps-1].weight_so_far + set_data[m].overlap;

      //std::cout<<"maxr: "<<maxr<<" maxs: "<<maxs<<std::endl;
      // overlap needed for threshold tau between r and s
      const double eqoverlap = r_size - distance_threshold;

      //std::cout<<"eqoverlap: "<<eqoverlap<<std::endl;

      // verify if r and s belong to the resultset, computed the structural filter
      if (structural_filter(r, s, eqoverlap, set_data[m].overlap, pr, ps, distance_threshold, maxr, maxs))
        join_candidates.emplace_back(r_id, m);

      // reset overlap in set_data
      set_data[m].overlap = 0;
    }
  }
}

bool CandidateIndex::structural_filter(
    std::vector<label_feature_set_converter::LabelSetElement>& r, 
    std::vector<label_feature_set_converter::LabelSetElement>& s, 
    const double t, 
    int olap, 
    int pr, 
    int ps, 
    int distance_threshold, 
    int maxr, 
    int maxs) {


  //std::cout<<"structural filter "<<std::endl;

  
  // starting from pr and ps, check if the overlap exceeds the threshold
  // stop if the threshold is reached or cannot be reached anymore
  while (maxr >= t && maxs >= t && olap < t) {
    //std::cout<<"checking step: "<<std::endl;
    if (r[pr].id == s[ps].id) {
      int tau_valid = structural_mapping(r[pr], s[ps], distance_threshold);
      olap += tau_valid;
      maxr -= r[pr].weight - tau_valid;
      maxs -= s[ps].weight - tau_valid;
      ++pr; ++ps; 
    } else if (r[pr].id < s[ps].id) {
      maxr -= r[pr].weight;
      ++pr;
    } else {
      maxs -= s[ps].weight;
      ++ps;
    }
  }

  return olap >= t;
}

int CandidateIndex::structural_mapping(
    label_feature_set_converter::LabelSetElement& sv_r, 
    label_feature_set_converter::LabelSetElement& sv_s,
    const double distance_threshold) {
  
  //std::cout<<"strucural mapping"<<std::endl;

  // std::cout<<" set in tree T: "<<sv_r.id<<std::endl;
  // std::cout<<" set in tree T': "<<sv_s.id<<std::endl;
  int tau_valid = 0;
  // check if duplicates exist
  if(sv_s.weight == 1 && sv_r.weight == 1) {
    // no duplicates -> do positional filter
    if(feature_filter(sv_s.struct_vect[0],sv_r.struct_vect[0],distance_threshold)!=-1)
      return 1;
  } else {

    //std::cout<<"neighter of them has mutilple labels"<<std::endl;
    std::reference_wrapper<label_feature_set_converter::LabelSetElement> se = std::ref(sv_r);
    std::reference_wrapper<label_feature_set_converter::LabelSetElement> le = std::ref(sv_s);
    if(sv_s.weight < sv_r.weight) {
      se = std::ref(sv_s);
      le = std::ref(sv_r);
    }

    std::size_t pid_lower_bound_start = 0;
    for(int i = 0; i < se.get().weight; ++i) {
      label_feature_set_converter::StructuralVector& left_hand_duplicate = se.get().struct_vect[i];
      int left_side_k_window = std::max(0.0, left_hand_duplicate.postorder_id - distance_threshold);
      //std::cout<<"left side k window: "<<left_side_k_window<<std::endl;
      //skip duplicates at the beginning that doesn't satisfy the postorder lower bound
      while(pid_lower_bound_start < le.get().struct_vect.size()
          && le.get().struct_vect[pid_lower_bound_start].postorder_id < left_side_k_window) {
        ++pid_lower_bound_start;
      }
      //std::cout<<"pid_lower_bound_start: "<<pid_lower_bound_start<<std::endl;
      if(pid_lower_bound_start == le.get().struct_vect.size())
        break;

      for(int j = pid_lower_bound_start; j < le.get().weight; ++j) {
        label_feature_set_converter::StructuralVector& right_hand_duplicate = le.get().struct_vect[j];
        // postorder id in right hand side duplicates is too large to satisfy the postorder lower bound
        if(right_hand_duplicate.postorder_id > distance_threshold + left_hand_duplicate.postorder_id)
          break;
        if(feature_filter(right_hand_duplicate,left_hand_duplicate,distance_threshold)!=-1){
          ++tau_valid;
          ++same_label_compa;
          break;
        }
        // if(abs(right_hand_duplicate.number_nodes_left - left_hand_duplicate.number_nodes_left) + 
        //    abs(right_hand_duplicate.number_nodes_right - left_hand_duplicate.number_nodes_right) +
        //    abs(right_hand_duplicate.number_nodes_ancestor - left_hand_duplicate.number_nodes_ancestor) +
        //    abs(right_hand_duplicate.number_nodes_descendant - left_hand_duplicate.number_nodes_descendant) <= distance_threshold) {
        //   ++tau_valid;
        //   break;
        // }
      }
    }
  }
  return tau_valid;
}

long int CandidateIndex::get_number_of_pre_candidates() const {
  return pre_candidates_;
}

void CandidateIndex::set_number_of_pre_candidates(
    const long int pc) {
  pre_candidates_ = pc;
}

long int CandidateIndex::get_number_of_il_lookups() const {
  return il_lookups_;
}

int CandidateIndex::feature_filter(label_feature_set_converter::StructuralVector& sv1, 
  label_feature_set_converter::StructuralVector& sv2, 
  const double threshold){

    int label_distance=0;
    int degree_distance=0;
    if(std::abs(sv1.number_nodes_ancestor-sv2.number_nodes_ancestor)+
    std::abs(sv1.number_nodes_descendant-sv2.number_nodes_descendant)+
    std::abs(sv1.number_nodes_left-sv2.number_nodes_left)+
    std::abs(sv1.number_nodes_right-sv2.number_nodes_right)> threshold)
      return -1;  
    else{
      //label distance:
     for(int i=0;i<4;i++){
       label_distance+=histogram_distance_dist_2(sv1.label_histogram[i],sv2.label_histogram[i]);
     }

     if(label_distance/2.0 > threshold)
      return -1;
     else{   
         int degree_distance=0;
         for(int i=0;i<4;i++){
           degree_distance+=histogram_distance_dist(sv1.degree_histogram[i],sv2.degree_histogram[i]);
         }
         if(degree_distance/3.0> threshold)
          return -1;
         else
          return std::max(label_distance,degree_distance);
     }    
    }

    return std::max(label_distance,degree_distance);

  }

  int CandidateIndex::histogram_distance_dist(const std::unordered_map<int,int>& m1,const std::unordered_map<int,int>& m2){

    int distance=0;

    for(auto kv: m1){
     auto it=m2.find(kv.first);
     if(it!=m2.end()){
       distance+=std::abs(it->second-kv.second);
     }
     else{
       distance+=kv.second;
     }
    }

    for(auto kv:m2){
      auto it=m1.find(kv.first);
      if(it==m1.end()){
        distance+=kv.second;
      }
    }
    //std::cout<<"distance: "<<distance<<std::endl;
    return distance;
  }

  int CandidateIndex::histogram_distance_dist_2(
    const std::unordered_map<std::string,int>& m1,
    const std::unordered_map<std::string,int>& m2){

    int distance=0;

    for(auto kv: m1){
     auto it=m2.find(kv.first);
     if(it!=m2.end()){
       distance+=std::abs(it->second-kv.second);
     }
     else{
       distance+=kv.second;
     }
    }

    for(auto kv:m2){
      auto it=m1.find(kv.first);
      if(it==m1.end()){
        distance+=kv.second;
      }
    }
    return distance;
  }

int CandidateIndex::structural_mapping_ti(label_feature_set_converter::LabelSetElement& sv_r, 
  label_feature_set_converter::LabelSetElement& sv_s,
  std::vector<std::vector<std::pair<int,double>>>& distance_vect,
  const double distance_threshold){

  // std::cout<<"strucural mapping: id-->"<<sv_r.id<<std::endl;
  // std::cout<<"sv_r weight: "<<sv_r.weight<<" sv_s weight: "<<sv_s.weight<<std::endl;

  // std::cout<<" set in tree T: "<<sv_r.id<<std::endl;
  // std::cout<<" set in tree T': "<<sv_s.id<<std::endl;
  int tau_valid = 0;
  // check if duplicates exist
  if(sv_s.weight == 1 && sv_r.weight == 1) {

    //std::cout<<"T and T's only has one such label"<<std::endl;
    // no duplicates -> do positional filter
    if(feature_filter(sv_s.struct_vect[0],sv_r.struct_vect[0],distance_threshold)!=-1)
      return 1;
    // if(abs(sv_s.struct_vect[0].number_nodes_left - sv_r.struct_vect[0].number_nodes_left) + 
    //    abs(sv_s.struct_vect[0].number_nodes_right - sv_r.struct_vect[0].number_nodes_right) +
    //    abs(sv_s.struct_vect[0].number_nodes_ancestor - sv_r.struct_vect[0].number_nodes_ancestor) +
    //    abs(sv_s.struct_vect[0].number_nodes_descendant - sv_r.struct_vect[0].number_nodes_descendant) <= distance_threshold) {
    //   return 1; // one tau-valid node pair
    // }
  } else {

    //std::cout<<"neighter of them has mutilple labels"<<std::endl;
    std::reference_wrapper<label_feature_set_converter::LabelSetElement> se = std::ref(sv_r);
    std::reference_wrapper<label_feature_set_converter::LabelSetElement> le = std::ref(sv_s);
    if(sv_s.weight < sv_r.weight) {
      se = std::ref(sv_s);
      le = std::ref(sv_r);
    }
    // std::cout<<"se weight: "<<se.get().weight<<std::endl;
    // std::cout<<"le weight: "<<le.get().weight<<std::endl;
    std::size_t pid_lower_bound_start = 0;
    std::set<int> passed_node;
    for(int i = 0; i < se.get().weight; ++i) {
      if(passed_node.count(i))
       continue;
      label_feature_set_converter::StructuralVector& left_hand_duplicate = se.get().struct_vect[i];
      int left_side_k_window = std::max(0.0, left_hand_duplicate.postorder_id - distance_threshold);
      //std::cout<<"left hand duplicate: "<<left_hand_duplicate.postorder_id<<std::endl;
      // std::cout<<"left side k window: "<<left_side_k_window<<std::endl;
      // skip duplicates at the beginning that doesn't satisfy the postorder lower bound
      while(pid_lower_bound_start < le.get().struct_vect.size()
          && le.get().struct_vect[pid_lower_bound_start].postorder_id < left_side_k_window) {
        ++pid_lower_bound_start;
      }
      //std::cout<<"pid_lower_bound_start: "<<pid_lower_bound_start<<std::endl;
      if(pid_lower_bound_start == le.get().struct_vect.size())
        break;

      for(int j = pid_lower_bound_start; j < le.get().weight; ++j) {
        label_feature_set_converter::StructuralVector& right_hand_duplicate = le.get().struct_vect[j];
        //std::cout<<"right hand duplicate: "<<right_hand_duplicate.postorder_id<<std::endl;
        // postorder id in right hand side duplicates is too large to satisfy the postorder lower bound
        if(right_hand_duplicate.postorder_id > distance_threshold + left_hand_duplicate.postorder_id)
          break;
        int feature_distacne;
        if(feature_distacne=feature_filter(right_hand_duplicate,left_hand_duplicate,distance_threshold)!=-1){
          //std::cout<<"pass the filter!!!!!!!!!!!!!!!!!"<<std::endl;
          ++tau_valid;
          ++same_label_compa;
          passed_node.insert(i);
          if(!distance_vect.empty()){

            for(auto p:distance_vect[i]){
              if(p.second<=distance_threshold-feature_distacne){
                passed_node.insert(p.first);
                ++tau_valid;
              }           
            }
            
          }

          break;
        }
      }
    }
  }
  return tau_valid;

}

void CandidateIndex::look_up_ti( 
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& join_candidates,
    std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>>& distance_collection,
    const int number_of_labels, 
    const double distance_threshold) {

  
  //std::cout<<"number of lable: "<<number_of_labels<<std::endl;
  // inverted list index.
  std::vector<candidate_index::InvertedListElement> il_index(number_of_labels);
  // containing specific data of a set. (e.g. actual overlap, index prefix)
  std::vector<candidate_index::SetData> set_data(sets_collection.size());
  // position in label set while processing r
  std::size_t p = 0;

  // iterate through all sets in the given collection
  std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>::iterator r_it = sets_collection.begin();
  for (; r_it != sets_collection.end(); ++r_it) {
    std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>& r_pair = *r_it; // dereference iterator to current set once
    std::vector<label_feature_set_converter::LabelSetElement> r = r_pair.second; // dereference iterator to current set once
    int r_id = r_it - sets_collection.begin(); // identifier for r (line number)
    std::vector<int> M; // holds the set identifiers of the candidate pairs, 
                                 // the overlap is stored in the set_data
    int r_size = sets_collection[r_id].first; // number of elements in r

    //std::cout<<"tree: "<<r_id<<std::endl;

    // *****************************
    // ** Generate pre candidates **
    // *****************************
    // add all small trees that does not have to share a common label in the prefix
    if(r_size <= distance_threshold){
      //std::cout<<"small tree"<<std::endl; 
      for(int i = 0; i < r_id; ++i) {
        if(set_data[i].overlap == 0) 
          M.push_back(i); // if not, add it to the candidate set M
        ++set_data[i].overlap; // increase overlap for set i
      }
    }

    // iterate through probing prefix elements and extend the candidate set
    p = 0;
    // until tau + 1 nodes of the probing set are processed
    //std::cout<<"travse all nodes"<<std::endl;
    while(p < r.size()) {
      // remove all entries in the inverted list index up to the position where 
      // the size is greater than the lower bound
      
      for(std::size_t s = il_index[r[p].id].offset; s < il_index[r[p].id].element_list.size() &&
          sets_collection[il_index[r[p].id].element_list[s].first].first < r_size - distance_threshold; s++){ 
          ++il_index[r[p].id].offset;
        }
        

      // iterate through all remaining sets for the current token r[p] in the 
      // inverted list index and add them to the candidates

      //std::cout<<" finding candidates"<<std::endl;
      for(std::size_t s = il_index[r[p].id].offset; s < il_index[r[p].id].element_list.size(); s++) {
        int set = il_index[r[p].id].element_list[s].first;
        int pos = il_index[r[p].id].element_list[s].second;
        // increase the number of lookups in the inverted list
        ++il_lookups_;
        // std::cout<<"current set: label pos-->"<<p<<std::endl;
        // std::cout<<"saving set: tree id-->"<<set<<" label pos: "<<pos<<std::endl;
        int tau_valid = structural_mapping_ti(r[p], sets_collection[set].second[pos],distance_collection[r_id][p],distance_threshold);
        //std::cout<<"tau vaild: "<<tau_valid<<std::endl;
        if(tau_valid != 0 && set_data[set].overlap == 0) 
          M.push_back(set); // if not, add it to the candidate set M
        set_data[set].overlap += tau_valid;
        
      }
      // stop as soon as tau + 1 elements have been discovered
      p++;
      if(r[p-1].weight_so_far > distance_threshold + 1)
        break;
    }

    // store last postition of the index of the label set
    set_data[r_id].prefix = p;
    // count number of precandidates
    pre_candidates_ += M.size();

    //std::cout<<"verifying pre_candidate"<<std::endl;
    // add all elements in the prefix of r in the inverted list
    for(int p = 0; p < set_data[r_id].prefix; p++)
      il_index[r[p].id].element_list.push_back(std::make_pair(r_id, p));


    // *****************************
    // *** Verify pre candidates ***
    // *****************************
    // compute structural filter for each candidate (r, s) in M
    //std::cout<<"**<-- Verify pre candidates -->**"<<std::endl;
    for (int m: M) {

      //std::cout<<"m: "<<m<<std::endl;
      std::vector<label_feature_set_converter::LabelSetElement>& s = sets_collection[m].second;
      // prefix positions for sets r and s in the candidate pair
      std::size_t pr = 0, ps = 0;

      // check last prefix position; the smaller one starts at prefix position, 
      // the greater one starts at the overlap
      if (r[set_data[r_id].prefix-1].id > s[set_data[m].prefix-1].id) {

        //std::cout<<" probing T greater than T' in canidate set"<<std::endl;
        for(; r[pr].weight_so_far < set_data[m].overlap && pr < r.size(); ++pr) {}
        ++pr;
        ps = set_data[m].prefix;
      } else {
        //std::cout<<" probing T  smaller than T' in canidate set"<<std::endl;
        pr = set_data[r_id].prefix;
        for(; s[ps].weight_so_far < set_data[m].overlap && ps < s.size(); ++ps) {}
        ++ps;
      }
      //std::cout<<"pr: "<<pr<<" ps: "<<ps<<std::endl;

      int maxr = r_size - r[pr-1].weight_so_far + set_data[m].overlap;
      int maxs = sets_collection[m].first - s[ps-1].weight_so_far + set_data[m].overlap;

      //std::cout<<"maxr: "<<maxr<<" maxs: "<<maxs<<std::endl;
      // overlap needed for threshold tau between r and s
      const double eqoverlap = r_size - distance_threshold;

      //std::cout<<"eqoverlap: "<<eqoverlap<<std::endl;

      // verify if r and s belong to the resultset, computed the structural filter
      if (structural_filter(r, s, eqoverlap, set_data[m].overlap, pr, ps, distance_threshold, maxr, maxs))
        join_candidates.emplace_back(r_id, m);

      // reset overlap in set_data
      set_data[m].overlap = 0;
    }
  }
}

int CandidateIndex::structural_mapping_rn(
      label_feature_set_converter::LabelSetElement& sv_r, 
      label_feature_set_converter::LabelSetElement& sv_s,
      std::vector<std::vector<std::pair<int,double>>>& distance_vect,
      const double distance_threshold){

        std::cout<<"mapping"<<std::endl;

        int tau_valid = 0;
        // check if duplicates exist
        if(sv_s.weight == 1 && sv_r.weight == 1) {
        // no duplicates -> do positional filter
          if(feature_filter(sv_s.struct_vect[0],sv_r.struct_vect[0],distance_threshold)!=-1)
            return 1;
        }
        else{
            std::reference_wrapper<label_feature_set_converter::LabelSetElement> se = std::ref(sv_r);
            std::reference_wrapper<label_feature_set_converter::LabelSetElement> le = std::ref(sv_s);
            if(sv_s.weight < sv_r.weight) {
              se = std::ref(sv_s);
              le = std::ref(sv_r);
            }
            std::vector<std::set<int>> included;
            included.resize(distance_vect.size());
            for(size_t i=0;i<distance_vect.size();i++){
              for(auto pari_set:distance_vect[i]){
                if(pari_set.second<=distance_threshold){
                  included[i].insert(pari_set.first);
                  included[pari_set.first].insert(i);
                }
              }  
            }

            for(size_t i=0;i<included.size();i++){
              std::cout<<"node["<<i<<"]: ";
              for(auto node: included[i]){
                std::cout<<node<<" ";
              } 
              std::cout<<std::endl;
            }
            std::set<int> node_set;
            for(int i=0;i<distance_vect.size();i++)
              node_set.insert(i);
            
            std::set<int> candidate_sets=node_set; 
            std::vector<int> result;

            while(!candidate_sets.empty()){
              while(!node_set.empty()){
                get_representive_node(node_set,result,included);
              }
              for(auto r: result){
                candidate_sets.erase(r);
              }
              node_set=candidate_sets;
            }

            for(auto r: result){
              std::cout<<r<<" ";
            }
            std::cout<<std::endl;

            std::set<int> checked_node;
            for(auto i: result){
              label_feature_set_converter::StructuralVector& left_hand_duplicate = se.get().struct_vect[i];
              int left_side_k_window = std::max(0.0, left_hand_duplicate.postorder_id - distance_threshold);
              for(int j = 0; j < le.get().weight; ++j) {
                label_feature_set_converter::StructuralVector& right_hand_duplicate = le.get().struct_vect[j];
                // postorder id in right hand side duplicates is too large to satisfy the postorder lower bound
                if(right_hand_duplicate.postorder_id > distance_threshold + left_hand_duplicate.postorder_id)
                  break;
                int feature_distacne;
                if(feature_distacne=feature_filter(right_hand_duplicate,left_hand_duplicate,distance_threshold)!=-1){
         
                ++tau_valid;
                ++same_label_compa;
                checked_node.insert(i);
                for(auto p: included[i])
                  checked_node.insert(p);
                ++tau_valid;
                break;
              }           
            }
          }
        }

        return tau_valid;
      }

 void CandidateIndex::get_representive_node(std::set<int>& candidates_node,
    std::vector<int>& result,
    std::vector<std::set<int>>& included){
      
      size_t max_weight=0;
      size_t max_index=0;

    for(auto index:candidates_node){
        //std::cout<<index<<std::endl;
        
        if(included[index].size()>max_weight){
            max_weight=included[index].size();
            max_index=index;
        }
    }
    //std::cout<<"max index"<<max_index<<std::endl;
    result.push_back(max_index);
    candidates_node.erase(max_index);

    for(auto i : included[max_index]){
        candidates_node.erase(i);
    }

 }

  void  CandidateIndex::look_up_rn( 
    std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& join_candidates,
    std::vector<std::vector<std::vector<std::vector<std::pair<int,double>>>>>& distance_collection,
    const int number_of_labels, 
    const double distance_threshold){
       //std::cout<<"number of lable: "<<number_of_labels<<std::endl;
  // inverted list index.
  std::vector<candidate_index::InvertedListElement> il_index(number_of_labels);
  // containing specific data of a set. (e.g. actual overlap, index prefix)
  std::vector<candidate_index::SetData> set_data(sets_collection.size());
  // position in label set while processing r
  std::size_t p = 0;

  // iterate through all sets in the given collection
  std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>::iterator r_it = sets_collection.begin();
  for (; r_it != sets_collection.end(); ++r_it) {
    std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>& r_pair = *r_it; // dereference iterator to current set once
    std::vector<label_feature_set_converter::LabelSetElement> r = r_pair.second; // dereference iterator to current set once
    int r_id = r_it - sets_collection.begin(); // identifier for r (line number)
    std::vector<int> M; // holds the set identifiers of the candidate pairs, 
                                 // the overlap is stored in the set_data
    int r_size = sets_collection[r_id].first; // number of elements in r

    std::cout<<"tree: "<<r_id<<std::endl;

    // *****************************
    // ** Generate pre candidates **
    // *****************************
    // add all small trees that does not have to share a common label in the prefix
    if(r_size <= distance_threshold){
      //std::cout<<"small tree"<<std::endl; 
      for(int i = 0; i < r_id; ++i) {
        if(set_data[i].overlap == 0) 
          M.push_back(i); // if not, add it to the candidate set M
        ++set_data[i].overlap; // increase overlap for set i
      }
    }

    // iterate through probing prefix elements and extend the candidate set
    p = 0;
    // until tau + 1 nodes of the probing set are processed
    //std::cout<<"travse all nodes"<<std::endl;
    while(p < r.size()) {
      // remove all entries in the inverted list index up to the position where 
      // the size is greater than the lower bound
      
      for(std::size_t s = il_index[r[p].id].offset; s < il_index[r[p].id].element_list.size() &&
          sets_collection[il_index[r[p].id].element_list[s].first].first < r_size - distance_threshold; s++){ 
          ++il_index[r[p].id].offset;
        }
        

      // iterate through all remaining sets for the current token r[p] in the 
      // inverted list index and add them to the candidates

      //std::cout<<" finding candidates"<<std::endl;
      for(std::size_t s = il_index[r[p].id].offset; s < il_index[r[p].id].element_list.size(); s++) {
        int set = il_index[r[p].id].element_list[s].first;
        int pos = il_index[r[p].id].element_list[s].second;
        // increase the number of lookups in the inverted list
        ++il_lookups_;
        std::cout<<"current set: label pos-->"<<p<<std::endl;
        std::cout<<"saving set: tree id-->"<<set<<" label pos: "<<pos<<std::endl;
        int tau_valid;
        if(sets_collection[set].second[pos].weight>r[p].weight)
          tau_valid = structural_mapping_rn(r[p], sets_collection[set].second[pos],distance_collection[r_id][p],distance_threshold);
        else
          tau_valid = structural_mapping_rn(r[p], sets_collection[set].second[pos],distance_collection[set][pos],distance_threshold);
        //std::cout<<"tau vaild: "<<tau_valid<<std::endl;
        if(tau_valid != 0 && set_data[set].overlap == 0) 
          M.push_back(set); // if not, add it to the candidate set M
        set_data[set].overlap += tau_valid;
        
      }
      // stop as soon as tau + 1 elements have been discovered
      p++;
      if(r[p-1].weight_so_far > distance_threshold + 1)
        break;
    }

    // store last postition of the index of the label set
    set_data[r_id].prefix = p;
    // count number of precandidates
    pre_candidates_ += M.size();

    //std::cout<<"verifying pre_candidate"<<std::endl;
    // add all elements in the prefix of r in the inverted list
    for(int p = 0; p < set_data[r_id].prefix; p++)
      il_index[r[p].id].element_list.push_back(std::make_pair(r_id, p));


    // *****************************
    // *** Verify pre candidates ***
    // *****************************
    // compute structural filter for each candidate (r, s) in M
    //std::cout<<"**<-- Verify pre candidates -->**"<<std::endl;
    for (int m: M) {

      //std::cout<<"m: "<<m<<std::endl;
      std::vector<label_feature_set_converter::LabelSetElement>& s = sets_collection[m].second;
      // prefix positions for sets r and s in the candidate pair
      std::size_t pr = 0, ps = 0;

      // check last prefix position; the smaller one starts at prefix position, 
      // the greater one starts at the overlap
      if (r[set_data[r_id].prefix-1].id > s[set_data[m].prefix-1].id) {

        //std::cout<<" probing T greater than T' in canidate set"<<std::endl;
        for(; r[pr].weight_so_far < set_data[m].overlap && pr < r.size(); ++pr) {}
        ++pr;
        ps = set_data[m].prefix;
      } else {
        //std::cout<<" probing T  smaller than T' in canidate set"<<std::endl;
        pr = set_data[r_id].prefix;
        for(; s[ps].weight_so_far < set_data[m].overlap && ps < s.size(); ++ps) {}
        ++ps;
      }
      //std::cout<<"pr: "<<pr<<" ps: "<<ps<<std::endl;

      int maxr = r_size - r[pr-1].weight_so_far + set_data[m].overlap;
      int maxs = sets_collection[m].first - s[ps-1].weight_so_far + set_data[m].overlap;

      //std::cout<<"maxr: "<<maxr<<" maxs: "<<maxs<<std::endl;
      // overlap needed for threshold tau between r and s
      const double eqoverlap = r_size - distance_threshold;

      //std::cout<<"eqoverlap: "<<eqoverlap<<std::endl;

      // verify if r and s belong to the resultset, computed the structural filter
      if (structural_filter(r, s, eqoverlap, set_data[m].overlap, pr, ps, distance_threshold, maxr, maxs))
        join_candidates.emplace_back(r_id, m);

      // reset overlap in set_data
      set_data[m].overlap = 0;
    }
  }
}

