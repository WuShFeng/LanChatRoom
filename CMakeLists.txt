cmake_minimum_required(VERSION 3.27)
project(LanChatRoom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_WIN32_EXECUTABLE TRUE)
set(CMAKE_PREFIX_PATH "C:/Tools/Qt/6.6.1/mingw_64")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
add_custom_target(BuildAll
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target LanChatRoom --target FileSender  --target FileReceiver
        COMMENT "Building LanChatRoom and FileSender"
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

add_executable(LanChatRoom main.cpp
        chatroom.cpp
        chatroom.h
        chatroom.ui
        tools/FileConvert.cpp
        tools/FileConvert.h
        logo.rc
        components/UdpServices/UdpServer.cpp
        components/UdpServices/UdpServer.h
)
add_executable(FileSender components/fileTransfer/FileSenderMain.cpp
        components/fileTransfer/filetransfer.cpp
        components/fileTransfer/filetransfer.h
        components/fileTransfer/filetransfer.ui
        components/fileTransfer/FileSender.cpp
        components/fileTransfer/FileSender.h
        components/fileTransfer/fileSender.rc
        components/UdpServices/UdpServer.cpp
        components/UdpServices/UdpServer.h
)
add_executable(FileReceiver components/fileTransfer/FileReceiver.cpp
        components/fileTransfer/FileReceiver.h
        components/fileTransfer/filetransfer.cpp
        components/fileTransfer/filetransfer.h
        components/fileTransfer/filetransfer.ui
        components/fileTransfer/FileReceiverMain.cpp
        components/fileTransfer/fileReceiver.rc
        components/UdpServices/UdpServer.cpp
        components/UdpServices/UdpServer.h
        components/UdpServices/UdpServer.cpp
        components/UdpServices/UdpServer.h
)
target_link_libraries(LanChatRoom
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
)
target_link_libraries(FileSender
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
)
target_link_libraries(FileReceiver
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
)
if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets Network)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
