// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/core/SampleGroupProcessors/LoadFeaturesSampleGroup.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <OpenMS/FORMAT/FeatureXMLFile.h>

namespace SmartPeak
{

  std::set<std::string> LoadFeaturesSampleGroup::getOutputs() const
  {
    return { "Sample Group Features" };
  }
  
  ParameterSet LoadFeaturesSampleGroup::getParameterSchema() const
  {
    return ParameterSet();
  }

  void LoadFeaturesSampleGroup::getFilenames(Filenames& filenames) const
  {
    filenames.addFileName("featureXMLSampleGroup_i", "${FEATURES_INPUT_PATH}/${INPUT_GROUP_NAME}.featureXML");
  };

  void LoadFeaturesSampleGroup::doProcess(SampleGroupHandler& sampleGroupHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const ParameterSet& params_I,
    Filenames& filenames_I
  ) const
  {
    getFilenames(filenames_I);

    if (!InputDataValidation::prepareToLoad(filenames_I, "featureXMLSampleGroup_i", false))
    {
      throw std::invalid_argument("Failed to load input file");
    }

    try {
      OpenMS::FeatureXMLFile featurexml;
      featurexml.load(filenames_I.getFullPath("featureXMLSampleGroup_i").generic_string(), sampleGroupHandler_IO.getFeatureMap());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sampleGroupHandler_IO.getFeatureMap().clear();
      LOGE << "feature map clear";
      throw;
    }
  }

}
