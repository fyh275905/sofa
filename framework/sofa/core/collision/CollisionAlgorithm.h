/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_CORE_COLLISION_COLLISIONALGORITHM_H
#define SOFA_CORE_COLLISION_COLLISIONALGORITHM_H

#include <sofa/core/objectmodel/BaseObject.h>
#include <vector>
#include <map>
#include <algorithm>

namespace sofa
{

namespace core
{

namespace collision
{

class CollisionAlgorithm : public virtual objectmodel::BaseObject
{
public:
    SOFA_CLASS(CollisionAlgorithm, objectmodel::BaseObject);

    /// Collision 'pipeline' instance, allowing to store multiple internal states
    typedef void* Instance;
protected:
    /// Current collision 'pipeline' instance, allowing to store multiple internal states
    Instance instance;
public:

    CollisionAlgorithm()
        : instance(NULL)
    {
    }

    /// Allow to store multiple internal states by specifying a different collision pipeline 'instance'
    virtual void setInstance(Instance inst)
    {
        if (instance == inst) return;
        changeInstance(inst);
        instance = inst;
    }

protected:
    virtual void changeInstance(Instance)=0;
};

} // namespace collision

} // namespace core

} // namespace sofa

#endif
