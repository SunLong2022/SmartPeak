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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/CastValue.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/EventDispatcher.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/ApplicationProcessors/BuildCommandsFromNames.h>

#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>

#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureSelector.h>
#include <OpenMS/DATASTRUCTURES/Param.h>
#include <OpenMS/FORMAT/MRMFeatureQCFile.h>

#include <regex>
#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <tuple>

#include <plog/Log.h>
#include <openssl/sha.h>

#define maxFunc(a,b) (((a) > (b)) ? (a) : (b))

namespace SmartPeak
{
  class Utilities
  {
public:
    Utilities()                            = delete;
    ~Utilities()                           = delete;
    Utilities(const Utilities&)            = delete;
    Utilities& operator=(const Utilities&) = delete;
    Utilities(Utilities&&)                 = delete;
    Utilities& operator=(Utilities&&)      = delete;

    /**
      Cast a string to the desired type and return the evaluation.

      The type check is case insensitive.

      @note a value "true" or "false" of type "string" will be converted to bool

      @param[in] value input string
      @param[in] type desired type
      @param[out] cast result
    */
    static void castString(
      const std::string& value,
      const std::string& type,
      CastValue& cast
    );

    /**
      Update a OpenMS' DefaultParamHandler object with user parameters.

      @param[in,out] Param_handler_IO OpenMS' DefaultParamHandler object to update
      @param[in] parameters_I user parameters
      @param[in] param_handler_name set if user parameter have different name entry.
    */
    static void setUserParameters(
      OpenMS::DefaultParamHandler& Param_handler_IO,
      const ParameterSet& user_parameters_I,
      const std::string param_handler_name_I = ""
    );

    /**
      Update a Param object.

      The type check is case insensitive.

      @note a value "true" or "false" of type "string" will be converted to bool

      @param[in,out] Param_IO OpenMS' Param object to update
      @param[in] parameters_I List of parameters to update
    */
    static void updateParameters(
      OpenMS::Param& Param_IO,
      const FunctionParameters& parameters_I
    );

    /**
      Convert a OpenMS::DataValue to SmartPeak's CastValue.

      @param[in] openms_datavalue OpenMSDataValue to convert
      @return SmartPeak's CastValue
    */
    static CastValue OpenMSDataValueToCastValue(
      const OpenMS::DataValue& openms_datavalue
    );

    /**
      Parse string and return the eval.

      The type check is case insensitive.

      @note a value "true" or "false" of type "string" will be converted to bool

      @param[in] str_I Input string
      @param[out] cast The evaluated string
    */
    static void parseString(const std::string& str_I, CastValue& cast);

    static void parseList(const std::string& line, std::regex& re, CastValue& cast);

    static std::vector<std::string> splitString(
      const std::string& s,
      const char sep
    );

    static std::string trimString(
      const std::string& s,
      const std::string& whitespaces = " \f\n\r\t\v" // std::isspace()
    );

    template<typename T>
    static std::map<std::string, float> calculateValidationMetrics(
      const std::vector<T>& y_true,
      const std::vector<T>& y_pred
    )
    {
      if (y_true.empty() || y_pred.empty()) {
        LOGD << "Actual size " << y_true.size() << ", predicted size " << y_pred.size();
        return {
          {"accuracy", 0},
          {"recall", 0},
          {"precision", 0},
          {"n_features", 0},
        };
      }

      const std::array<size_t, 4> conf = computeConfusionMatrix(y_true, y_pred);
      const size_t TP = conf[0];
      const size_t FP = conf[1];
      const size_t FN = conf[2];
      const size_t TN = conf[3];
      const float accuracy = (TP + TN) / static_cast<float>(TP + FP + FN + TN);
      const float recall = TP / static_cast<float>(TP + FN);
      const float precision = TP / static_cast<float>(TP + FP);

      return {
        {"accuracy", accuracy},
        {"recall", recall},
        {"precision", precision},
        {"n_features", y_true.size()},
      };
    }

    template<typename T>
    static std::array<size_t, 4> computeConfusionMatrix(
      const std::vector<T>& y_true,
      const std::vector<T>& y_pred
    )
    {
      if (y_true.size() != y_pred.size())
        throw std::invalid_argument("Sizes don't match.");

      // Ref: https://en.wikipedia.org/wiki/Confusion_matrix

      std::array<size_t, 4> conf = {0, 0, 0, 0}; // TP, FP, FN, TN
      size_t& TP = conf[0];
      size_t& FP = conf[1];
      size_t& FN = conf[2];
      size_t& TN = conf[3];
      typename std::vector<T>::const_iterator a = y_true.cbegin(); // a = actual
      typename std::vector<T>::const_iterator p = y_pred.cbegin(); // p = predicted

      for (; a != y_true.cend(); ++a, ++p) {
        if (*p) { // positives
          if (*a)
            ++TP;
          else
            ++FP;
        } else {  // negatives
          if (*a)
            ++FN;
          else
            ++TN;
        }
      }

      LOGD << "Confusion matrix: [TP, FP, FN, TN] = [" << TP << ", " << FP << ", " << FN << ", " << TN << "]";

      return conf;
    }

    /**
      @brief Test absolute and relative closeness of values

      References: http://realtimecollisiondetection.net/blog/?p=89

      @param: lhs Left Hand Side to compare
      @param: rhs Right Hand Side to Compare
      @param: rel_tol Relative Tolerance threshold
      @param: abs_tol Absolute Tolerance threshold

      @returns True or False
    */
    template<typename T>
    static bool assert_close(const T& lhs, const T& rhs, double rel_tol = 1e-4, double abs_tol = 1e-4)
    {
      return (std::fabs(lhs - rhs) <= maxFunc(abs_tol, rel_tol * maxFunc(std::fabs(lhs), std::fabs(rhs)) ));
    }

    static std::vector<OpenMS::MRMFeatureSelector::SelectorParameters> extractSelectorParameters(
      const FunctionParameters& params,
      const FunctionParameters& score_weights
    );

    template<typename Iterator>
    static std::string join(Iterator first, Iterator last, const std::string& delimiter = "")
    {
      std::ostringstream oss;
      if (first != last) {
        oss << *first;
        for (Iterator it = 1 + first; it != last; ++it) {
          oss << delimiter << *it;
        }
      }
      return oss.str();
    }

    // To validate quantitation methods stored in examples' tests
    static bool testStoredQuantitationMethods(const std::string& pathname);

    /**
      @brief Check if str ends with suffix

      Useful for filtering filenames by the extension

      @param[in] str Input string
      @param[in] suffix Suffix
      @param[in] case_sensitive Case sensitive string comparison

      @returns True if str ends with suffix. Otherwise false.
    */
    static bool endsWith(
      std::string str,
      std::string suffix,
      const bool case_sensitive = true
    );
    
    /**
      @brief Retrieves information about files in a given directory

      @param[in] folder_path Given path to inspect and scan
      where the first element is "name", "extension", "size" and "last_write_time", the second element can either be "ascending" or "descending".
      @return List of files found where each string of vectors is a representation of a file's name, size, type and date.
    */
    static std::array<std::vector<std::string>, 4> getFolderContents(const std::filesystem::path& folder_path, bool only_directories);
    
    /**
      @brief Get the parent path from a given path, the given path is returned when the parent path isn't existent

      @param[in] p Path to a directory of interest.
      @param[out] parent_path The parent path of the given path as a standard string.
    */
    static std::string getParentPath(const std::filesystem::path& p);

    /**
      @brief Moves the elements in vector v according to indices

      Example:
      indices: {3, 4, 2, 0, 1}
      old v:   {'a', 'b', 'c', 'd', 'e'}
      new v:   {'d', 'e', 'c', 'a', 'b'}

      @throws std::invalid_argument Sizes of indices and v differ

      @param[in] indices The vector of indices that decides the sorting
      @param[in,out] v The vector of elements to be sorted
    */
    template<typename T>
    static void sortPairs(
      const std::vector<size_t>& indices,
      std::vector<T>& v
    )
    {
      if (indices.size() != v.size()) {
        throw std::invalid_argument("sortPairs: arguments' sizes do not match");
      }
      const std::vector<T> cp(v); // a copy of v
      for (size_t i = 0; i < v.size(); ++i) {
        v[i] = cp[indices[i]];
      }
    }

    /**
      @brief Case-insensitive string comparison

      Equivalent to a.compare(b) < 0, ignoring letter case.

      @param[in] a left-side string
      @param[in] b right-side string

      @returns True if 'a' is lexicographically less than 'b'. Otherwise false.
    */
    static bool is_less_than_icase(const std::string& a, const std::string& b);
    
    /**
      @brief Retrieves basic information on a given directory

      @param[in] p Path to inspect
      @param[out] directory_info A tuple where the first element is the total size of files in the directory,
      and the second is the total entries in the directory (files and directories).
    */
    static void getDirectoryInfo(const std::filesystem::path& p, std::tuple<float, uintmax_t>& directory_info);
    
    /**
      @brief Check whether a path is hidden

      @param[in] entry_path path to inspect
      @returns True if a hidden entry is encountered
    */
    static bool isHiddenEntry(const std::filesystem::path& entry_path);

    /**
    * @brief Constructs an absolute filepath to an application logs.
    * 
    * Default locations of logs:
    *   - Windows: C:\Users\<user>\AppData\Local\SmartPeak
    *   - Linux and MacOS: ~/.SmartPeak
    * User can change default location and specify directory where the logs are stored by
    * setting SMARTPEAK_LOGS env variable. If directory specified by the path doesn't exist, the function will create one.
    * 
    * @param[in] filename Log filename
    * @returns The absolute path to log file and boolean flag whether the path to directory was created
    */
    static std::pair<std::filesystem::path, bool> getLogFilepath(const std::string& filename);
    
    /**
      @brief Get a value fromt the current environment

      @param[in] env_name environment variable name
      @param[out] path variable's value
    */
    static void getEnvVariable(const char *env_name, std::string *path);


    /**
    * @brief Returns the build version of SmartPeak package if available.
    */
    static std::string getSmartPeakVersion();

    /**
     * Converts an input string to upper case string.
     * 
     * @param[in] str input string
     * @return formated string
    */
    static std::string str2upper(const std::string& str);
    
    /**
     @brief Remove trailing characters.
      
     @param[in] to_remove string to remove from given string.
     @param[in,out] str string on which the modification is done.
    */
    static void removeTrailing(std::string& str, std::string to_remove);

    /**
     @brief Construct filename using the default files organisation.
     
     Can be used to create an empty session from a directory.
    */
    static Filenames buildFilenamesFromDirectory(ApplicationHandler& application_handler, const std::filesystem::path& path);
    
    /**
     @brief Returns a unique string based on the current time.
     
     @param[out] a unique string in the format : %Y-%m-%d_%H-%M-%S_TIME_SINCE_EPOCHS
    */
    static std::string makeUniqueStringFromTime();

    /**
     @brief return an empty, temporary directory.
    */
    static std::filesystem::path createEmptyTempDirectory();

    /**
      @brief for parameter that is supposed to be file path, make it full path if it exists.
    */
    static void prepareFileParameter(
      ParameterSet& parameter_set,
      const std::string& function_parameter,
      const std::string& parameter_name,
      const std::filesystem::path main_path);

    /**
     @brief for parameter that is supposed to be file path, make it full path if it exists (list version).
     */
    static void prepareFileParameterList(
      ParameterSet& parameter_set,
      const std::string& function_parameter,
      const std::string& parameter_name,
      const std::filesystem::path main_path);
    
    /**
     @brief Returns current time
     
     @returns Current time in the format : %H-%M-%S_%d-%m-%Y
    */
    static std::string getCurrentTime();
    
    /**
     @brief Calculates sha256 hash on a given string
     
     @param[in] str string to perfrom the sha256 hashing on
     
     @returns hashed string
    */
    static std::string sha256(const std::string str);
    
    /**
     @brief Creates .serversession.ssi file
     
     @param[in] file_path pull path to the location of where to create the .serversession.ssi if not existing
    */
    static void createServerSessionFile(std::filesystem::path file_path);
    
    /**
     @brief Writes comma seperated values to .serversession.ssi file
     
     @param[in] file_path pull path to the location of where to create the .serversession.ssi if not existing
     @param[in] user_id user id as provided in the login dialog
     @param[in] dataset_name the name of the dataset name
     @param[in] workflow_status workflow status if the current/last workflow run
     @param[in] started_at the start time of the workflow
     @param[in] finished_at the end time of the workflow
     @param[in] path_to_exports the full path to the exported reports
     @param[in] log_file full path to the generated log file
    */
    static void writeToServerSessionFile(
      std::filesystem::path file_path,
      std::string usr_id, std::string dataset_name, std::string workflow_status,
      std::string started_at, std::string finished_at, std::string path_to_exports,
      std::string log_file);
    
    /**
     @brief Checks for the last workflow status
     
     @param[in] file_path pull path to the location of the dataset
     @param[in] username user id of the current user
    */
    static bool checkLastServerWorkflowRun(std::filesystem::path file_path, std::string& username);
    
    /**
     @brief Loads processed raw data and features
     
     @param[in] application_handler application handler
     @param[in] session_handler session handler
     @param[in] workflow_manager workflow manager
     @param[in] event_dispatcher event dispatcher
    */
    static void loadRawDataAndFeatures(
      ApplicationHandler& application_handler, SessionHandler& session_handler,
      WorkflowManager& workflow_manager, EventDispatcher& event_dispatcher);

    /**
     @brief returns true if the string is a list of items matching the regex.
     */
    static bool isList(const std::string& str, const std::regex& re);

    /**
     @brief replace all instance of one string with another string.
     @param[in] str the string to look into
     @param[in] search the string to look for
     @param[in] replace the string to replace with
     */
    static std::string replaceAll(const std::string& str, const std::string search, const std::string& replace);

    /**
     @brief Check if a csv file has required headers
    */
    template<char Separator, typename ...Columns>
    static bool checkCSVHeader(const std::filesystem::path& filename, const Columns& ...columns);

    /**
     @brief returns true if file has BOM marker
    */
    static bool hasBOMMarker(const std::filesystem::path& filename);
  };

  template<char Separator, typename ...Columns>
  bool Utilities::checkCSVHeader(const std::filesystem::path& filename, const Columns& ...columns)
  {
    try
    {
      io::CSVReader<sizeof...(Columns), io::trim_chars<>, io::no_quote_escape<Separator>> in(filename.generic_string());
      in.read_header(
        io::ignore_extra_column,
        columns...
      );
    }
    catch (const io::error::missing_column_in_header&)
    {
      return false;
    }
    return true;
  }
}

