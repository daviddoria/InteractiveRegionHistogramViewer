cmake_minimum_required(VERSION 2.6)

Project(InteractiveRegionHistogramViewer)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${USE_VTK_FILE})

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${USE_ITK_FILE})

QT4_WRAP_UI(UISrcs InteractiveRegionHistogramViewerWidget.ui HistogramWidget.ui)
QT4_WRAP_CPP(MOCSrcs InteractiveRegionHistogramViewerWidget.h HistogramWidget.h)

ADD_EXECUTABLE(InteractiveRegionHistogramViewer InteractiveRegionHistogramViewer.cpp
InteractiveRegionHistogramViewerWidget.cpp
RegionSelectionWidget.cpp
HistogramWidget.cpp
Layer.cpp
Helpers.cpp ${MOCSrcs} ${UISrcs})
TARGET_LINK_LIBRARIES(InteractiveRegionHistogramViewer ${VTK_LIBRARIES} QVTK ${QT_LIBRARIES} ${ITK_LIBRARIES})
