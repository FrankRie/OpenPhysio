cmake_minimum_required(VERSION 3.16)

project(SmtpClient-for-Qt LANGUAGES CXX)

add_library(SmtpClient STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/smtpclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimemessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimetext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimeattachment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimemultipart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/emailaddress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimepart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimefile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimehtml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimeinlinefile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/quotedprintable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimecontentencoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimebase64encoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimeqpencoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimeqpformatter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimebase64formatter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mimecontentformatter.cpp

)

target_include_directories(SmtpClient PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(SmtpClient PUBLIC Qt6::Core Qt6::Network)

# So wie bei SimpleMail:
set(LIBRARY_TARGET_NAME SmtpClient CACHE INTERNAL "Library Target Name")
