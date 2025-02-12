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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>

namespace SmartPeak
{
  class SelectDilutionsParser
  {
  public:
    SelectDilutionsParser() = delete;
    ~SelectDilutionsParser() = delete;
    SelectDilutionsParser(const SelectDilutionsParser&) = delete;
    SelectDilutionsParser& operator=(const SelectDilutionsParser&) = delete;
    SelectDilutionsParser(SelectDilutionsParser&&) = delete;
    SelectDilutionsParser& operator=(SelectDilutionsParser&&) = delete;

    /**
      Parse parameters from csv file.

      @param[in] filename Input csv file
      @param[out] dilution_map
    */
    static void read(
      const std::string& filename,
      std::map<std::string, int>& dilution_map
    );
  };
}
