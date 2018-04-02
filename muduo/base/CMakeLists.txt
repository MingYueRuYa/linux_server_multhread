set( base_SRCS
    CountDownLatch.cc
    Exception.cc
    FileUtil.cc
    LogFile.cc
    Logging.cc
    LogStream.cc
    Thread.cc
    ThreadPool.cc
    Timestamp.cc 
)

add_library(muduo_base ${base_SRCS})
target_link_libraries(muduo_base pthread rt)

install(TARGETS muduo_base DESTINATION lib)
file(GLOB HEADES "*.h")
install(FILES ${HEADES} DESTINATION include/muduo/base)

if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(tests)
endif()












