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
#include <sofa/helper/system/SetDirectory.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <direct.h>
#endif
#if defined (__APPLE__)
#include <sys/param.h>
#include <mach-o/dyld.h>
#endif
#include <string.h>
#include <iostream>

namespace sofa
{

namespace helper
{

namespace system
{

SetDirectory::SetDirectory(const char* filename)
{
    directory = GetParentDir(filename);
    if (!directory.empty())
    {
        //std::cout << ">chdir("<<directory<<")"<<std::endl;
        previousDir = GetCurrentDir();
#ifndef WIN32
        chdir(directory.c_str());
#else
        _chdir(directory.c_str());
#endif
    }
}

SetDirectory::SetDirectory(const std::string& filename)
{
    directory = GetParentDir(filename.c_str());
    if (!directory.empty())
    {
        //std::cout << ">chdir("<<directory<<")"<<std::endl;
        previousDir = GetCurrentDir();
#ifndef WIN32
        chdir(directory.c_str());
#else
        _chdir(directory.c_str());
#endif
    }
}

SetDirectory::~SetDirectory()
{
    if (!directory.empty() && !previousDir.empty())
    {
        //std::cout << "<chdir("<<previousDir<<")"<<std::endl;
#ifndef WIN32
        chdir(previousDir.c_str());
#else
        _chdir(previousDir.c_str());
#endif
    }
}

/// Return true if the given file has an absolute path
bool SetDirectory::IsAbsolute(const std::string& filename)
{
    if (filename.empty()) return false;
    if (filename[0] == '/' || filename[0] == '\\') return true;
#ifdef WIN32
    if (filename.length() >= 2 && ((filename[0]>='a' && filename[0]<='z') || (filename[0]>='A' && filename[0]<='Z')) && filename[1]==':') return true;
#endif
    return false;
}

/// Get the current directory
std::string SetDirectory::GetCurrentDir()
{
    char dir[1024];
    memset(dir,0,sizeof(dir));
#ifndef WIN32
    getcwd(dir, sizeof(dir));
#else
    _getcwd(dir, sizeof(dir));
#endif
    return dir;
}

std::string SetDirectory::GetParentDir(const char* filename)
{
    std::string s = filename;
    std::string::size_type pos = s.find_last_of("/\\");
    if (pos == std::string::npos)
        return ""; // no directory
    else
        return s.substr(0,pos);
}

std::string SetDirectory::GetFileName(const char* filename)
{
    std::string s = filename;
    std::string::size_type pos = s.find_last_of("/\\");
    if (pos == std::string::npos)
        return s; // no directory
    else
        return s.substr(pos+1);
}

std::string SetDirectory::GetExtension(const char* filename)
{
    std::string s = filename;
    std::string::size_type pos = s.find_last_of('.');
    if (pos == std::string::npos)
        return ""; // no extension
    else
        return s.substr(pos+1);
}

std::string SetDirectory::GetRelativeFromDir(const char* filename, const char* basename)
{
    if (!filename || !filename[0]) return "";
    if (IsAbsolute(filename)) return filename; // absolute path
    std::string base = basename;
    std::string s = filename;
    // remove any ".."
    while ((s.substr(0,3)=="../" || s.substr(0,3)=="..\\") && !base.empty())
    {
        s = s.substr(3);
        base = GetParentDir(base.c_str());
    }
    if (base.empty())
        return s;
    else if (base[base.length()-1] == '/')
        return base + s;
    else
        return base + "/" + s;
}

std::string SetDirectory::GetRelativeFromFile(const char* filename, const char* basename)
{
    std::string base = GetParentDir(basename);
    return GetRelativeFromDir(filename, base.c_str());
}

std::string SetDirectory::GetRelativeFromProcess(const char* filename, const char* basename)
{
    std::string base = GetProcessFullPath(basename);
    return GetRelativeFromFile(filename, base.c_str());
}

/// Get the full path of the current process. The given filename should be the value of argv[0].
std::string SetDirectory::GetProcessFullPath(const char* filename)
{
#if defined (WIN32)
    if (!filename || !filename[0])
    {
        //return __argv[0];
        int n=0;
        LPWSTR wpath = *CommandLineToArgvW(GetCommandLineW(),&n);
        if (wpath)
        {
            char path[1024];
            memset(path,0,sizeof(path));
            wcstombs(path, wpath, sizeof(path)-1);
            //std::cout << "Current process: "<<path<<std::endl;
            if (path[0]) return path;
        }
    }
    /// \TODO use GetCommandLineW and/or CommandLineToArgvW. This is however not strictly necessary, as argv[0] already contains the full path in most cases.
#elif defined (__linux__)
    if (!filename || filename[0]!='/')
    {
        char path[1024];
        memset(path,0,sizeof(path));
        readlink("/proc/self/exe",path,sizeof(path)-1);
// 		std::cout << "Current process: "<< path <<std::endl;
        if (path[0])
            return path;
        else
            std::cout << "ERROR: can't get current process path..." << std::endl;
    }
#elif defined (__APPLE__)
    if (!filename || filename[0]!='/')
    {
        char path[1024];
        unsigned int size;
        _NSGetExecutablePath( path, &size );
// 		std::cout << "Current process path: "<<path<<std::endl;

        return path;
    }
#endif

    return filename;
}

} // namespace system

} // namespace helper

} // namespace sofa

