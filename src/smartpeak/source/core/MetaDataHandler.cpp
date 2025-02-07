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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <iostream>
#include <plog/Log.h>

// required for strptime on Windows
#ifdef _WIN32
#include <time.h>
#include <iomanip>
#include <sstream>
#endif

namespace SmartPeak
{
  void MetaDataHandler::setSampleName(const std::string& sample_name_I)
  {
    sample_name = sample_name_I;
  }

  std::string& MetaDataHandler::getSampleName()
  {
    return sample_name;
  }

  const std::string& MetaDataHandler::getSampleName() const
  {
    return sample_name;
  }

  void MetaDataHandler::setSampleGroupName(const std::string& sample_group_name_I)
  {
    sample_group_name = sample_group_name_I;
  }

  std::string& MetaDataHandler::getSampleGroupName()
  {
    return sample_group_name;
  }

  const std::string& MetaDataHandler::getSampleGroupName() const
  {
    return sample_group_name;
  }

  void MetaDataHandler::setSequenceSegmentName(const std::string& sequence_segment_name_I)
  {
    sequence_segment_name = sequence_segment_name_I;
  }

  std::string& MetaDataHandler::getSequenceSegmentName()
  {
    return sequence_segment_name;
  }

  const std::string& MetaDataHandler::getSequenceSegmentName() const
  {
    return sequence_segment_name;
  }

  void MetaDataHandler::setReplicateGroupName(const std::string& replicate_group_name_I)
  {
    replicate_group_name = replicate_group_name_I;
  }

  std::string& MetaDataHandler::getReplicateGroupName()
  {
    return replicate_group_name;
  }

  const std::string& MetaDataHandler::getReplicateGroupName() const
  {
    return replicate_group_name;
  }

  void MetaDataHandler::setFilename(const std::string& filename_I)
  {
    original_filename = filename_I;
  }

  std::string& MetaDataHandler::getFilename()
  {
    return original_filename;
  }

  const std::string& MetaDataHandler::getFilename() const
  {
    return original_filename;
  }

  void MetaDataHandler::setSampleType(SampleType sample_type_I)
  {
    sample_type = sample_type_I;
  }

  SampleType& MetaDataHandler::getSampleType()
  {
    return sample_type;
  }

  const SampleType& MetaDataHandler::getSampleType() const
  {
    return sample_type;
  }

  const std::string MetaDataHandler::getSampleTypeAsString() const
  {
    return sampleTypeToString.at(sample_type);
  }

  bool MetaDataHandler::validateMetaData(const MetaDataHandler& meta_data)
  {
    // std::vector<std::string> sample_types {"Unknown", "Standard", "QC", "Blank", "Double Blank", "Solvent"};

    bool is_valid { true };

    if (meta_data.getSampleName().empty()) {
      LOGE << "sample_name must be specified";
      is_valid = false;
    }

    if (meta_data.getSampleGroupName().empty()) {
      LOGE << "sample_group_name must be specified";
      is_valid = false;
    }

    if (meta_data.getSequenceSegmentName().empty()) {
      LOGE << "sequence_segment_name must be specified";
      is_valid = false;
    }

    if (meta_data.getFilename().empty()) {
      LOGE << "original_filename must be specified";
      is_valid = false;
    }

    if (meta_data.getSampleType() == SampleType::Unrecognized) {
      LOGE << "sample type could not be recognized";
      is_valid = false;
    }

    if (meta_data.acq_method_name.empty()) {
      LOGE << "acq_method_name must be specified";
      is_valid = false;
    }

    if (meta_data.inj_volume <= 0.0) {
      LOGE << "no value for inj_volume";
      is_valid = false;
    }

    if (meta_data.inj_volume_units.empty()) {
      LOGE << "inj_volume_units must be specified";
      is_valid = false;
    }

    if (meta_data.batch_name.empty()) {
      LOGE << "batch_name must be specified";
      is_valid = false;
    }

    return is_valid;
  }

  void MetaDataHandler::clear()
  {
    sample_name.clear();
    sample_group_name.clear();
    sequence_segment_name.clear();
    replicate_group_name.clear();
    original_filename.clear();
    sample_type = SampleType::Unknown;
    acq_method_name.clear();
    inj_volume = -1.0;
    inj_volume_units.clear();
    batch_name.clear();

    // optional
    rack_number = -1;
    plate_number = -1;
    pos_number = -1;
    inj_number = -1;
    dilution_factor = 1.0;
    instrument.clear();
    operator_name.clear();
    proc_method_name.clear();
    acquisition_date_and_time = { 0, 0, 0, 1, 0, 0, 0, 0, 0 };
    scan_polarity = "Unknown";
    scan_mass_low = 0.0;
    scan_mass_high = 1e12;
  }

  std::string MetaDataHandler::getInjectionName() const
  {
    std::string name(sample_name + "_" + std::to_string(inj_number) + "_" + batch_name + "_");
    name.append(getAcquisitionDateAndTimeAsString());
    return name;
  }

#ifdef _WIN32
  // https://stackoverflow.com/questions/321849/strptime-equivalent-on-windows
  char* strptime(const char* s, const char* f, std::tm* tm) {
    // Isn't the C++ standard lib nice? std::get_time is defined such that its
    // format parameters are the exact same as strptime. Of course, we have to
    // create a string stream first, and imbue it with the current C locale, and
    // we also have to make sure we return the right things if it fails, or
    // if it succeeds, but this is still far simpler an implementation than any
    // of the versions in any of the C standard libraries.
    std::istringstream input(s);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(tm, f);
    if (input.fail()) {
      return nullptr;
    }
    return (char*)(s + input.tellg());
  }
#endif

  void MetaDataHandler::setAcquisitionDateAndTimeFromString(
    const std::string& acquisition_datetime,
    const std::string& format
  )
  {
    std::tm tm = { 0, 0, 0, 1, 0, 0, 0, 0, 0 };
    if (strptime(acquisition_datetime.data(), format.data(), &tm) == NULL) {
      throw "Could not convert string to date time object.";
    }
    acquisition_date_and_time = tm;
  }

  std::string MetaDataHandler::getAcquisitionDateAndTimeAsString(const std::string& format) const
  {
    char time_repr[64];
    // ISO 8601 date format, without colons (for Windows filesystems compatibility)
    // i.e. "2019-01-23_164055"
    const size_t bytes_written = strftime(time_repr, 64, format.data(), &acquisition_date_and_time);
    if (bytes_written != 17) {
      throw "Unexpected number of characters written into the array.";
    }
    std::string acquisition_datetime(time_repr);
    return acquisition_datetime;
  }

  std::string MetaDataHandler::getRackNumberAsString(std::optional<std::string> undefined_str) const
  {
    return ((rack_number != -1) || (!undefined_str)) ? std::to_string(rack_number) : *undefined_str;
  }

  std::string MetaDataHandler::getPlateNumberAsString(std::optional<std::string> undefined_str) const
  {
    return ((plate_number != -1) || (!undefined_str)) ? std::to_string(plate_number) : *undefined_str;
  }

  std::string MetaDataHandler::getPosNumberAsString(std::optional<std::string> undefined_str) const
  {
    return ((pos_number != -1) || (!undefined_str)) ? std::to_string(pos_number) : *undefined_str;
  }

  std::string MetaDataHandler::getInjectionNumberAsString(std::optional<std::string> undefined_str) const
  {
    return ((inj_number != -1) || (!undefined_str)) ? std::to_string(inj_number) : *undefined_str;
  }

  std::string MetaDataHandler::getScanMassLowAsString(std::optional<std::string> undefined_str) const
  {
    return ((scan_mass_low != 0.0f) || (!undefined_str)) ? std::to_string(scan_mass_low) : *undefined_str;
  }

  std::string MetaDataHandler::getScanMassHighAsString(std::optional<std::string> undefined_str) const
  {
    return ((scan_mass_high != 1e12f) || (!undefined_str)) ? std::to_string(scan_mass_high) : *undefined_str;
  }
}
