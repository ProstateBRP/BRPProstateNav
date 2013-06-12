cmake_minimum_required(VERSION 2.8.9)

project(BRPProstateNav)

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/BRPProstateNav")
set(EXTENSION_CATEGORY "IGT")
set(EXTENSION_CONTRIBUTORS "Junichi Tokuda (BWH)")
set(EXTENSION_DESCRIPTION "3D Slicer Extension for MRI-guided Robotic Prostate Intervention")
set(EXTENSION_ICONURL "http://viewvc.slicer.org/viewvc.cgi/Slicer4/trunk/Extensions/Testing/BRPProstateNav/BRPProstateNav.png?revision=21746&view=co")
set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/4/42/Slicer-r19441-BRPProstateNav-screenshot.png")

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(BRPProstateNav)

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
