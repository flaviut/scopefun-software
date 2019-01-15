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
    source
    lib/cJSON
    lib/kiss_fft130
    lib/libusb-1.0.20/examples/
)

set(SOURCE_FILES
    # cJSON
    lib/cJSON/cJSON.c
    # kiss_fft130
    lib/kiss_fft130/kiss_fft.c
    # libusb
    lib/libusb-1.0.20/examples/ezusb.c

    # scopefun sources
    source/core/input/input.cpp
    source/core/core.cpp
    source/core/string/corestring.cpp
    source/core/format/format.cpp
    source/core/render/font/font.cpp
    source/core/render/canvas/canvas2d.cpp
    source/core/render/canvas/canvas3d.cpp
    source/core/render/render/render.cpp
    source/core/render/camera/camera.cpp
    source/core/opengl/opengl_devicestate.cpp
    source/core/opengl/opengl_texture.cpp
    source/core/opengl/opengl_device.cpp
    source/core/opengl/opengl_staticmesh.cpp
    source/core/opengl/opengl.cpp
    source/core/opengl/opengl_shader.cpp
    source/core/memory/memory.cpp
    source/core/libusb/libusb.cpp
    source/core/manager/manager.cpp
    source/core/file/file.cpp
    source/core/timer/timer.cpp
    source/osciloscope/osciloscope.cpp
    source/osciloscope/gui/OsciloskopOsciloskop.cpp
    source/osciloscope/gui/OsciloskopMeasure.cpp
    source/osciloscope/gui/OsciloskopDebug.cpp
    source/osciloscope/gui/OsciloskopHardwareGenerator.cpp
    source/osciloscope/gui/OsciloskopThermal.cpp
    source/osciloscope/gui/OsciloskopInfo.cpp
    source/osciloscope/gui/osc.cpp
    source/osciloscope/gui/OsciloskopDisplay.cpp
    source/osciloscope/gui/OsciloskopSoftwareGenerator.cpp
    source/osciloscope/osciloscope/oscrender.cpp
    source/osciloscope/osciloscope/oscsignal.cpp
    source/osciloscope/osciloscope/oscfile.cpp
    source/osciloscope/osciloscope/oscfft.cpp
    source/osciloscope/osciloscope/oscsettings.cpp
    source/osciloscope/osciloscope/oscmng.cpp
    source/osciloscope/window/wndshadow.cpp
    source/osciloscope/window/wndmain.cpp
    source/osciloscope/window/tool.cpp
    source/osciloscope/window/wnddisplay.cpp
    source/osciloscope/window/wndgenerate.cpp
    source/osciloscope/app/managers.cpp
    source/osciloscope/app/app.cpp
    source/osciloscope/xpm/xpm96.c
    source/osciloscope/xpm/xpm512.c
    source/osciloscope/xpm/xpm64.c
    source/osciloscope/xpm/xpm256.c
    source/osciloscope/xpm/xpm16.c
    source/osciloscope/xpm/xpm32.c
    source/osciloscope/xpm/xpm128.c
)

add_executable(scopefun ${SOURCE_FILES})
target_link_libraries(scopefun
    ${SDL2_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${wxWidgets_LIBRARIES}
    ${LibUSB_LIBRARIES}
)
