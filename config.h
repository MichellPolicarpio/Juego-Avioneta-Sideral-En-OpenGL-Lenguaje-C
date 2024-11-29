#ifndef CONFIG_H
#define CONFIG_H

// Silenciar warnings de deprecación de OpenGL en macOS
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

// Includes comunes de OpenGL
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#endif // CONFIG_H