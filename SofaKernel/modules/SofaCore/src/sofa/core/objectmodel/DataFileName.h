/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_OBJECTMODEL_DATAFILENAME_H
#define SOFA_CORE_OBJECTMODEL_DATAFILENAME_H

#include <sofa/core/objectmodel/Data.h>
#include <sofa/helper/SVector.h>

namespace sofa
{

namespace core
{

namespace objectmodel
{

enum class PathType {
    FILE,
    DIRECTORY,
    BOTH
};

/**
 *  \brief Data specialized to store filenames, potentially relative to the current directory at the time it was specified.
 *
 */
class SOFA_CORE_API DataFileName : public sofa::core::objectmodel::Data<std::string>
{
public:


    typedef sofa::core::objectmodel::Data<std::string> Inherit;

    DataFileName( const std::string& helpMsg="", bool isDisplayed=true, bool isReadOnly=false )
        : Inherit(helpMsg, isDisplayed, isReadOnly),
          m_pathType(PathType::FILE)
    {
    }

    DataFileName( const std::string& value, const std::string& helpMsg="", bool isDisplayed=true, bool isReadOnly=false )
        : Inherit(value, helpMsg, isDisplayed, isReadOnly),
        m_pathType(PathType::FILE)
    {
        updatePath();
    }

    /** Constructor
        this constructor should be used through the initData() methods
     */
    explicit DataFileName(const BaseData::BaseInitData& init)
        : Inherit(init),
          m_pathType(PathType::FILE)
    {
    }

    /** Constructor
        this constructor should be used through the initData() methods
     */
    explicit DataFileName(const Inherit::InitData& init)
        : Inherit(init),
          m_pathType(PathType::FILE)
    {
        updatePath();
    }

    virtual ~DataFileName()
    {
    }


    void setPathType(PathType pathType)
    {
        m_pathType = pathType;
    }

    PathType getPathType()
    {
        return m_pathType;
    }

    bool read(const std::string& s ) override;

    void endEdit() override
    {
        updatePath();
        Data::notifyEndEdit();
    }


    virtual const std::string& getRelativePath() const
    {
        this->updateIfDirty();
        return m_relativepath ;
    }

    virtual const std::string& getFullPath() const
    {
        this->updateIfDirty();
        return m_fullpath;
    }
    virtual const std::string& getAbsolutePath() const
    {
        this->updateIfDirty();
        return m_fullpath;
    }
    virtual const std::string& getExtension() const
    {
        this->updateIfDirty();
        return m_extension;
    }

    void doOnUpdate() override
    {
        updatePath();
    }

protected:
    void updatePath();

    std::string m_fullpath;
    std::string m_relativepath;
    std::string m_extension;
    PathType    m_pathType; //< used to determine how file dialogs should be opened

private:
    DataFileName(const Inherit& d);
    DataFileName& operator=(const DataFileName&);
};



class SOFA_CORE_API DataFileNameVector : public sofa::core::objectmodel::Data< sofa::helper::SVector<std::string> >
{
public:
    typedef sofa::core::objectmodel::Data<sofa::helper::SVector<std::string> > Inherit;

    DataFileNameVector( const char* helpMsg=nullptr, bool isDisplayed=true, bool isReadOnly=false)
        : Inherit(helpMsg, isDisplayed, isReadOnly),
          m_pathType(PathType::FILE)
    {
    }

    DataFileNameVector( const sofa::helper::vector<std::string>& value, const char* helpMsg=nullptr, bool isDisplayed=true, bool isReadOnly=false )
        : Inherit(value, helpMsg, isDisplayed, isReadOnly),
          m_pathType(PathType::FILE)
    {
        updatePath();
    }

    /** Constructor
        this constructor should be used through the initData() methods
     */
    explicit DataFileNameVector(const BaseData::BaseInitData& init)
        : Inherit(init),
          m_pathType(PathType::FILE)
    {
    }

    /** Constructor
        this constructor should be used through the initData() methods
     */
    explicit DataFileNameVector(const Inherit::InitData& init)
        : Inherit(init),
          m_pathType(PathType::FILE)
    {
        updatePath();
    }

    ~DataFileNameVector() override;

    void endEdit() override
    {
        updatePath();
        Inherit::endEdit();
    }

    void addPath(const std::string& v, bool clear = false)
    {
        sofa::helper::vector<std::string>& val = *beginEdit();
        if(clear) val.clear();
        val.push_back(v);
        endEdit();
    }
    void setValueAsString(const std::string& v)
    {
        sofa::helper::SVector<std::string>& val = *beginEdit();
        val.clear();
        std::istringstream ss( v );
        ss >> val;
        endEdit();
    }

    bool read(const std::string& s ) override
    {
        bool ret = Inherit::read(s);
        if (ret || m_fullpath.empty()) updatePath();
        return ret;
    }

    virtual const std::string& getRelativePath(unsigned int i) { return getValue()[i]; }
    virtual const std::string& getFullPath(unsigned int i) const
    {
        this->updateIfDirty();
        return m_fullpath[i];
    }
    virtual const std::string& getAbsolutePath(unsigned int i) const
    {
        this->updateIfDirty();
        return m_fullpath[i];
    }

    void doOnUpdate() override
    {
        this->updatePath();
    }

    void setPathType(PathType pathType)
    {
        m_pathType = pathType;
    }

    PathType getPathType()
    {
        return m_pathType;
    }

protected:
    void updatePath();

    sofa::helper::vector<std::string> m_fullpath;
    PathType m_pathType; //< used to determine how file dialogs should be opened

private:
    DataFileNameVector(const Inherit& d);
    DataFileNameVector& operator=(const DataFileNameVector&);
};

} // namespace objectmodel

} // namespace core

} // namespace sofa

#endif
