set(pyduchaintest_SRCS
    pyduchaintest.cpp
    ../duchaindebug.cpp)

ecm_add_test(${pyduchaintest_SRCS}
    TEST_NAME pyduchaintest)

set(duchainbench_SRCS
    duchainbench.cpp
    ../duchaindebug.cpp)

ecm_add_test(${duchainbench_SRCS}
    TEST_NAME duchainbench)

add_definitions(-DDUCHAIN_PY_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(pyduchaintest
    kdevpythonduchain
    kdevpythonparser
    ${kdevpythonparser_LIBRARIES}
    Qt${QT_MAJOR_VERSION}::Test
    KDev::Tests
)

target_link_libraries(duchainbench
    kdevpythonduchain
    kdevpythonparser
    ${kdevpythonparser_LIBRARIES}
    Qt${QT_MAJOR_VERSION}::Test
    KDev::Tests
)

if (BUILD_ENAML_SUPPORT)
    set(enamlduchaintest_SRCS
        enamlduchaintest.cpp
        ../duchaindebug.cpp)
    ecm_add_test(${enamlduchaintest_SRCS}
        TEST_NAME enamlduchaintest)
    target_link_libraries(enamlduchaintest
        kdevpythonduchain
        kdevpythonparser
        ${kdevpythonparser_LIBRARIES}
        Qt${QT_MAJOR_VERSION}::Test
        KDev::Tests
    )
endif()
