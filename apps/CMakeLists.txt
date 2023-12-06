macro(add_app exec_name)
  add_executable("${exec_name}" "${exec_name}.cc" "webget_impl.cc")
  target_link_libraries("${exec_name}" minnow_debug)
  target_link_libraries("${exec_name}" util_debug)
endmacro(add_app)

add_app(webget)

# add my test here
add_app(test_webget)
