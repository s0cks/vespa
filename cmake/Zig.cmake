
function(add_zig_target name sources)
  set(OUTPUT_BINARY "${CMAKE_CURRENT_BINARY_DIR}/${name}")
  add_custom_command(
    OUTPUT ${OUTPUT_BINARY}
    COMMAND zig build-exe ${sources} --name ${name}
    DEPENDS ${sources}
    COMMENT "compiling zig project"
  )

  add_custom_target(zig-${name} ALL DEPENDS ${OUTPUT_BINARY})
endfunction()
