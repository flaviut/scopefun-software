cmake_minimum_required(VERSION 3.2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O2 -Wfatal-errors")

add_definitions(-DPLATFORM_LINUX -DLINUX -D__WXGTK__)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(wxWidgets COMPONENTS core base adv REQUIRED)
find_package(LibUSB REQUIRED)

include(${wxWidgets_USE_FILE})
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${wxWidgets_INCLUDE_DIRS}
    ${LibUSB_INCLUDE_DIRS}
    src
    src/vendor/cJSON
    src/vendor/kiss_fft130
)

set(SOURCE_FILES
    src/vendor/cJSON/cJSON.c
    src/vendor/kiss_fft130/kiss_fft.c

    # scopefun sources
    src/core/input/input.cpp
    src/core/core.cpp
    src/core/string/corestring.cpp
    src/core/format/format.cpp
    src/core/render/font/font.cpp
    src/core/render/canvas/canvas2d.cpp
    src/core/render/canvas/canvas3d.cpp
    src/core/render/render/render.cpp
    src/core/render/camera/camera.cpp
    src/core/opengl/opengl_devicestate.cpp
    src/core/opengl/opengl_texture.cpp
    src/core/opengl/opengl_device.cpp
    src/core/opengl/opengl_staticmesh.cpp
    src/core/opengl/opengl.cpp
    src/core/opengl/opengl_shader.cpp
    src/core/memory/memory.cpp
    src/core/libusb/libusb.cpp
    src/core/libusb/ezusb.c
    src/core/manager/manager.cpp
    src/core/file/file.cpp
    src/core/timer/timer.cpp
    src/osciloscope/osciloscope.cpp
    src/osciloscope/gui/OsciloskopOsciloskop.cpp
    src/osciloscope/gui/OsciloskopMeasure.cpp
    src/osciloscope/gui/OsciloskopDebug.cpp
    src/osciloscope/gui/OsciloskopHardwareGenerator.cpp
    src/osciloscope/gui/OsciloskopThermal.cpp
    src/osciloscope/gui/OsciloskopInfo.cpp
    src/osciloscope/gui/osc.cpp
    src/osciloscope/gui/OsciloskopDisplay.cpp
    src/osciloscope/gui/OsciloskopSoftwareGenerator.cpp
    src/osciloscope/osciloscope/oscrender.cpp
    src/osciloscope/osciloscope/oscsignal.cpp
    src/osciloscope/osciloscope/oscfile.cpp
    src/osciloscope/osciloscope/oscfft.cpp
    src/osciloscope/osciloscope/oscsettings.cpp
    src/osciloscope/osciloscope/oscmng.cpp
    src/osciloscope/window/wndshadow.cpp
    src/osciloscope/window/wndmain.cpp
    src/osciloscope/window/tool.cpp
    src/osciloscope/window/wnddisplay.cpp
    src/osciloscope/window/wndgenerate.cpp
    src/osciloscope/app/managers.cpp
    src/osciloscope/app/app.cpp
    src/osciloscope/xpm/xpm96.c
    src/osciloscope/xpm/xpm512.c
    src/osciloscope/xpm/xpm64.c
    src/osciloscope/xpm/xpm256.c
    src/osciloscope/xpm/xpm16.c
    src/osciloscope/xpm/xpm32.c
    src/osciloscope/xpm/xpm128.c
)

add_executable(scopefun ${SOURCE_FILES})
target_link_libraries(scopefun
    ${SDL2_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${wxWidgets_LIBRARIES}
    ${LibUSB_LIBRARIES}
)
