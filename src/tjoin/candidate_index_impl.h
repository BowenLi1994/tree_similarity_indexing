

#pragma once
using namespace candidate_index;

CandidateIndex::CandidateIndex() {
  pre_candidates_ = 0;
  il_lookups_ = 0;
  same_label_comparation=0;
}

void CandidateIndex::lookup(
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
    std::vector<std::pair<int, int>>& join_candidates,
    const int number_of_labels, 
    const double distance_threshold) {

  //std::cout<<"looking up: "<<std::endl;
  // inverted list index.
  std::vector<candidate_index::InvertedListElement> il_index(number_of_labels);
  // containing specific data of a set. (e.g. actual overlap, index prefix)
  std::vector<candidate_index::SetData> set_data(sets_collection.size());
  // position in label set while processing r
  std::size_t p = 0;

  // iterate through all sets in the given collection
  std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>::iterator r_it = sets_collection.begin();
  for (; r_it != sets_collection.end(); ++r_it) {
    std::pair<int, std::vector<label_set_converter::LabelSetElement>>& r_pair = *r_it; // dereference iterator to current set once
    std::vector<label_set_converter::LabelSetElement> r = r_pair.second; // dereference iterator to current set once
    int r_id = r_it - sets_collection.begin(); // identifier for r (line number)
    std::vector<int> M; // holds the set identifiers of the candidate pairs, 
                                 // the overlap is stored in the set_data
    int r_size = sets_collection[r_id].first; // number of elements in r


    // std::cout<<"**<------------------ Generate pre candidates ------------------>**"<<std::endl;
    // std::cout<<"r id: "<<r_id<<" r size: "<<r_size<<std::endl;


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
    //std::cout<<"probing the tau +1 nodes"<<std::endl;
    while(p < r.size()) {
      // remove all entries in the inverted list index up to the position where 
      // the size is greater than the lower bound
      
      //bool flag=false;
      //std::cout<<"label id: "<<r[p].id<<std::endl;
      for(std::size_t s = il_index[r[p].id].offset; s < il_index[r[p].id].element_list.size() &&
          sets_collection[il_index[r[p].id].element_list[s].first].first < r_size - distance_threshold; s++){
      
          ++il_index[r[p].id].offset;
        }
        
      // std::cout<<"iverted list: updating offset"<<std::endl;
      // for(size_t i=0; i<il_index.size();i++){
      //   std::cout<<"il["<<i<<"]: offset: "<<il_index[i].offset<<std::endl;    
      // }

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

    // std::cout<<"set data: "<<std::endl;
    // for(size_t i=0;i<set_data.size();i++){
    //   std::cout<<"set["<<i<<"] : overlap: "<<set_data[i].overlap<<" prefix: "<<set_data[i].prefix<<std::endl;
    // }
    
    // add all elements in the prefix of r in the inverted list
    for(int p = 0; p < set_data[r_id].prefix; p++)
      il_index[r[p].id].element_list.push_back(std::make_pair(r_id, p));


    // std::cout<<"updated iverted list: "<<std::endl;
    //   for(size_t i=0; i<il_index.size();i++){
    //     std::cout<<"il["<<i<<"]: offset: "<<il_index[i].offset<<std::endl;
    //     std::cout<<"elment list: "<<std::endl;
    //     for(size_t l=0;l<il_index[i].element_list.size();l++){
    //       std::cout<<"pair: "<<il_index[i].element_list[l].first<<" "<<il_index[i].element_list[l].second<<std::endl;
    //     }
    //   }


    // *****************************
    // *** Verify pre candidates ***
    // *****************************
    // compute structural filter for each candidate (r, s) in M
    //std::cout<<"**<-- Verify pre candidates -->**"<<std::endl;
    for (int m: M) {

      //std::cout<<"m: "<<m<<std::endl;
      std::vector<label_set_converter::LabelSetElement>& s = sets_collection[m].second;
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
    std::vector<label_set_converter::LabelSetElement>& r, 
    std::vector<label_set_converter::LabelSetElement>& s, 
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
    label_set_converter::LabelSetElement& sv_r, 
    label_set_converter::LabelSetElement& sv_s,
    const double distance_threshold) {
  
  //std::cout<<"strucural mapping"<<std::endl;

  // std::cout<<" set in tree T: "<<sv_r.id<<std::endl;
  // std::cout<<" set in tree T': "<<sv_s.id<<std::endl;
  int tau_valid = 0;
  // check if duplicates exist
  if(sv_s.weight == 1 && sv_r.weight == 1) {

    //std::cout<<"T and T's only has one such label"<<std::endl;
    // no duplicates -> do positional filter
    if(abs(sv_s.struct_vect[0].number_nodes_left - sv_r.struct_vect[0].number_nodes_left) + 
       abs(sv_s.struct_vect[0].number_nodes_right - sv_r.struct_vect[0].number_nodes_right) +
       abs(sv_s.struct_vect[0].number_nodes_ancestor - sv_r.struct_vect[0].number_nodes_ancestor) +
       abs(sv_s.struct_vect[0].number_nodes_descendant - sv_r.struct_vect[0].number_nodes_descendant) <= distance_threshold) {
      return 1; // one tau-valid node pair
    }
  } else {

    //std::cout<<"neighter of them has mutilple labels"<<std::endl;
    std::reference_wrapper<label_set_converter::LabelSetElement> se = std::ref(sv_r);
    std::reference_wrapper<label_set_converter::LabelSetElement> le = std::ref(sv_s);
    if(sv_s.weight < sv_r.weight) {
      se = std::ref(sv_s);
      le = std::ref(sv_r);
    }
    // std::cout<<"se weight: "<<se.get().weight<<std::endl;
    // std::cout<<"le weight: "<<le.get().weight<<std::endl;
    std::size_t pid_lower_bound_start = 0;
    for(int i = 0; i < se.get().weight; ++i) {
      label_set_converter::StructuralVector& left_hand_duplicate = se.get().struct_vect[i];
      int left_side_k_window = std::max(0.0, left_hand_duplicate.postorder_id - distance_threshold);
      //std::cout<<"left side k window: "<<left_side_k_window<<std::endl;
      // skip duplicates at the beginning that doesn't satisfy the postorder lower bound
      while(pid_lower_bound_start < le.get().struct_vect.size()
          && le.get().struct_vect[pid_lower_bound_start].postorder_id < left_side_k_window) {
        ++pid_lower_bound_start;
      }
      //std::cout<<"pid_lower_bound_start: "<<pid_lower_bound_start<<std::endl;
      if(pid_lower_bound_start == le.get().struct_vect.size())
        break;

      for(int j = pid_lower_bound_start; j < le.get().weight; ++j) {
        label_set_converter::StructuralVector& right_hand_duplicate = le.get().struct_vect[j];
        // postorder id in right hand side duplicates is too large to satisfy the postorder lower bound
        if(right_hand_duplicate.postorder_id > distance_threshold + left_hand_duplicate.postorder_id)
          break;
        if(abs(right_hand_duplicate.number_nodes_left - left_hand_duplicate.number_nodes_left) + 
           abs(right_hand_duplicate.number_nodes_right - left_hand_duplicate.number_nodes_right) +
           abs(right_hand_duplicate.number_nodes_ancestor - left_hand_duplicate.number_nodes_ancestor) +
           abs(right_hand_duplicate.number_nodes_descendant - left_hand_duplicate.number_nodes_descendant) <= distance_threshold) {
          ++same_label_comparation;
          ++tau_valid;
          break;
        }
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
