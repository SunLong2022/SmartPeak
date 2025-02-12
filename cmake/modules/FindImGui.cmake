#.rst:
# Find ImGui
# -------------
#
# Finds the ImGui library. This module defines:
#
#  ImGui_FOUND                - True if ImGui is found
#  ImGui::ImGui               - ImGui interface target
#  ImGui::Examples            - ImGui example target
#  ImGui::Sources             - ImGui source target for core functionality
#  ImGui::SourcesMiscCpp      - ImGui source target for misc/cpp
#
# Additionally these variables are defined for internal usage:
#
#  ImGui_INCLUDE_DIR          - Include dir 
#  ImGui_INCLUDE_DIR          - Include dir for example helpers
#
# The find module first tries to find ``imgui`` via a CMake config file (which
# is distributed this way via Vcpkg, for example). If that's found, the
# ``ImGui::ImGui`` target is an alias to it and the ``ImGui::Sources`` target
# is empty except for having ``ImGui::ImGui`` as a dependency.
#
# If ``imgui`` is not found, as a fallback it tries to find the C++ sources.
# You can supply their location via an ``IMGUI_DIR`` variable. Once found, the
# ``ImGui::ImGui`` target contains just the header file, while
# ``ImGui::Sources`` contains the source files in ``INTERFACE_SOURCES``.
#
# The ``ImGui::SourcesMiscCpp`` component, if requested, is always searched for
# in the form of C++ sources. Vcpkg doesn't distribute these.
#
# The desired usage that covers both cases is to link ``ImGui::Sources``
# ``PRIVATE``\ ly to a *single* target, which will then contain either the
# sources or be linked to the imgui library from Vcpkg; and linking
# ``ImGui::ImGui`` to this target ``PUBLIC``\ ly.
#

#
#   This file is part of Magnum.
#
#   Copyright 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
#             Vladimir Vondrus <mosra@centrum.cz>
#   Copyright 2018 Jonathan Hale <squareys@googlemail.com>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

# Vcpkg distributes imgui as a library with a config file, so try that first --
# but only if IMGUI_DIR wasn't explicitly passed, in which case we'll look
# there instead
find_package(imgui CONFIG QUIET)
if(imgui_FOUND AND NOT IMGUI_DIR)
    if(NOT TARGET ImGui::ImGui)
        add_library(ImGui::ImGui INTERFACE IMPORTED)
        set_property(TARGET ImGui::ImGui APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES imgui::imgui)

        # Retrieve include directory for FindPackageHandleStandardArgs later
        get_target_property(ImGui_INCLUDE_DIR imgui::imgui
            INTERFACE_INCLUDE_DIRECTORIES)

        add_library(ImGui::Sources INTERFACE IMPORTED)
        set_property(TARGET ImGui::Sources APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES ImGui::ImGui)
    endif()

# Otherwise find the source files and compile them as part of the library they
# get linked to
else()
    # Disable the find root path here, it overrides the
    # CMAKE_FIND_ROOT_PATH_MODE_INCLUDE setting potentially set in
    # toolchains.
    find_path(ImGui_INCLUDE_DIR NAMES imgui.h HINTS ${IMGUI_DIR}
        NO_CMAKE_FIND_ROOT_PATH)
    mark_as_advanced(ImGui_INCLUDE_DIR)

    if(NOT TARGET ImGui::ImGui)
        add_library(ImGui::ImGui INTERFACE IMPORTED)
        set_property(TARGET ImGui::ImGui APPEND PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${ImGui_INCLUDE_DIR})

        ## Handle export and import of imgui symbols via IMGUI_API definition
        ## in visibility.h of Magnum ImGuiIntegration.
        #set_property(TARGET ImGui::ImGui APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
        #    "IMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\"")
    endif()
endif()

macro(_imgui_setup_source_file source_var)
    ## Handle export and import of imgui symbols via IMGUI_API
    ## definition in visibility.h of Magnum ImGuiIntegration.
    #set_property(SOURCE ${${source_var}} APPEND PROPERTY COMPILE_DEFINITIONS
    #    "IMGUI_USER_CONFIG=\"Magnum/ImGuiIntegration/visibility.h\"")

    # Hide warnings from imgui source files

    # GCC- and Clang-specific flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?Clang"
        AND NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC") OR CORRADE_TARGET_EMSCRIPTEN)
        set_property(SOURCE ${${source_var}} APPEND_STRING PROPERTY COMPILE_FLAGS
            " -Wno-old-style-cast -Wno-zero-as-null-pointer-constant")
    endif()

    # GCC-specific flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set_property(SOURCE ${${source_var}} APPEND_STRING PROPERTY COMPILE_FLAGS
            " -Wno-double-promotion")
    endif()

    mark_as_advanced(${source_var})
endmacro()

# Find components
foreach(_component IN LISTS ImGui_FIND_COMPONENTS)
    if(_component STREQUAL "Sources")
        if(NOT TARGET ImGui::Sources)
            set(ImGui_Sources_FOUND TRUE)
            set(ImGui_SOURCES )

            foreach(_file imgui imgui_widgets imgui_draw  imgui_tables)
                # Disable the find root path here, it overrides the
                # CMAKE_FIND_ROOT_PATH_MODE_INCLUDE setting potentially set in
                # toolchains.
                find_file(ImGui_${_file}_SOURCE NAMES ${_file}.cpp
                    HINTS ${IMGUI_DIR} NO_CMAKE_FIND_ROOT_PATH)
                list(APPEND ImGui_SOURCES ${ImGui_${_file}_SOURCE})

                if(NOT ImGui_${_file}_SOURCE)
                    set(ImGui_Sources_FOUND FALSE)
                    break()
                endif()

                _imgui_setup_source_file(ImGui_${_file}_SOURCE)
            endforeach()

            add_library(ImGui::Sources INTERFACE IMPORTED)
            set_property(TARGET ImGui::Sources APPEND PROPERTY
                INTERFACE_SOURCES "${ImGui_SOURCES}")
            set_property(TARGET ImGui::Sources APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ImGui::ImGui)
        else()
            set(ImGui_Sources_FOUND TRUE)
        endif()
    elseif(_component STREQUAL "SourcesMiscCpp")
        set(ImGui_SourcesMiscCpp_FOUND TRUE)
        set(ImGui_SOURCES )

        foreach(_file imgui_stdlib)
            # Disable the find root path here, it overrides the
            # CMAKE_FIND_ROOT_PATH_MODE_INCLUDE setting potentially set in
            # toolchains.
            find_file(ImGui_${_file}_MISC_CPP_SOURCE NAMES ${_file}.cpp
                HINTS ${IMGUI_DIR}/misc/cpp NO_CMAKE_FIND_ROOT_PATH)
            list(APPEND ImGui_MISC_CPP_SOURCES ${ImGui_${_file}_MISC_CPP_SOURCE})

            if(NOT ImGui_${_file}_MISC_CPP_SOURCE)
                set(ImGui_SourcesMiscCpp_FOUND FALSE)
                break()
            endif()

            _imgui_setup_source_file(ImGui_${_file}_MISC_CPP_SOURCE)
        endforeach()

        if(NOT TARGET ImGui::SourcesMiscCpp)
            add_library(ImGui::SourcesMiscCpp INTERFACE IMPORTED)
            set_property(TARGET ImGui::SourcesMiscCpp APPEND PROPERTY
                INTERFACE_SOURCES "${ImGui_MISC_CPP_SOURCES}")
            set_property(TARGET ImGui::SourcesMiscCpp APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ImGui::ImGui)
        endif()
    endif()
    if(_component STREQUAL "Examples")
        if(NOT TARGET ImGui::Examples)
            set(ImGui_Examples_FOUND TRUE)
            set(ImGui_EXAMPLES )
			# NOTE: these will need to be updated based upon the SDL/OpenGL or other graphics backend used
            foreach(_file imgui_impl_opengl2 imgui_impl_sdl)
                # Disable the find root path here, it overrides the
                # CMAKE_FIND_ROOT_PATH_MODE_INCLUDE setting potentially set in
                # toolchains.
                # find_file(ImGui_${_file}_EXAMPLE NAMES ${_file}.h
                    # HINTS ${IMGUI_DIR}/examples NO_CMAKE_FIND_ROOT_PATH)
                # list(APPEND ImGui_EXAMPLES ${ImGui_${_file}_EXAMPLE})

                # if(NOT ImGui_${_file}_EXAMPLE)
                    # set(ImGui_Examples_FOUND FALSE)
                    # break()
                # endif()

                # _imgui_setup_source_file(ImGui_${_file}_EXAMPLE)
				
                find_file(ImGui_${_file}_EXAMPLE NAMES ${_file}.cpp
                    HINTS ${IMGUI_DIR}/backends NO_CMAKE_FIND_ROOT_PATH)
                list(APPEND ImGui_EXAMPLES ${ImGui_${_file}_EXAMPLE})

                if(NOT ImGui_${_file}_EXAMPLE)
                    set(ImGui_Examples_FOUND FALSE)
                    break()
                endif()

                _imgui_setup_source_file(ImGui_${_file}_EXAMPLE)
            endforeach()

            add_library(ImGui::Examples INTERFACE IMPORTED)
            set_property(TARGET ImGui::Examples APPEND PROPERTY
                INTERFACE_SOURCES "${ImGui_EXAMPLES}")
			# set_property(TARGET ImGui::Examples APPEND PROPERTY
				# INTERFACE_INCLUDE_DIRECTORIES ${IMGUI_DIR}/examples)
            set_property(TARGET ImGui::Examples APPEND PROPERTY
                INTERFACE_LINK_LIBRARIES ImGui::ImGui)
        else()
            set(ImGui_Examples_FOUND TRUE)
        endif()
    endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ImGui REQUIRED_VARS ImGui_INCLUDE_DIR HANDLE_COMPONENTS)