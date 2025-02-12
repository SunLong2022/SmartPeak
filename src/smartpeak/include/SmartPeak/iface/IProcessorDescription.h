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
// $Maintainer: Douglas McCloskey, Krzysztof Abram $
// $Authors: Douglas McCloskey, Krzysztof Abram $
// --------------------------------------------------------------------------

#pragma once
#include <memory>

namespace SmartPeak 
{
  struct IProcessorDescription 
  {
    /**
      Get the processor struct name
    */
    virtual std::string getName() const = 0;

    /**
      Get the processor struct description
    */
    virtual std::string getDescription() const = 0;

    /**
      Get the Schema (possible, default parameters) of the processor.
    */
    virtual ParameterSet getParameterSchema() const = 0;

    /**
      Returns the list of file ids that has to be defined prior to be process.
    */
    virtual std::vector<std::string> getRequirements() const = 0;

    /**
      Returns the list of data type accepted as input.
    */
    virtual std::set<std::string> getInputs() const = 0;

    /**
      Returns the list of data type produced as output.
    */
    virtual std::set<std::string> getOutputs() const = 0;

    virtual ~IProcessorDescription() = default;
  };
}