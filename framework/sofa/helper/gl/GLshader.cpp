/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
//*******************************************************************//
//	OpenGL Shader class 											 //
//                                                                   //
//	Based on code from Ben Humphrey	/ digiben@gametutorilas.com		 //
//																	 //
//*******************************************************************//

#include <sofa/helper/gl/GLshader.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#if !defined(_WIN32) && !defined(__APPLE__)
#  include <GL/glx.h>
#endif
#ifdef __APPLE__
//CHANGE(Jeremie A.): NS* methods to access symbols are deprecated in favor of standard dl* methods
//#include "mach-o/dyld.h"
#include <dlfcn.h>
#endif


namespace sofa
{

namespace helper
{

namespace gl
{

// The function pointers for shaders
PFNGLCREATESHADEROBJECTARBPROC	glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC		glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC		glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC		glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC			glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC	glUseProgramObjectARB = NULL;
PFNGLUNIFORM1IARBPROC			glUniform1iARB = NULL;
PFNGLUNIFORM1FARBPROC			glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC			glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC			glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC			glUniform4fARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC	glGetUniformLocationARB = NULL;
PFNGLDETACHOBJECTARBPROC		glDetachObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC		glDeleteObjectARB = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB = NULL;
PFNGLGETOBJECTPARAMETERIV		glGetObjectParameterivARB = NULL;
PFNGLGETINFOLOGARBPROC			glGetInfoLogARB = NULL;


///////////////////////////////////// INIT GLSL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes all of our GLSL functions and checks availability.
/////
///////////////////////////////////// INIT GLSL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

#ifdef __APPLE__
//CHANGE(Jeremie A.): NS* methods to access symbols are deprecated in favor of standard dl* methods
#if 0
void *NSGLGetProcAddress(const char *name)
{
    NSSymbol symbol;

    /* Prepend a '_' for the Unix C symbol mangling convention */
    char* symbolName = (char*)malloc(strlen(name) + 2);
    if (!symbolName)
    {
        fprintf(stderr, "Failed to allocate memory for NSGLGetProcAddress\n");
        return NULL;
    }
    symbolName[0] = '_';
    strcpy(symbolName + 1, name);

    if (!NSIsSymbolNameDefined(symbolName))
    {
        free(symbolName);
        return NULL;
    }

    symbol = NSLookupAndBindSymbol(symbolName);
    free(symbolName);
    if (!symbol)
    {
        return NULL;
    }

    return NSAddressOfSymbol(symbol);
}
#endif
#endif

#if defined (WIN32)
PROC glewGetProcAddress(const char* name)
#elif defined(__APPLE__)
void (*glewGetProcAddress(const char* name))
#else
void (*glewGetProcAddress(const char* name))(void)
#endif
{
#if defined(_WIN32)
    return wglGetProcAddress((LPCSTR)name);
#elif defined(__APPLE__)
//CHANGE(Jeremie A.): NS* methods to access symbols are deprecated in favor of standard dl* methods
//    return NSGLGetProcAddress(name);
    return dlsym(RTLD_DEFAULT, name);
#elif defined(__sgi) || defined(__sun)
    return dlGetProcAddress(name);
#else /* __linux */
    return (*glXGetProcAddressARB)((const GLubyte*)name);
#endif
}

bool CShader::InitGLSL()
{
    // This grabs a list of all the video card's extensions it supports
    char *szGLExtensions = (char*)glGetString(GL_EXTENSIONS);

    // Make sure find the GL_ARB_shader_objects extension so we can use shaders.
    if(!strstr(szGLExtensions, "GL_ARB_shader_objects"))
    {
        printf("GL_ARB_shader_objects extension not supported!\n");
        return false;
    }

    // Make sure we support the GLSL shading language 1.0
    if(!strstr(szGLExtensions, "GL_ARB_shading_language_100"))
    {
        printf("GL_ARB_shading_language_100 extension not supported!\n");
        return false;
    }

    // Now let's set all of our function pointers for our extension functions
    glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)glewGetProcAddress("glCreateShaderObjectARB");
    glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)glewGetProcAddress("glShaderSourceARB");
    glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)glewGetProcAddress("glCompileShaderARB");
    glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)glewGetProcAddress("glCreateProgramObjectARB");
    glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)glewGetProcAddress("glAttachObjectARB");
    glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)glewGetProcAddress("glLinkProgramARB");
    glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)glewGetProcAddress("glUseProgramObjectARB");
    glUniform1iARB = (PFNGLUNIFORM1IARBPROC)glewGetProcAddress("glUniform1iARB");
    glUniform1fARB = (PFNGLUNIFORM1FARBPROC)glewGetProcAddress("glUniform1fARB");
    glUniform2fARB = (PFNGLUNIFORM2FARBPROC)glewGetProcAddress("glUniform2fARB");
    glUniform3fARB = (PFNGLUNIFORM3FARBPROC)glewGetProcAddress("glUniform3fARB");
    glUniform4fARB = (PFNGLUNIFORM4FARBPROC)glewGetProcAddress("glUniform4fARB");
    glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)glewGetProcAddress("glGetUniformLocationARB");
    glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)glewGetProcAddress("glDetachObjectARB");
    glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)glewGetProcAddress("glDeleteObjectARB");
    glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glewGetProcAddress("glActiveTextureARB");
    glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)glewGetProcAddress("glMultiTexCoord2fARB");
    glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIV)glewGetProcAddress("glGetObjectParameterivARB");
    glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)glewGetProcAddress("glGetInfoLogARB");

    // Return a success!
    return true;
}

CShader::CShader()
{
    m_hVertexShader = 0; //NULL;
    m_hFragmentShader = 0; //NULL;
    m_hProgramObject = 0; //NULL;
}

CShader::~CShader()
{
    // BUGFIX: if the GL context is gone, this can crash the application on exit -- Jeremie A.
    //Release();
}

///////////////////////////////// LOAD TEXT FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads and returns a text file for our shaders
/////
///////////////////////////////// LOAD TEXT FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

std::string CShader::LoadTextFile(std::string strFile)
{
    // Open the file passed in
    std::ifstream fin(strFile.c_str());

    // Make sure we opened the file correctly
    if(!fin)
        return "";

    std::string strLine = "";
    std::string strText = "";

    // Go through and store each line in the text file within a "string" object
    while(std::getline(fin, strLine))
    {
        strText = strText + "\n" + strLine;
    }

    // Close our file
    fin.close();

    // Return the text file's data
    return strText;
}


///////////////////////////////// INIT SHADERS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads a vertex and fragment shader file
/////
///////////////////////////////// INIT SHADERS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShader::InitShaders(std::string strVertex, std::string strFragment)
{
    // These will hold the shader's text file data
    std::string strVShader, strFShader;

    // Make sure the user passed in a vertex and fragment shader file
    if(!strVertex.length() || !strFragment.length())
        return;

    // If any of our shader pointers are set, let's free them first.
    if(m_hVertexShader || m_hFragmentShader || m_hProgramObject)
        Release();

    // Here we get a pointer to our vertex and fragment shaders
    m_hVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    m_hFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    // Now we load the shaders from the respective files and store it in a string.
    strVShader = LoadTextFile(strVertex.c_str());
    strFShader = LoadTextFile(strFragment.c_str());

    // Do a quick switch so we can do a double pointer below
    const char *szVShader = strVShader.c_str();
    const char *szFShader = strFShader.c_str();

    // Now this assigns the shader text file to each shader pointer
    glShaderSourceARB(m_hVertexShader, 1, &szVShader, NULL);
    glShaderSourceARB(m_hFragmentShader, 1, &szFShader, NULL);

    // Now we actually compile the shader's code
    glCompileShaderARB(m_hVertexShader);
    glCompileShaderARB(m_hFragmentShader);

    // Next we create a program object to represent our shaders
    m_hProgramObject = glCreateProgramObjectARB();

    // We attach each shader we just loaded to our program object
    glAttachObjectARB(m_hProgramObject, m_hVertexShader);
    glAttachObjectARB(m_hProgramObject, m_hFragmentShader);

    // Our last init function is to link our program object with OpenGL
    glLinkProgramARB(m_hProgramObject);

    GLint logLength = 0;
    glGetObjectParameterivARB(m_hProgramObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength);

    if (logLength > 1)
    {
        GLcharARB *szLog = (GLcharARB*)malloc(logLength+1);
        GLint writtenLength = 0;

        glGetInfoLogARB(m_hProgramObject, logLength, &writtenLength, szLog);
        printf("ERROR during shader initialization: %s\n", szLog);

        free(szLog);
    }

    // Now, let's turn off the shader initially.
    glUseProgramObjectARB(0);
}

void CShader::SetInt(GLint variable, int newValue)                              { if (variable!=-1) glUniform1iARB(variable, newValue);       }
void CShader::SetFloat(GLint variable, float newValue)                          { if (variable!=-1) glUniform1fARB(variable, newValue);       }
void CShader::SetFloat2(GLint variable, float v0, float v1)                     { if (variable!=-1) glUniform2fARB(variable, v0, v1);         }
void CShader::SetFloat3(GLint variable, float v0, float v1, float v2)           { if (variable!=-1) glUniform3fARB(variable, v0, v1, v2);     }
void CShader::SetFloat4(GLint variable, float v0, float v1, float v2, float v3) { if (variable!=-1) glUniform4fARB(variable, v0, v1, v2, v3); }

// These 2 functions turn on and off our shader
void CShader::TurnOn()	{ if (m_hProgramObject) glUseProgramObjectARB(m_hProgramObject); }
void CShader::TurnOff()	{ if (m_hProgramObject) glUseProgramObjectARB(0);                }

///////////////////////////////// GET VARIABLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns a variable ID for a shader variable
/////
///////////////////////////////// GET VARIABLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

GLint CShader::GetVariable(std::string strVariable)
{
    // If we don't have an active program object, let's return -1
    if(!m_hProgramObject)
        return -1;

    // This returns the variable ID for a variable that is used to find
    // the address of that variable in memory.
    return glGetUniformLocationARB(m_hProgramObject, strVariable.c_str());
}


///////////////////////////////// RELEASE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function frees all of our shader data
/////
///////////////////////////////// RELEASE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShader::Release()
{
    // If our vertex shader pointer is valid, free it
    if(m_hVertexShader)
    {
        glDetachObjectARB(m_hProgramObject, m_hVertexShader);
        glDeleteObjectARB(m_hVertexShader);
        m_hVertexShader = 0; //NULL;
    }

    // If our fragment shader pointer is valid, free it
    if(m_hFragmentShader)
    {
        glDetachObjectARB(m_hProgramObject, m_hFragmentShader);
        glDeleteObjectARB(m_hFragmentShader);
        m_hFragmentShader = 0; //NULL;
    }

    // If our program object pointer is valid, free it
    if(m_hProgramObject)
    {
        glDeleteObjectARB(m_hProgramObject);
        m_hProgramObject = 0; //NULL;
    }
}

} // namespace gl

} // namespace helper

} // namespace sofa

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES *
//
// Nothing new was added to this file for this tutorial.
//
//
// (C) 2000-2005 GameTutorials
