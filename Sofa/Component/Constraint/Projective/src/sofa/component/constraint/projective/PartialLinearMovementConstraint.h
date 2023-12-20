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
#pragma once
#include <sofa/component/constraint/projective/config.h>

#include <sofa/core/behavior/ProjectiveConstraintSet.h>
#include <sofa/core/behavior/MechanicalState.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/core/objectmodel/Event.h>
#include <sofa/linearalgebra/BaseMatrix.h>
#include <sofa/linearalgebra/BaseVector.h>
#include <sofa/type/vector.h>
#include <sofa/core/topology/TopologySubsetIndices.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <type_traits>
#include <set>

namespace sofa::component::constraint::projective
{

template<class DataTypes>
class PartialLinearMovementConstraintInternalData
{
};

/** impose a motion to given DOFs (translation and rotation) in some directions only.
  The moved and free directioons are the same for all the particles, defined  in the movedDirections attribute.
    The motion between 2 key times is linearly interpolated
*/
template <class TDataTypes>
class PartialLinearMovementConstraint : public core::behavior::ProjectiveConstraintSet<TDataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(PartialLinearMovementConstraint,TDataTypes),SOFA_TEMPLATE(sofa::core::behavior::ProjectiveConstraintSet, TDataTypes));

    using Index = sofa::Index;
    typedef TDataTypes DataTypes;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::MatrixDeriv MatrixDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef typename MatrixDeriv::RowIterator MatrixDerivRowIterator;
    typedef typename MatrixDeriv::RowType MatrixDerivRowType;
    typedef Data<VecCoord> DataVecCoord;
    typedef Data<VecDeriv> DataVecDeriv;
    typedef Data<MatrixDeriv> DataMatrixDeriv;
    typedef type::vector<Index> SetIndexArray;
    typedef sofa::core::topology::TopologySubsetIndices SetIndex;

protected:
    PartialLinearMovementConstraintInternalData<DataTypes> *data;
    friend class PartialLinearMovementConstraintInternalData<DataTypes>;

public :
    /// indices of the DOFs the constraint is applied to
    SetIndex m_indices;
    /// the key frames when the motion is defined by the user
    core::objectmodel::Data<type::vector<Real> > m_keyTimes;
    /// the motions corresponding to the key frames
    core::objectmodel::Data<VecDeriv > m_keyMovements;

    /// attributes to precise display
    /// if showMovement is true we display the expected movement
    /// otherwise we show which are the fixed dofs
    core::objectmodel::Data< bool > showMovement;

    /// the key times surrounding the current simulation time (for interpolation)
    Real prevT, nextT;
    ///the motions corresponding to the surrouding key times
    Deriv prevM, nextM;
    ///initial constrained DOFs position
    VecCoord x0;

    core::objectmodel::Data<bool> linearMovementBetweenNodesInIndices; ///< Take into account the linear movement between the constrained points
    core::objectmodel::Data<unsigned> mainIndice; ///< The main indice node in the list of constrained nodes, it defines how to apply the linear movement between this constrained nodes 
    core::objectmodel::Data<unsigned> minDepIndice; ///< The indice node in the list of constrained nodes, which is imposed the minimum displacment 
    core::objectmodel::Data<unsigned> maxDepIndice; ///< The indice node in the list of constrained nodes, which is imposed the maximum displacment 
    core::objectmodel::Data<type::vector<Real> > m_imposedDisplacmentOnMacroNodes; ///< imposed displacement at  u1 u2 u3 u4 for 2d case
    ///< and u1 u2 u3 u4 u5 u6 u7 u8 for 3d case
    Data<Real> X0; ///< Size of specimen in X-direction
    Data<Real> Y0; ///< Size of specimen in Y-direction
    Data<Real> Z0; ///< Size of specimen in Z-direction

    enum { NumDimensions = Deriv::total_size };
    typedef sofa::type::fixed_array<bool,NumDimensions> VecBool;
    core::objectmodel::Data<VecBool> movedDirections;  ///< Defines the directions in which the particles are moved: true (or 1) for fixed, false (or 0) for free.

    /// Link to be set to the topology container in the component graph.
    SingleLink<PartialLinearMovementConstraint<DataTypes>, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;

protected:
    PartialLinearMovementConstraint();
    ~PartialLinearMovementConstraint() override;

public:
    ///methods to add/remove some indices, keyTimes, keyMovement
    void clearIndices();
    void addIndex(Index index);
    void removeIndex(Index index);
    void clearKeyMovements();

    ///@brief Add a new key movement
    ///@param time : the simulation time you want to set a movement (in sec)
    ///@param movement : the corresponding motion
    ///for instance, addKeyMovement(1.0, Deriv(5,0,0) ) will set a translation of 5 in x direction a time 1.0s
    void addKeyMovement(Real time, Deriv movement);


    /// -- Constraint interface
    void init() override;
    void reset() override;

    void projectResponse(const core::MechanicalParams* mparams, DataVecDeriv& resData) override;
    void projectVelocity(const core::MechanicalParams* mparams, DataVecDeriv& vData) override;
    void projectPosition(const core::MechanicalParams* mparams, DataVecCoord& xData) override;
    void projectJacobianMatrix(const core::MechanicalParams* mparams, DataMatrixDeriv& cData) override;

    void applyConstraint(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix) override;
    void applyConstraint(const core::MechanicalParams* mparams, linearalgebra::BaseVector* vector, const sofa::core::behavior::MultiMatrixAccessor* matrix) override;
    void applyConstraint(sofa::core::behavior::ZeroDirichletCondition* matrix) override;

    void draw(const core::visual::VisualParams*) override;

protected:
    template <class DataDeriv>
    void projectResponseT(DataDeriv& dx,
        const std::function<void(DataDeriv&, const unsigned int, const VecBool&)>& clear);

    template <class MyCoord>
    void interpolatePosition(Real cT, typename std::enable_if<!std::is_same<MyCoord, sofa::defaulttype::RigidCoord<3, Real> >::value, VecCoord>::type& x);
    template <class MyCoord>
    void interpolatePosition(Real cT, typename std::enable_if<std::is_same<MyCoord, sofa::defaulttype::RigidCoord<3, Real> >::value, VecCoord>::type& x);

private:

    /// to keep the time corresponding to the key times
    Real currentTime;

    /// to know if we found the key times
    bool finished;

    /// find previous and next time keys
    void findKeyTimes();
};


#if !defined(SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_PARTIALLINEARMOVEMENTCONSTRAINT_CPP)
extern template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API PartialLinearMovementConstraint<defaulttype::Vec3Types>;
extern template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API PartialLinearMovementConstraint<defaulttype::Vec2Types>;
extern template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API PartialLinearMovementConstraint<defaulttype::Vec1Types>;
extern template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API PartialLinearMovementConstraint<defaulttype::Vec6Types>;
extern template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API PartialLinearMovementConstraint<defaulttype::Rigid3Types>;
#endif

} // namespace sofa::component::constraint::projective
