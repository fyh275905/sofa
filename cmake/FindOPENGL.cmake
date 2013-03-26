# This module defines the following variables:
#  OPENGL_INCLUDE_DIRS - include directories for OPENGL
#  OPENGL_LIBRARIES - libraries to link against OPENGL
#  OPENGL_FOUND - true if OPENGL has been found and can be used

if (NOT DEFINED OPENGL_FOUND)

	if(WIN32)
		set(OPENGL_LIBRARIES "opengl32")
		set(OPENGL_FOUND CACHE INTERNAL TRUE)
	endif(WIN32)

	if(UNIX)
		find_path(OPENGL_INCLUDE_DIR GL/gl.h)
		#find_library(OPENGL_gl_LIBRARY NAMES opengl32 OpenGL GL MesaGL)
		#find_library(OPENGL_glu_LIBRARY NAMES opengl32 OpenGL GL MesaGL)
		find_library(OPENGL_LIBRARIES NAMES opengl32 OpenGL GL MesaGL)

		if(EXISTS "${OPENGL_INCLUDE_DIR}/GL/glew.h" AND EXISTS ${OPENGL_LIBRARIES})
			#message(STATUS "OPENGL Lib found: ${OPENGL_LIBRARIES} ${OPENGL_INCLUDE_DIR}")
			set(OPENGL_FOUND CACHE INTERNAL TRUE)
		else()
			message(FATAL_ERROR "OPENGL NOT FOUND ${OPENGL_LIBRARIES} ${OPENGL_INCLUDE_DIR}")
			set(OPENGL_FOUND FALSE)
		endif()

	endif(UNIX)



endif(NOT DEFINED OPENGL_FOUND)

mark_as_advanced(OPENGL_INCLUDE_DIR OPENGL_LIBRARIES)
