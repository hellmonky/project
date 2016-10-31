# 设置最小的构建版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 设置当前的工程名称和版本号
PROJECT(WebServicewithCurl CXX)
SET(LIB_MAJOR_VERSION "1")
SET(LIB_MINOR_VERSION "1")
SET(LIB_PATCH_VERSION "0")
SET(LIB_VERSION_STRING "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_PATCH_VERSION}")

# 设置LIBCURL库的头文件路径：
FIND_PATH(LIBCURL_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 设置外部依赖的第三方库路径：
FIND_LIBRARY(LIB_CURL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/libcurl_imp.lib)

# 添加库的子文件夹：
ADD_SUBDIRECTORY(src)
# 添加测试库子文件夹：
ADD_SUBDIRECTORY(libtest)

# 设置生成的release和debug版本命名区别：
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")  
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")  
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")  
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")