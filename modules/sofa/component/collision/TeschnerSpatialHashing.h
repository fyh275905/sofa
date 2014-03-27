/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_COLLISION_TESCHNERSPATIALHASHING_H
#define SOFA_COMPONENT_COLLISION_TESCHNERSPATIALHASHING_H

#include <sofa/core/collision/BroadPhaseDetection.h>
#include <sofa/core/collision/NarrowPhaseDetection.h>
#include <sofa/core/CollisionElement.h>
#include <sofa/component/component.h>
#include <sofa/component/collision/CubeModel.h>
#include <sofa/defaulttype/Vec.h>
#include <set>
#include <boost/unordered_map.hpp>
#include <sofa/core/CollisionModel.h>
#include <sofa/component/collision/TeschnerHashTable.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/core/behavior/BaseMechanicalState.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/simulation/common/ResetVisitor.h>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

class SOFA_BASE_COLLISION_API TeschnerSpatialHashing :
    public core::collision::BroadPhaseDetection,
    public core::collision::NarrowPhaseDetection
{
public:
    SOFA_CLASS2(TeschnerSpatialHashing, core::collision::BroadPhaseDetection, core::collision::NarrowPhaseDetection);

private:
    template <class DataTypes>
    void sumEdgeLength_template(core::CollisionModel *cm);
protected:
    TeschnerSpatialHashing();

    virtual ~TeschnerSpatialHashing(){}

    virtual bool keepCollisionBetween(core::CollisionModel *cm1, core::CollisionModel *cm2);

    void sumEdgeLength(core::CollisionModel *cm);

    //boost::unordered_map<std::pair<core::CollisionModel*,core::CollisionModel*>,CollidingPair> _qsdf;

    sofa::helper::vector<sofa::component::collision::CubeModel*> cubeModels;//AABBs containing the final collision model
    TeschnerHashTable _grid;
    SReal _timeStamp;
    SReal _cell_size;
    bool _params_initialized;
    SReal _nb_edges;
    SReal _nb_elems;
    SReal _total_edges_length;
public:
    inline virtual void beginBroadPhase()
    {
        core::collision::BroadPhaseDetection::beginBroadPhase();
        cubeModels.clear();
        _timeStamp += this->getContext()->getDt();
        //collisionModels.clear();
    }


    void init();
    void reinit();

    void addCollisionModel (core::CollisionModel *cm);

    virtual void endBroadPhase();

    /**
      *Unuseful methods because all is done in addCollisionModel
      */
    void addCollisionPair (const std::pair<core::CollisionModel*, core::CollisionModel*>& ){}
    void addCollisionPairs (std::vector<std::pair<core::CollisionModel*, core::CollisionModel*> >&){}

    virtual void beginNarrowPhase();


    /* for debugging */
    inline void draw(const core::visual::VisualParams*){}

    inline virtual bool needsDeepBoundingTree()const{return false;}
};

} // namespace collision

} // namespace component

} // namespace sofa

#endif
