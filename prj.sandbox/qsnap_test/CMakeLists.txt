project(qsnap_test)

set(qsnap_LIBS opencv_core)

set(qsnap_test_SRCS src/main.cpp)
set(qsnap_test_HDRS)

add_executable(qsnap_test ${qsnap_test_SRCS} ${qsnap_test_HDRS} )

add_definitions(-DQSNP_DYNAMIC)
if(MSVC)
  target_link_libraries(qsnap_test opencv_core)
ELSE()
  target_link_libraries(qsnap_test opencv_core dl)
endif()

add_dependencies(qsnap_test qsnap opencv_core)

set_property(TARGET qsnap_test PROPERTY FOLDER "prj.sandbox")
