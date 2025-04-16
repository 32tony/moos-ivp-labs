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

