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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_CONSTRAINT_FIXEDTRANSLATIONCONSTRAINT_H
#define SOFA_COMPONENT_CONSTRAINT_FIXEDTRANSLATIONCONSTRAINT_H

#include <sofa/core/behavior/Constraint.h>
#include <sofa/component/topology/PointSubset.h>
#include <sofa/helper/vector.h>

namespace sofa
{

namespace component
{

namespace constraint
{

/// This class can be overridden if needed for additionnal storage within template specializations.
template <class DataTypes>
class FixedTranslationConstraintInternalData
{
};

/** Attach given particles to their initial positions.
*/
template <class DataTypes>
class FixedTranslationConstraint : public core::behavior::Constraint<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(FixedTranslationConstraint,DataTypes),SOFA_TEMPLATE(sofa::core::behavior::Constraint, DataTypes));

    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef topology::PointSubset SetIndex;
    typedef sofa::helper::vector<unsigned int> SetIndexArray;

protected:
    FixedTranslationConstraintInternalData<DataTypes> data;
    friend class FixedTranslationConstraintInternalData<DataTypes>;

public:
    Data<SetIndex> f_indices;
    Data<bool> f_fixAll;
    Data<double> _drawSize;
    Data<SetIndex> f_coordinates;

    FixedTranslationConstraint();

    virtual ~FixedTranslationConstraint();

    // methods to add/remove some indices
    void clearIndices();
    void addIndex(unsigned int index);
    void removeIndex(unsigned int index);

    // -- Constraint interface
    void init();
    virtual void projectResponse(VecDeriv& dx);
    virtual void projectVelocity(VecDeriv& /*v*/) {}; ///< project v to constrained space (v models a velocity)
    virtual void projectPosition(VecCoord& /*x*/) {}; ///< project x to constrained space (x models a position)

    // Handle topological changes
    virtual void handleTopologyChange();

    virtual void draw();

    /// this constraint is holonomic
    bool isHolonomic() {return true;}

protected:

    sofa::core::topology::BaseMeshTopology* topology;

    // Define TestNewPointFunction
    static bool FCTestNewPointFunction(int, void*, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >& );

    // Define RemovalFunction
    static void FCRemovalFunction ( int , void*);

};

#if defined(WIN32) && !defined(SOFA_COMPONENT_CONSTRAINT_FIXEDTRANSLATIONCONSTRAINT_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_COMPONENT_CONSTRAINT_API FixedTranslationConstraint<defaulttype::Vec3dTypes>;
extern template class SOFA_COMPONENT_CONSTRAINT_API FixedTranslationConstraint<defaulttype::Rigid3dTypes>;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_COMPONENT_CONSTRAINT_API FixedTranslationConstraint<defaulttype::Vec3fTypes>;
extern template class SOFA_COMPONENT_CONSTRAINT_API FixedTranslationConstraint<defaulttype::Rigid3fTypes>;
#endif
#endif

} // namespace constraint

} // namespace component

} // namespace sofa


#endif
