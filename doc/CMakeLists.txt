find_package(Doxygen REQUIRED)

set(DOXYGEN_BUILTIN_STL_SUPPORT    YES)
set(DOXYGEN_DISTRIBUTE_GROUP_DOC   YES)
set(DOXYGEN_EXTRACT_STATIC         YES)
set(DOXYGEN_CASE_SENSE_NAMES       YES)
set(DOXYGEN_SORT_GROUP_NAMES       YES)
set(DOXYGEN_GENERATE_TODOLIST      YES)
set(DOXYGEN_SHOW_FILES             YES)
set(DOXYGEN_FILE_PATTERNS          *.c *.h *.dox)
set(DOXYGEN_WARN_IF_UNDOCUMENTED   YES)
set(DOXYGEN_WARN_LOGFILE           doxygenUserWarnings.log)
set(DOXYGEN_IMAGE_PATH             ${PROJECT_SOURCE_DIR}/doc/graphics)
set(DOXYGEN_ALPHABETICAL_INDEX     NO)
set(DOXYGEN_DISABLE_INDEX          YES)
set(DOXYGEN_GENERATE_TREEVIEW      YES)
set(DOXYGEN_GENERATE_HTML          YES)
set(DOXYGEN_SEARCHENGINE           NO)
set(DOXYGEN_CALL_GRAPH             YES)
set(DOXYGEN_CALLER_GRAPH           YES)
set(DOXYGEN_DOT_IMAGE_FORMAT       svg)

set(DOXYGEN_EXTRACT_PRIVATE       YES)
set(DOXYGEN_EXTRACT_ALL 	  YES)
set(DOXYGEN_EXTRACT_LOCAL_CLASSES YES)
set(DOXYGEN_HIDE_SCOPE_NAMES      NO)
set(DOXYGEN_SHOW_INCLUDE_FILES    YES)
set(DOXYGEN_SORT_BY_SCOPE_NAME    YES)
set(DOXYGEN_SHOW_USED_FILES       YES)
set(DOXYGEFMIC_N_VERBATIM_HEADERS      YES)
set(DOXYGEFMIC_N_VERBATIM_VARS DOXYGEN_ALIASES)
set(DOXYGEN_RECURSIVE              YES)
set(documentation_directories      ${PROJECT_SOURCE_DIR}/src
                                   ${PROJECT_SOURCE_DIR}/include
				   ${PROJECT_SOURCE_DIR}/doc)




# Ensure that all images are copied, they are not auto copied since most of them are added through alias commands
file(GLOB png_files ${CMAKE_CURRENT_LIST_DIR}/graphics/*.png)
set(DOXYGEN_HTML_EXTRA_FILES ${png_files})

# On MINGW CMake TO_NATIVE_PATH inside FindDoxygen does not work, and we need to fix the path here
if (MINGW)
    string(REPLACE "\\" "" DOXYGEN_DOT_PATH "${DOXYGEN_DOT_PATH}")
    string(REPLACE "/" "\\" DOXYGEN_DOT_PATH "${DOXYGEN_DOT_PATH}")
    message(STATUS "Using dot path: ${DOXYGEN_DOT_PATH}")
endif()

doxygen_add_docs(
    documentation
    ALL
    ${documentation_directories}
    COMMENT "Generate HTML documentation"
)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION ./doc)
