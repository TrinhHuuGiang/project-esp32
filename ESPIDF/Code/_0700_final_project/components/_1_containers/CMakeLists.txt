# idf_component_register(
#   SRCS              # List of C/C++ source files to compile into object files (.o)
#   INCLUDE_DIRS      # List of directories where header files (.h) are located help compiler comple
#   REQUIRES          # List of other components folder was precompiled (.o, .a) this one depends on
# )

idf_component_register(
    SRCS
    "container.c"
    INCLUDE_DIRS
    "."
    REQUIRES
    # reference libs
    _1_reference_libs
    )
