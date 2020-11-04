# CMakeList.txt : CMake project for FFmpegCMake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("littlePlayer" VERSION 0.0.1)

# --------------------------------------------------------------------------
# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

IF(MSVC) 
    SET(CMAKE_CXX_FLAGS "/EHsc")  # deal with MSVC Warning C4530
ENDIF(MSVC)     
    
######################################
#  FFmpeg lib setting
######################################
#make sure ffmpeg/bin is in your PATH.
  
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)

find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)

find_path(AVFILTER_INCLUDE_DIR libavfilter/avfilter.h)
find_library(AVFILTER_LIBRARY avfilter)

find_path(POSTPROC_INCLUDE_DIR libpostproc/postprocess.h)
find_library(POSTPROC_LIBRARY postproc)

find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
find_library(SWRESAMPLE_LIBRARY swresample)

find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)



  set(CMAKE_INCLUDE_PATH
         "D:/dependence/ffmpeg-20191013-4f4334b-win64-static/bin/include"
        )
 
 set(CMAKE_LIBRARY_PATH
         "D:/dependence/ffmpeg-20191013-4f4334b-win64-static/bin/bin"
        )

set(SDL_INCLUDE_DIR "D:/dependence/ffmpeg-20191013-4f4334b-win64-static/tools/SDL/include")
set(SDL_LIBRARY "D:/dependence/ffmpeg-20191013-4f4334b-win64-static/tools/SDL/lib/x64/SDL2.lib") 
 
 
 
set(JRTPLIB_INCLUDE_PATH   "D:/dependence/jrtplib/x64-Release/include/jrtplib3")
set(JRTP_LIBRARY   "D:/dependence/jrtplib/x64-Release/lib/jrtplib.lib")

 set(JTHREAD_INCLUDE_DIR   "D:/dependence/jthread/x64-Release/include") 
 set(JTHREAD_LIBRARY   "D:/dependence/jthread/x64-Release/lib/jthread.lib")
 


  #[[
set(JRTPLIB_INCLUDE_PATH   "D:/dependence/jrtp/jrtplib/x64-Debug/include/jrtplib3")
set(JRTP_LIBRARY   "D:/dependence/jrtp/jrtplib/x64-Debug/lib/cq/jrtplib_d.lib")

set(JTHREAD_INCLUDE_DIR   "D:/dependence/jrtp/jthread/x64-Debug/include") 
set(JTHREAD_LIBRARY   "D:/dependence/jrtp/jthread/x64-Debug/lib/cq/jthread_d.lib")
]]


set(ASIO_DIR   "D:/dependence/asio-1.16.1/include")

set(OPENAL_INCLUDE_DIR "D:/dependence/OpenAL 1.1 SDK/include")
set(OPENAL_LIBRARY "D:/dependence/OpenAL 1.1 SDK/libs/Win64/OpenAL32.lib")
   

######################################
#  information
######################################

message(CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR})
message(PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR})

message("FFMPEG INFORMATION")
message("--------------------")
message(AVCODEC_INCLUDE_DIR=${AVCODEC_INCLUDE_DIR})
message(AVCODEC_LIBRARY=${AVCODEC_LIBRARY})
message("--------------------")
message(AVFORMAT_INCLUDE_DIR=${AVFORMAT_INCLUDE_DIR})
message(AVFORMAT_LIBRARY=${AVFORMAT_LIBRARY})
message("--------------------")
message(AVUTIL_INCLUDE_DIR=${AVUTIL_INCLUDE_DIR})
message(AVUTIL_LIBRARY=${AVUTIL_LIBRARY})
message("--------------------")
message(AVDEVICE_INCLUDE_DIR=${AVDEVICE_INCLUDE_DIR})
message(AVDEVICE_LIBRARY=${AVDEVICE_LIBRARY})
message("--------------------")
message(AVFILTER_INCLUDE_DIR=${AVFILTER_INCLUDE_DIR})
message(AVFILTER_LIBRARY=${AVFILTER_LIBRARY})
message("--------------------")
message(POSTPROC_INCLUDE_DIR=${POSTPROC_INCLUDE_DIR})
message(POSTPROC_LIBRARY=${POSTPROC_LIBRARY})
message("--------------------")
message(SWRESAMPLE_INCLUDE_DIR=${SWRESAMPLE_INCLUDE_DIR})
message(SWRESAMPLE_LIBRARY=${SWRESAMPLE_LIBRARY})
message("--------------------")
message(SWSCALE_INCLUDE_DIR=${SWSCALE_INCLUDE_DIR})
message(SWSCALE_LIBRARY=${SWSCALE_LIBRARY})


############################################
# Add source to this project's executable.
############################################
add_executable (${PROJECT_NAME} 
	 #"src/sdl_simple.cpp"
	 #"src/main.cpp"
	 #"src/sdl_audioplayer_simple.cpp"
	 "src/sdl_audio_video_player.cpp"
	 )


target_include_directories( ${PROJECT_NAME}  
	PRIVATE 
		${PROJECT_SOURCE_DIR}/include
		${AVCODEC_INCLUDE_DIR} 
		${AVFORMAT_INCLUDE_DIR} 
		${AVUTIL_INCLUDE_DIR} 
		${AVDEVICE_INCLUDE_DIR}
		${AVFILTER_INCLUDE_DIR}
		${POSTPROC_INCLUDE_DIR}
		${SWRESAMPLE_INCLUDE_DIR}
		${SWSCALE_INCLUDE_DIR}
		${SDL_INCLUDE_DIR}
		${OPENAL_INCLUDE_DIR}

)

target_link_libraries( ${PROJECT_NAME}  
	PRIVATE 
		${AVCODEC_LIBRARY} 
		${AVFORMAT_LIBRARY} 
		${AVUTIL_LIBRARY} 
		${AVDEVICE_LIBRARY}
		${AVFILTER_LIBRARY}
		${POSTPROC_LIBRARY}
		${SWRESAMPLE_LIBRARY}
		${SWSCALE_LIBRARY}
		${SDL_LIBRARY}
		${OPENAL_LIBRARY}

)


