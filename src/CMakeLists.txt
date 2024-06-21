add_application(
  BombPatcher
  TYPE "APPL"
  CREATOR "ÆDS@"
  bombpatcher.c
  bombs.r
  bundle.r
  dialog.r
)

set_target_properties(BombPatcher PROPERTIES COMPILE_OPTIONS -ffunction-sections)
if(CMAKE_SYSTEM_NAME MATCHES Retro68)
  set_target_properties(BombPatcher PROPERTIES LINK_FLAGS "-Wl,-gc-sections -Wl,--mac-single")
else()
  set_target_properties(BombPatcher PROPERTIES LINK_FLAGS "-Wl,-gc-sections")
endif()
