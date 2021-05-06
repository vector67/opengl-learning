# Make an automatic library - will be static or dynamic based on user setting
add_library(marchingCubes CIsoSurface.h CIsoSurface.cpp Vectors.h Vectors.cpp)
# All users of this library will need at least C++11
target_compile_features(marchingCubes PUBLIC cxx_std_11)
