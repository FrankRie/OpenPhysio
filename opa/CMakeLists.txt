cmake_minimum_required(VERSION 3.16)
project(opa LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENSSL_ROOT_DIR "${CMAKE_SOURCE_DIR}/../external/install")

#find_package(Qt6 REQUIRED COMPONENTS Core Network)

# Qt6-Pakete
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Network
    Gui
    Sql
    Xml
    Svg
    SvgWidgets
    Help
    PrintSupport
    Widgets
)

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../openphysio
)

# Header-Dateien
set(HEADERS
    opa.h
    confsurgery.h
    confdatabase.h
    confbase.h
    confprinter.h
    ../openphysio/config.h
    ../openphysio/mydb.h
    ../openphysio/msgbox.h
    ../openphysio/dlgabout.h
    ../openphysio/helperwidget.h
    ../openphysio/dlghelp.h
    ../openphysio/helperwrapper.h
    confreader.h
    confdisplay.h
    confroles.h
    confbackup.h
    confupdates.h
    confpricespriv.h
    confperm.h
    dlgupdateinsurances.h
    confhotkeys.h
    conftakeover.h
    confdocs.h
    confplannerdisplay.h
    confother.h
    confadmin.h
    dlgaccess.h
    confshortcuts.h
    confprices.h
    ../openphysio/src_docedit/dlgdoceditor.h
    ../openphysio/src_docedit/dlgdoceditdata.h
    ../openphysio/src_docedit/dlgdoceditnew.h
    ../openphysio/src_docedit/dlgdocopen.h
    ../openphysio/src_docedit/dlgdocsave.h
    ../openphysio/src_docedit/doctools.h
    conftextmodules.h
)

# Quellen
set(SOURCES
    main.cpp
    opa.cpp
    confsurgery.cpp
    confdatabase.cpp
    confbase.cpp
    confprinter.cpp
    ../openphysio/config.cpp
    ../openphysio/mydb.cpp
    ../openphysio/msgbox.cpp
    ../openphysio/dlgabout.cpp
    ../openphysio/helperwidget.cpp
    ../openphysio/dlghelp.cpp
    ../openphysio/helperwrapper.cpp
    confreader.cpp
    confdisplay.cpp
    confroles.cpp
    confbackup.cpp
    confupdates.cpp
    confpricespriv.cpp
    confperm.cpp
    dlgupdateinsurances.cpp
    confhotkeys.cpp
    conftakeover.cpp
    confdocs.cpp
    confplannerdisplay.cpp
    confother.cpp
    confadmin.cpp
    dlgaccess.cpp
    confshortcuts.cpp
    confprices.cpp
    ../openphysio/src_docedit/dlgdoceditor.cpp
    ../openphysio/src_docedit/dlgdoceditdata.cpp
    ../openphysio/src_docedit/dlgdoceditnew.cpp
    ../openphysio/src_docedit/dlgdocopen.cpp
    ../openphysio/src_docedit/dlgdocsave.cpp
    ../openphysio/src_docedit/doctools.cpp
    conftextmodules.cpp
)

# UI-Dateien
set(FORMS
    opa.ui
    ../openphysio/dlgabout.ui
    ../openphysio/dlghelp.ui
    confsurgery.ui
    confdatabase.ui
    confprinter.ui
    confreader.ui
    confdisplay.ui
    confroles.ui
    confbackup.ui
    confupdates.ui
    confpricespriv.ui
    confperm.ui
    dlgupdateinsurances.ui
    confhotkeys.ui
    conftakeover.ui
    confdocs.ui
    confplannerdisplay.ui
    confother.ui
    confadmin.ui
    dlgaccess.ui
    confshortcuts.ui
    confprices.ui
    ../openphysio/src_docedit/dlgdoceditor.ui
    ../openphysio/src_docedit/dlgdoceditdata.ui
    ../openphysio/src_docedit/dlgdoceditnew.ui
    ../openphysio/src_docedit/dlgdocopen.ui
    ../openphysio/src_docedit/dlgdocsave.ui
    conftextmodules.ui
)

# Ressourcen
set(RESOURCES
    images/images.qrc
    ../openphysio/src_docedit/docedit.qrc
)

# Automatische Qt-Verarbeitung
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(OpenSSL REQUIRED)

# Executable
qt_add_executable(opa
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)
target_compile_definitions(opa PRIVATE
    OPA
)

# Plattformabhängige Einstellungen
if(WIN32)
    set_target_properties(opa PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
#    # Windows-Spezifisch: Ressourcen-Datei (falls vorhanden)
#    target_sources(opa PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/../run/appicon/appicon.rc
#    )
elseif(APPLE)
    # macOS-Spezifisch: Icon + Info.plist
    set_target_properties(opa PROPERTIES
        MACOSX_BUNDLE_ICON_FILE opa.icns
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
    target_sources(opa PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/appicon/opa.icns
    )
elseif(UNIX)
    # Linux-Spezifisch: (pcsclite explizit entfernt)
    message(STATUS "Building on Linux – no pcsclite linked.")
endif()


# Linken
target_link_libraries(opa PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    Qt6::Core
    Qt6::Network
    Qt6::Gui
    Qt6::Sql
    Qt6::Xml
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Help
    Qt6::PrintSupport
    Qt6::Widgets
)

