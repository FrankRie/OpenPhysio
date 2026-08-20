# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.16)
set(CMAKE_RC_COMPILER llvm-rc)

project(PhysioApp VERSION 9.0 LANGUAGES CXX)

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatische Verarbeitung von MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SMTPCLIENT_LIB ${CMAKE_BINARY_DIR}/external/SmtpClient-build/libSmtpClient.a)
set(OPENSSL_ROOT_DIR "${CMAKE_SOURCE_DIR}/../external/install")
set(OPENSSL_USE_STATIC_LIBS TRUE)

# Qt6 Module
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Help
    Svg
    SvgWidgets
    Xml
    Sql
    Network
    PrintSupport
)

# -----------------------
# Quellen & Header
# -----------------------
set(SOURCE_FILES
    main.cpp
    config.cpp
    dlgdate.cpp
    dlgbill2.cpp
    dlgbill1.cpp
    geneinv.cpp
    protect.cpp
    browsedoctemplates.cpp
    browse.cpp
    dlgreceipeoptions.cpp
    dlgcourse.cpp
    browseemployees.cpp
    browsecourses.cpp
    browseinvoices.cpp
    browsepinvoices.cpp
    gendoc.cpp
    therapy.cpp
    mydb.cpp
    dlgreceipepriv.cpp
    browseroutes.cpp
    kvkread.cpp
    browsephysicians.cpp
    browseinsurances.cpp
    browsereceipes.cpp
    browsepatients.cpp
    dlgabout.cpp
    dlgreceipe.cpp
    dlgempl.cpp
    dlgphys.cpp
    dlgpatient.cpp
    openphysio.cpp
    browsedocs.cpp
    browsequittances.cpp
    browsereminders.cpp
    msgbox.cpp
    browsereports.cpp
    dlgdocfinding.cpp
    browsefindings.cpp
    dlglogin.cpp
    dlgdocviewer.cpp
    helperwidget.cpp
    dlghelp.cpp
    helperwrapper.cpp
    displayfilter.cpp
    browsecashpos.cpp
    browsecash.cpp
    widgetcashloss.cpp
    widgetcasharticle.cpp
    dlgbackup.cpp
    calendarwidget.cpp
    dlgmanagerooms.cpp
    dlgmanagetherapygroups.cpp
    dlgbrowse.cpp
    dlgmanagecars.cpp
    dlgpinvoice.cpp
    dlgdatemove.cpp
    browsepraxis.cpp
    employeetools.cpp
    dlgpraxis.cpp
    dlgschedule.cpp
    datetools.cpp
    dlgreceipeprof.cpp
    src_docedit/dlgdoceditor.cpp
    src_docedit/dlgdoceditdata.cpp
    src_docedit/dlgdoceditnew.cpp
    src_docedit/dlgdocopen.cpp
    src_docedit/dlgdocsave.cpp
    src_docedit/doctools.cpp
    dlgtextblock.cpp
)

set(HEADER_FILES
    config.h
    dlgdate.h
    dlgbill2.h
    dlgbill1.h
    geneinv.h
    protect.h
    browsedoctemplates.h
    browse.h
    dlgreceipeoptions.h
    dlgcourse.h
    browseemployees.h
    browsecourses.h
    browseinvoices.h
    browsepinvoices.h
    gendoc.h
    therapy.h
    mydb.h
    dlgreceipepriv.h
    browseroutes.h
    kvkread.h
    browsephysicians.h
    browseinsurances.h
    browsereceipes.h
    browsepatients.h
    dlgabout.h
    dlgreceipe.h
    dlgempl.h
    dlgphys.h
    ctbcs.h
    dlgpatient.h
    openphysio.h
    browsedocs.h
    browsequittances.h
    browsereminders.h
    msgbox.h
    browsereports.h
    dlgdocfinding.h
    browsefindings.h
    dlglogin.h
    dlgdocviewer.h
    helperwidget.h
    dlghelp.h
    helperwrapper.h
    displayfilter.h
    browsecashpos.h
    browsecash.h
    widgetcashloss.h
    widgetcasharticle.h
    dlgbackup.h
    calendarwidget.h
    dlgmanagerooms.h
    dlgmanagetherapygroups.h
    dlgbrowse.h
    dlgmanagecars.h
    dlgpinvoices.h
    dlgdatemove.h
    browsepraxis.h
    employeetools.h
    dlgpraxis.h
    dlgschedule.h
    datetools.h
    dlgreceipeprof.h
    src_docedit/dlgdoceditor.h
    src_docedit/dlgdoceditdata.h
    src_docedit/dlgdoceditnew.h
    src_docedit/dlgdocopen.h
    src_docedit/dlgdocsave.h
    src_docedit/doctools.h
    dlgtextblock.h
)

# -----------------------
# UI-Dateien
# -----------------------
set(UI_FILES
    dlgdate.ui
    dlgbill2.ui
    dlgbill1.ui
    browsedoctemplates.ui
    dlgreceipeoptions.ui
    dlgcourse.ui
    browseemployees.ui
    browsecourses.ui
    browseinvoices.ui
    browsepinvoices.ui
    dlgreceipepriv.ui
    browseroutes.ui
    browsephysicians.ui
    browsepraxis.ui
    browseinsurances.ui
    browsereceipes.ui
    browsepatients.ui
    dlgabout.ui
    dlgreceipe.ui
    dlgempl.ui
    dlgphys.ui
    dlgpatient.ui
    openphysio.ui
    browsedocs.ui
    browsequittances.ui
    browsereminders.ui
    browsereports.ui
    dlgdocfinding.ui
    browsefindings.ui
    dlglogin.ui
    dlgdocviewer.ui
    dlghelp.ui
    displayfilter.ui
    browsecashpos.ui
    browsecash.ui
    widgetcashloss.ui
    widgetcasharticle.ui
    dlgbackup.ui
    calendarwidget.ui
    dlgmanagerooms.ui
    dlgmanagetherapygroups.ui
    dlgbrowse.ui
    dlgmanagecars.ui
    dlgdatemove.ui
    dlgpinvoice.ui
    dlgschedule.ui
    dlgpraxis.ui
    dlgreceipeprof.ui
    dlgtextblock.ui
)

# -----------------------
# Ressourcen
# -----------------------
set(RESOURCE_FILES
    images/images.qrc
)

# -----------------------
# SmtpClient
# -----------------------
# externe Lib einbinden
# Angabe des Source-Ordners und des Binary-Ordners für die externe Lib
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../external/SmtpClient/
    ${CMAKE_BINARY_DIR}/external/SmtpClient-build
)


# -----------------------
# libqrencode (intern)
# -----------------------
# libqrencode ohne PNG
set(WITH_PNG OFF CACHE BOOL "Disable PNG support in libqrencode" FORCE)
#set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ENABLE_PNG OFF CACHE BOOL "" FORCE)

set(QRENCODE_SHARED OFF CACHE BOOL "Build libqrencode shared" FORCE)
add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../external/libqrencode/
    ${CMAKE_BINARY_DIR}/external/libqrencode-build
)

if(UNIX)

    set(OPENSSL_ROOT_DIR "/usr")
    set(OPENSSL_INCLUDE_DIR "/usr/include")
    set(OPENSSL_CRYPTO_LIBRARY "/usr/lib64/libcrypto.so")
    set(OPENSSL_SSL_LIBRARY "/usr/lib64/libssl.so")

endif()

find_package(OpenSSL REQUIRED)

# -----------------------
# Executable zusammenstellen
# -----------------------
add_executable(PhysioApp
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${UI_FILES}
    ${RESOURCE_FILES}
)

set_target_properties(PhysioApp PROPERTIES OUTPUT_NAME "openphysio")

# Include-Verzeichnisse für dein Projekt
target_include_directories(PhysioApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}             # Basis: Projektverzeichnis
    ${CMAKE_CURRENT_SOURCE_DIR}/src_docedit # für src_docedit/doctools.h
    ${CMAKE_CURRENT_SOURCE_DIR}/images      # falls Header in images/ liegen
    ${CMAKE_SOURCE_DIR}/../external/SmtpClient
    ${CMAKE_SOURCE_DIR}/../external/libqrencode
)

# -----------------------
# Mit Qt6 verbinden
# -----------------------
target_link_libraries(PhysioApp PRIVATE
    ${LIBRARY_TARGET_NAME}
    ${QRENCODE_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Help
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::Xml
    Qt6::Sql
    Qt6::Network
    Qt6::PrintSupport
    qrencode
#    pcsclite
)

# -----------------------
# Plattformabhängige Include- und Link-Pfade
# -----------------------
if(WIN32)
    set_target_properties(PhysioApp PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
#    # Windows-Spezifisch: Ressourcen-Datei (falls vorhanden)
#    target_sources(PhysioApp PRIVATE
#       appicon/appicon.rc
#    )
elseif(UNIX)
    # Ersetzen Sie "MeinProjekt" durch Ihren Target-Namen
    target_include_directories(PhysioApp PRIVATE
        "/usr/include/PCSC"
    )

    target_link_libraries(PhysioApp PRIVATE
        pcsclite
    )
endif()

#add_subdirectory(.)
