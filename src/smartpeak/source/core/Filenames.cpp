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

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <plog/Log.h>

#include <string>
#include <map>
#include <iostream>
#include <string>
#include <regex>

namespace SmartPeak
{


  std::map<std::string, Filenames::Tag> Filenames::string_to_tag_ =
  {
    { "MAIN_DIR", Filenames::Tag::MAIN_DIR },
    { "MZML_INPUT_PATH", Filenames::Tag::MZML_INPUT_PATH },
    { "FEATURES_INPUT_PATH", Filenames::Tag::FEATURES_INPUT_PATH },
    { "FEATURES_OUTPUT_PATH", Filenames::Tag::FEATURES_OUTPUT_PATH },
    { "INPUT_MZML_FILENAME", Filenames::Tag::INPUT_MZML_FILENAME },
    { "INPUT_INJECTION_NAME", Filenames::Tag::INPUT_INJECTION_NAME },
    { "OUTPUT_INJECTION_NAME", Filenames::Tag::OUTPUT_INJECTION_NAME },
    { "INPUT_SEQUENCE_SEGMENT_NAME", Filenames::Tag::INPUT_SEQUENCE_SEGMENT_NAME},
    { "OUTPUT_SEQUENCE_SEGMENT_NAME", Filenames::Tag::OUTPUT_SEQUENCE_SEGMENT_NAME },
    { "INPUT_GROUP_NAME", Filenames::Tag::INPUT_GROUP_NAME },
    { "OUTPUT_GROUP_NAME", Filenames::Tag::OUTPUT_GROUP_NAME }
  };

  void Filenames::addFileName(const std::string& id, 
                              const std::string& name_pattern, 
                              const std::string& description, 
                              bool embeddable,
                              bool default_embedded,
                              bool overwrite)
  {
    if (overwrite || (file_names_.find(id) == file_names_.end()))
    {
      FileName f{ name_pattern, description, embeddable, default_embedded };
      file_names_.insert_or_assign(id, f);
      updateFullPaths();
    }
    else
    {
      // if exits, override description and embeddable flag
      file_names_.at(id).description_ = description;
      file_names_.at(id).embeddable_ = embeddable;
    }
  }

  std::filesystem::path Filenames::getFullPath(const std::string& file_id) const
  {
    if (file_names_.count(file_id))
    {
      return file_names_.at(file_id).full_path_.lexically_normal().generic_string();
    }
    else
    {
      return "";
    }
  }

  std::filesystem::path Filenames::getNamePattern(const std::string& file_id) const
  {
    if (file_names_.count(file_id))
    {
      return file_names_.at(file_id).name_pattern_;
    }
    else
    {
      return "";
    }
  }

  bool Filenames::isEmbeddable(const std::string& file_id) const
  {
    if (file_names_.count(file_id))
    {
      return file_names_.at(file_id).embeddable_;
    }
    else
    {
      return false;
    }
  }

  void Filenames::setFullPath(const std::string& id, const std::filesystem::path& full_path)
  {
    if (file_names_.find(id) == file_names_.end())
    {
      addFileName(id, "");
    }
    file_names_.at(id).full_path_override_ = true;
    file_names_.at(id).full_path_ = full_path;
    file_names_.at(id).name_pattern_ = "";
  }

  void Filenames::updateFullPaths()
  {
    for (auto& filename : file_names_)
    {
      updateFullPath(filename.second);
    }
  }

  void Filenames::updateFullPath(FileName& filename)
  {
    if (!filename.full_path_override_)
    {
      std::string file_pattern = filename.name_pattern_;
      for (const auto& tag : string_to_tag_)
      {
        std::string search = "${" + tag.first + "}";
        std::string replace_with;
        if (string_to_tag_.count(tag.first))
        {
          replace_with = tags_[string_to_tag_[tag.first]];
        }
        file_pattern = Utilities::replaceAll(file_pattern, search, replace_with);
      }
      filename.full_path_ = file_pattern;
    }
  }

  void Filenames::merge(const Filenames& other)
  {
    for (const auto& p : other.file_names_)
      file_names_.emplace(p.first, p.second);
    updateFullPaths();
  }

  std::vector<std::string> Filenames::getFileIds() const
  {
    std::vector<std::string> file_ids;
    for (const auto& key : file_names_)
    {
      file_ids.push_back(key.first);
    }
    return file_ids;
  }

  void Filenames::setTagValue(Tag tag, const std::string& value)
  {
    tags_[tag] = value;
    updateFullPaths();
  }

  std::string Filenames::getTagValue(Tag tag) const
  {
    if (tags_.count(tag))
    {
      return tags_.at(tag);
    }
    else
    {
      return "";
    }
  }

  std::map<std::string, Filenames::Tag> Filenames::getTagNames() const
  {
    return string_to_tag_;
  }

  std::string Filenames::getDescription(const std::string& file_id) const
  {
    return file_names_.at(file_id).description_;
  }

  void Filenames::setEmbedded(const std::string& file_id, bool embedded)
  {
    file_names_.at(file_id).embedded_ = embedded;
  }

  bool Filenames::isEmbedded(const std::string& file_id) const
  {
    if (file_names_.count(file_id))
    {
      return file_names_.at(file_id).embedded_;
    }
    else
    {
      return false;
    }
  }

  void Filenames::log() const
  {
    LOGI << "Input Files Setup:";
    for (const auto& file_id : getFileIds())
    {
      const auto full_path = getFullPath(file_id);
      if (full_path.empty())
      {
        LOGI << file_id << ": Not used";
      }
      else
      {
        LOGI << file_id << ": " << "\"" << full_path.generic_string() << "\"";
      }
    }
  }
}
