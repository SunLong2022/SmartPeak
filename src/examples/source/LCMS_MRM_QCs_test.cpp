// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/LCMS_MRM_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatures.h>

using namespace SmartPeak;
using namespace std;

void test_main_LCMS_MRM_QCs()
{
  ApplicationHandler application_handler;
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs");
  Filenames filenames_main = Utilities::buildFilenamesFromDirectory(application_handler, main_dir);
  filenames_main.setTagValue(Filenames::Tag::MAIN_DIR, main_dir);
  filenames_main.setTagValue(Filenames::Tag::MZML_INPUT_PATH, main_dir + "/mzML/");
  filenames_main.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, main_dir + "/features/");
  filenames_main.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, main_dir + "/features/");

  example_LCMS_MRM_Unknowns(main_dir, filenames_main, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  Filenames filenames;

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1_1_BatchName_1900-01-01_000000.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("LCMS_MRM_QCs/features/150601_0_BloodProject01_PLT_QC_Broth-1_test.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  assert(fm1.size() == fm2.size());
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());

  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  f1 = &fm1[15].getSubordinates()[0];
  f2 = &fm2[15].getSubordinates()[0];
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
}

int main()
{
  test_main_LCMS_MRM_QCs();
  return 0;
}
