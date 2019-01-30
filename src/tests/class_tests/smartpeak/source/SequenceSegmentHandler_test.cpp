// TODO: Add copyright

#define BOOST_TEST_MODULE SequenceSegmentHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SequenceSegmentHandler.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequencesegmenthandler)

BOOST_AUTO_TEST_CASE(constructor)
{
  SequenceSegmentHandler* ptr = nullptr;
  SequenceSegmentHandler* nullPointer = nullptr;
  ptr = new SequenceSegmentHandler();
  BOOST_CHECK_NE(ptr, nullPointer);
}

BOOST_AUTO_TEST_CASE(destructor)
{
  SequenceSegmentHandler* ptr = nullptr;
  ptr = new SequenceSegmentHandler();
  delete ptr;
}

BOOST_AUTO_TEST_CASE(set_get_SequenceSegmentName)
{
  SequenceSegmentHandler ssh;
  const string foo {"foo"};

  ssh.setSequenceSegmentName(foo);

  const string& name1 = ssh.getSequenceSegmentName(); // testing const getter
  BOOST_CHECK_EQUAL(name1, foo);

  const string bar {"bar"};
  ssh.getSequenceSegmentName() = bar;
  const string& name2 = ssh.getSequenceSegmentName(); // testing non-const getter
  BOOST_CHECK_EQUAL(name2, bar);
}

BOOST_AUTO_TEST_CASE(set_get_SampleIndices)
{
  SequenceSegmentHandler ssh;
  vector<size_t> si1 {1, 3, 5};

  ssh.setSampleIndices(si1);

  const vector<size_t>& si2 = ssh.getSampleIndices(); // testing const getter
  BOOST_CHECK_EQUAL(si2.size(), 3);
  BOOST_CHECK_EQUAL(si2[0], 1);
  BOOST_CHECK_EQUAL(si2[1], 3);
  BOOST_CHECK_EQUAL(si2[2], 5);

  ssh.getSampleIndices()[1] = 2;

  const vector<size_t>& si3 = ssh.getSampleIndices(); // testing non-const getter
  BOOST_CHECK_EQUAL(si3.size(), 3);
  BOOST_CHECK_EQUAL(si3[1], 2);
}

BOOST_AUTO_TEST_CASE(set_get_StandardsConcentrations)
{
  SequenceSegmentHandler ssh;
  OpenMS::AbsoluteQuantitationStandards::runConcentration rc;
  const string foo {"foo"};
  rc.sample_name = foo;
  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> runs1;
  runs1.push_back(rc);

  ssh.setStandardsConcentrations(runs1);

  const vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& runs2 = ssh.getStandardsConcentrations();
  BOOST_CHECK_EQUAL(runs2.size(), 1);
  BOOST_CHECK_EQUAL(runs2[0].sample_name, foo);

  const string bar {"bar"};
  ssh.getStandardsConcentrations()[0].component_name = bar;
  const vector<OpenMS::AbsoluteQuantitationStandards::runConcentration>& runs3 = ssh.getStandardsConcentrations();
  BOOST_CHECK_EQUAL(runs3.size(), 1);
  BOOST_CHECK_EQUAL(runs3[0].sample_name, foo);
  BOOST_CHECK_EQUAL(runs3[0].component_name, bar);
}

BOOST_AUTO_TEST_CASE(set_get_QuantitationMethods)
{
  SequenceSegmentHandler ssh;
  OpenMS::AbsoluteQuantitationMethod qm;
  const string foo {"foo"};
  qm.setComponentName(foo);
  vector<OpenMS::AbsoluteQuantitationMethod> qms1;
  qms1.push_back(qm);

  ssh.setQuantitationMethods(qms1);

  const vector<OpenMS::AbsoluteQuantitationMethod>& qms2 = ssh.getQuantitationMethods();
  BOOST_CHECK_EQUAL(qms2.size(), 1);
  BOOST_CHECK_EQUAL(qms2[0].getComponentName(), foo);

  const string bar {"bar"};
  ssh.getQuantitationMethods()[0].setFeatureName(bar);
  const vector<OpenMS::AbsoluteQuantitationMethod>& qms3 = ssh.getQuantitationMethods();
  BOOST_CHECK_EQUAL(qms3.size(), 1);
  BOOST_CHECK_EQUAL(qms3[0].getComponentName(), foo);
  BOOST_CHECK_EQUAL(qms3[0].getFeatureName(), bar);
}

BOOST_AUTO_TEST_CASE(set_get_ComponentsToConcentrations)
{
  SequenceSegmentHandler ssh;
  OpenMS::AbsoluteQuantitationStandards::featureConcentration fc;
  const string foo {"foo"};
  const double ac1 {4.5};
  fc.actual_concentration = ac1;
  vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> fc1;
  fc1.push_back(fc);
  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> m1;
  m1.insert({foo, fc1});

  ssh.setComponentsToConcentrations(m1);

  const map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    m2 = ssh.getComponentsToConcentrations();
  BOOST_CHECK_EQUAL(m2.size(), 1);
  BOOST_CHECK_EQUAL(m2.count(foo), 1);
  BOOST_CHECK_EQUAL(m2.at(foo)[0].actual_concentration, ac1);

  const double ac2 {7.3};
  fc1[0].IS_actual_concentration = ac2;
  ssh.getComponentsToConcentrations().erase(foo);
  ssh.getComponentsToConcentrations().insert({foo, fc1});

  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>>&
    m3 = ssh.getComponentsToConcentrations();
  BOOST_CHECK_EQUAL(m3.size(), 1);
  BOOST_CHECK_EQUAL(m3.count(foo), 1);
  BOOST_CHECK_EQUAL(m3.at(foo)[0].actual_concentration, ac1);
  BOOST_CHECK_EQUAL(m3.at(foo)[0].IS_actual_concentration, ac2);
}

BOOST_AUTO_TEST_CASE(clear)
{
  SequenceSegmentHandler ssh;

  ssh.setSequenceSegmentName("foo");

  vector<size_t> si1 {1, 3, 5};
  ssh.setSampleIndices(si1);

  vector<OpenMS::AbsoluteQuantitationStandards::runConcentration> runs1(1);
  ssh.setStandardsConcentrations(runs1);

  vector<OpenMS::AbsoluteQuantitationMethod> qms1(1);
  ssh.setQuantitationMethods(qms1);

  vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> fc1(1);
  map<string, vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> m1;
  m1.insert({"foo", fc1});
  ssh.setComponentsToConcentrations(m1);

  BOOST_CHECK_EQUAL(ssh.getSequenceSegmentName().empty(), false);
  BOOST_CHECK_EQUAL(ssh.getSampleIndices().empty(), false);
  BOOST_CHECK_EQUAL(ssh.getStandardsConcentrations().empty(), false);
  BOOST_CHECK_EQUAL(ssh.getQuantitationMethods().empty(), false);
  BOOST_CHECK_EQUAL(ssh.getComponentsToConcentrations().empty(), false);

  ssh.clear();

  BOOST_CHECK_EQUAL(ssh.getSequenceSegmentName().empty(), true);
  BOOST_CHECK_EQUAL(ssh.getSampleIndices().empty(), true);
  BOOST_CHECK_EQUAL(ssh.getStandardsConcentrations().empty(), true);
  BOOST_CHECK_EQUAL(ssh.getQuantitationMethods().empty(), true);
  BOOST_CHECK_EQUAL(ssh.getComponentsToConcentrations().empty(), true);
}

BOOST_AUTO_TEST_SUITE_END()
