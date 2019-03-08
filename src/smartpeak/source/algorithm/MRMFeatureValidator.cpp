// TODO: Add copyright

#include <SmartPeak/algorithm/MRMFeatureValidator.h>
#include <SmartPeak/core/Utilities.h>

namespace SmartPeak
{
  void MRMFeatureValidator::validate_MRMFeatures(
    const std::vector<std::map<std::string, Utilities::CastValue>>& reference_data_v,
    const OpenMS::FeatureMap& features,
    const std::string& injection_name,
    OpenMS::FeatureMap& output_validated,
    std::map<std::string, float>& validation_metrics,
    const float Tr_window
  )
  {
    std::cout << "features " << features.size() << std::endl;
    std::cout << "reference data " << reference_data_v.size() << std::endl;
    std::cout << "injection_name " << injection_name << std::endl;
    std::vector<int> y_true;
    std::vector<int> y_pred;
    output_validated.clear(true);

    std::map<std::string, std::map<std::string, Utilities::CastValue>> reference_data;
    for (const std::map<std::string, Utilities::CastValue>& m : reference_data_v) {
      if (m.at("injection_name").s_ != injection_name) {
        continue;
      }
      const std::string& name = m.at("component_name").s_;
      // std::cout << "FOUND A MATCH" << std::endl;
      reference_data[name] = m;
    }

    for (const OpenMS::Feature& feature : features) {
      std::vector<OpenMS::Feature> subordinates_tmp;
      for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
        
        bool fc_pass = false;
        if (!subordinate.metaValueExists("native_id")) {
          throw "native_id info is missing.";
        }
        const std::string reference_data_key = subordinate.getMetaValue("native_id").toString();
        // std::cout << "subordinate " << reference_data_key << std::endl;
        OpenMS::Feature subordinate_copy = subordinate;

        if (0 == reference_data.count(reference_data_key)) {
          // std::cout << " reference data count os 0 " << std::endl;
          subordinate_copy.setMetaValue("validation", "ND");
          subordinates_tmp.push_back(subordinate_copy);
          continue;
        }

        // extract and format rt information
        const float reference_rt = reference_data[reference_data_key]["retention_time"].f_;

        if (0.0 == reference_rt) {
          continue;
        }
        const float feature_rt = feature.getRT();
        const float feature_leftWidth = static_cast<float>(feature.getMetaValue("leftWidth"));
        const float feature_rightWidth = static_cast<float>(feature.getMetaValue("rightWidth"));
        // validate the retention time
        if (std::fabs(reference_rt - feature_rt) < Tr_window ||
            (reference_rt > feature_leftWidth &&
             reference_rt < feature_rightWidth)) {
          fc_pass = true;
        }
        // other validation parameters
        // NOTE: if there are no other validation parameters that are
        // transition-specific, there is no need to loop
        // through each transition
        if (fc_pass) { // True Positive
          subordinate_copy.setMetaValue("validation", "TP");
          subordinates_tmp.push_back(subordinate_copy);
          y_pred.push_back(1);
          y_true.push_back(1);
        } else {       // False Positive
          subordinate_copy.setMetaValue("validation", "FP");
          subordinates_tmp.push_back(subordinate_copy);
          y_pred.push_back(1);
          y_true.push_back(0);
        }
      }
      // check that subordinates were found
      if (subordinates_tmp.empty()) {
        continue;
      }
      // copy out all feature values
      OpenMS::Feature feature_tmp = feature;
      feature_tmp.setSubordinates(subordinates_tmp);
      output_validated.push_back(feature_tmp);
    }

<<<<<<< HEAD
    validation_metrics = Utilities::calculateValidationMetrics(y_true, y_pred);
=======
    if (y_true.size() == 0 || y_pred.size() == 0) {
      std::cout << "NO DATA" << std::endl;
      return;
    }
    validation_metrics = Utilities::calculateValidationMetrics(y_true, y_pred, verbose_I);
>>>>>>> Validation workflow
  }
}
