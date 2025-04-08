#--------------------------------------------------------
# The CMakeLists.txt for:                       pUnderwatercomms
# Author(s):                              sy
#--------------------------------------------------------

SET(SRC
  ProtocolHandler.cpp
  Underwatercomms.cpp
  Underwatercomms_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pUnderwatercomms ${SRC})

TARGET_LINK_LIBRARIES(pUnderwatercomms
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

   find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(pUnderwatercomms
    ${Boost_LIBRARIES}
    apputil
    mbutil
    m
    pthread
)
