using namespace histogram;

void Histogram_distance::histogtam_distance_indexing(
        std::vector<std::pair<int, std::vector<label_feature_set_converter::LabelSetElement>>>& sets_collection,
        std::string file_name){

            feature_candidate_index::CandidateIndex fc_can;

            std::ofstream label_outfile("/home/bowen/dataset/distance/"+file_name+".label_distance");
            std::ofstream degree_outfile("/home/bowen/dataset/distance/"+file_name+".degree_distance");
            
            for(auto set: sets_collection){
                
                std::string label_set_string,degree_set_string;

                for(auto label_set : set.second){
                    std::string label_histogram_string="[";
                    std::string degree_histogram_string="[";

                    // std::vector<std::vector<std::pair<int,double>>> label_distance_vect;
                    // label_distance_vect.reserve(label_set.struct_vect.size());
                    // std::vector<std::vector<std::pair<int,double>>> degree_distance_vect;
                    // degree_distance_vect.reserve(label_set.struct_vect.size());

                    for(int i=0;i<label_set.struct_vect.size()-1;i++){
                        for(int j=i+1;j<label_set.struct_vect.size();j++){
                            
                            std::string label_pari_string="(";
                            std::string degree_pair_string="(";
                            int degree_dist=0;
                            for(int m=0;m<4;m++){
                                degree_dist+=fc_can.histogram_distance_dist(
                                label_set.struct_vect[i].degree_histogram[m],
                                label_set.struct_vect[j].degree_histogram[m]);
                            }
                            //degree_distance_vect[i].emplace_back(j,(double)degree_dist/3.0);

                            int label_dist=0;
                            for(int m=0;m<4;m++){
                               label_dist+=fc_can.histogram_distance_dist_2(
                                   label_set.struct_vect[i].label_histogram[m],
                                   label_set.struct_vect[j].label_histogram[m]
                               );
                            }
                            //label_distance_vect[i].emplace_back(j,(double)label_dist/2.0);

                            label_pari_string+=std::to_string(i)+","+std::to_string(j)+","+std::to_string((double)label_dist/2.0)+")";
                            degree_pair_string+=std::to_string(i)+","+std::to_string(j)+","+std::to_string((double)degree_dist/3.0)+")";

                            label_histogram_string+=label_pari_string;
                            degree_histogram_string+=degree_pair_string;
                        }
                    }

                    label_histogram_string+="]";
                    degree_histogram_string+="]";

                    label_set_string+=label_histogram_string;
                    degree_set_string+=degree_histogram_string;

                }
                
                label_outfile<<label_set_string<<std::endl;
                degree_outfile<<degree_set_string<<std::endl;
                //  std::cout<<"label: "<<label_set_string<<std::endl;
                //  std::cout<<"dgree: "<<degree_set_string<<std::endl;
            }
           

        }