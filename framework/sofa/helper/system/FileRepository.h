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
#ifndef SOFA_HELPER_SYSTEM_FILEREPOSITORY_H
#define SOFA_HELPER_SYSTEM_FILEREPOSITORY_H

#include <string>
#include <vector>

namespace sofa
{

namespace helper
{

namespace system
{

/// Helper class to find files in a list of directories.
///
/// Each file is searched as follow:
///
/// 1: Using the specified filename in current directory, or in the specified directory.
/// If the filename does not start with "/", "./", or "../" :
/// 2: In the directory path specified using addFirstPath method.
/// 3: In the directory path specified using an environment variable (default to SOFA_DATA_PATH).
/// 4: In the default directories relative to the main executable (default to ../share).
/// 5: In the directory path specified using addLastPath method.
///
/// For file name starting with '/', './' or '../' only the first step is used.
///
/// A path is considered as a concatenation of directories separated by :
// A small utility class to temporarly set the current directory to the same as a specified file
class FileRepository
{
public:

    /// Initialize the set of paths from an environment variable.
    FileRepository(const char* envVar = "SOFA_DATA_PATH", const char* relativePath = "../share" );

    ~FileRepository();

    /// Adds a path to the front of the set of paths.
    void addFirstPath(const std::string& path);

    /// Adds a path to the back of the set of paths.
    void addLastPath(const std::string& path);

    /// Get the first path into the set of paths
    std::string getFirstPath();

    /// Find file using the stored set of paths.
    /// @param basedir override current directory (optional)
    /// @param filename requested file as input, resolved file path as output
    /// @return true if the file was found in one of the directories, false otherwise
    bool findFile(std::string& filename, const std::string& basedir="");

    /// Alias for findFile, but returning the resolved file as the result.
    /// Less informative for errors, but sometimes easier to use
    std::string getFile(std::string filename, const std::string& basedir="")
    {
        findFile(filename, basedir);
        return filename;
    }

    /// Find file using the stored set of paths.
    /// @param basefile override current directory by using the parent directory of the given file
    /// @param filename requested file as input, resolved file path as output
    /// @return true if the file was found in one of the directories, false otherwise
    bool findFileFromFile(std::string& filename, const std::string& basefile);

    /// Print the list of path to std::cout
    void print();

    /// OS-dependant character separing entries in list of paths.
    static char entrySeparator()
    {
#ifdef WIN32
        return ';';
#else
        return ':';
#endif
    }

protected:

    /// Vector of paths.
    std::vector<std::string> vpath;

    /// Search file in a given path.
    static bool findFileIn(std::string& filename, const std::string& path);
};

extern FileRepository DataRepository; ///< Default repository

} // namespace system

} // namespace helper

} // namespace sofa

#endif
