/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_TOPOLOGY_POINTSETGEOMETRYALGORITHMS_H
#define SOFA_COMPONENT_TOPOLOGY_POINTSETGEOMETRYALGORITHMS_H

#include <sofa/core/componentmodel/topology/BaseTopology.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>

namespace sofa
{
namespace core
{
namespace componentmodel
{
namespace behavior
{
template <class DataTypes>
class MechanicalState;
}
}
}

namespace component
{

namespace topology
{
using core::componentmodel::topology::BaseMeshTopology;
using core::componentmodel::behavior::MechanicalState;
typedef BaseMeshTopology::PointID PointID;

/**
* A class that can perform some geometric computation on a set of points.
*/
template<class DataTypes>
class PointSetGeometryAlgorithms : public core::componentmodel::topology::GeometryAlgorithms
{
public:
    typedef typename DataTypes::Real Real;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::VecCoord VecCoord;

    PointSetGeometryAlgorithms()
        : GeometryAlgorithms()
    {}

    virtual ~PointSetGeometryAlgorithms() {}

    virtual void init();

    /** return the centroid of the set of points */
    Coord getPointSetCenter() const;

    /** return the centre and a radius of a sphere enclosing the  set of points (may not be the smalled one) */
    void getEnclosingSphere(Coord &center, Real &radius) const;

    /** return the axis aligned bounding box : index 0 = xmin, index 1=ymin,
    index 2 = zmin, index 3 = xmax, index 4 = ymax, index 5=zmax */
    void getAABB(Real bb[6]) const;

    /** \brief Returns the object where the mechanical DOFs are stored */
    sofa::core::componentmodel::behavior::MechanicalState<DataTypes> *getDOF() const { return object;	}

    virtual std::string getTemplateName() const
    {
        return templateName(this);
    }

    static std::string templateName(const PointSetGeometryAlgorithms<DataTypes>* = NULL)
    {
        return DataTypes::Name();
    }
protected:
    /** the object where the mechanical DOFs are stored */
    sofa::core::componentmodel::behavior::MechanicalState<DataTypes> *object;
    sofa::core::componentmodel::topology::BaseMeshTopology* m_topology;
};

} // namespace topology

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENTS_POINTSETGEOMETRYALGORITHMS_H
