# Minimum CMake version required
cmake_minimum_required(VERSION 2.8.11)

# Global name of the project
project(QMLGMlibTest)

# Version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# Adjust the CMAKE_MODULE_PATH to include the "local" cmake/Modules directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



################################
# Find the required Qt libraries

find_package(Qt5Core 5.1)
find_package(Qt5Quick 5.1)
find_package(Qt5Gui 5.1)

################################
# Find GMlib
find_package(
  GMlib 0.4 EXACT
  PATHS ${GMLIB_DIR}
  NO_DEFAULT_PATH
)
include_directories( ${GLEW_INCLUDE_DIRS} )
add_definitions(-DGLEW_STATIC)

#######
# Files

set( HDRS
  glscenerenderer.h
  gmlibwrapper.h
  guiapplication.h
  window.h
  )

set( SRCS
  glscenerenderer.cpp
  gmlibwrapper.cpp
  guiapplication.cpp
  main.cpp
  window.cpp
  )

#########
# Moc'ing
QT5_WRAP_CPP( HDRS_MOC
  glscenerenderer.h
  gmlibwrapper.h
  guiapplication.h
  window.h
  )

set( RCCS
 qml.qrc
)

QT5_ADD_RESOURCES( RCCS_MOC ${RCCS} )

#########
# Compile
add_executable( ${CMAKE_PROJECT_NAME} ${HDRS} ${SRCS} ${HDRS_MOC} ${FORM_HDRS} ${RCCS_MOC} )

######
# Link
target_link_libraries( ${CMAKE_PROJECT_NAME}
  ${GMLIB_LIBRARIES}
  Qt5::Core
  Qt5::Quick
  Qt5::Gui
  ${GLEW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )
