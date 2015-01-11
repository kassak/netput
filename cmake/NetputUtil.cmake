cmake_minimum_required(VERSION 2.8.11)

function(copy_target_to_dir trg_from trg_to dir when)
   add_custom_command(TARGET ${trg_to} ${when}
      DEPENDS ${trg_from} ${trg_to}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
         "$<TARGET_FILE:${trg_from}>"
         "${dir}"
   )
endfunction(copy_target_to_dir)

function(copy_target_to_dir2 trg_from trg_to dir when)
   add_custom_command(TARGET ${trg_to} ${when}
      DEPENDS ${trg_from} ${trg_to}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
         "$<TARGET_PROPERTY:${trg_from},TARGET_FILE>"
         "${dir}"
   )
endfunction(copy_target_to_dir2)

function(copy_target_to_target_dir trg_from trg_to when)
   copy_target_to_dir("${trg_from}" "${trg_to}" "$<TARGET_FILE_DIR:${trg_to}>" "${when}")
endfunction(copy_target_to_target_dir)

function(netput_install_default)
   install(TARGETS ${ARGN}
      RUNTIME DESTINATION bin
      LIBRARY DESTINATION lib
      ARCHIVE DESTINATION lib/static
   )
endfunction(netput_install_default)

function(netput_install_files type)
   if("${type}" STREQUAL "PROG")
      set(out_type "PROGRAMS")
      set(out_dir "bin")
   elseif("${type}" STREQUAL "BIN")
      set(out_type "FILES")
      set(out_dir "bin")
   elseif("${type}" STREQUAL "LIB")
      set(out_type "FILES")
      set(out_dir "lib")
   elseif("${type}" STREQUAL "SLIB")
      set(out_type "FILES")
      set(out_dir "lib/static")
   endif()
   install(${out_type} ${ARGN} DESTINATION ${out_dir})
endfunction(netput_install_files)
