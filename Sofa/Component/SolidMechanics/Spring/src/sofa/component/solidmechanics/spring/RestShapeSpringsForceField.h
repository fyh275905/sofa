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
#include <sofa/component/solidmechanics/spring/config.h>

#include <sofa/type/RGBAColor.h>

#include <sofa/core/behavior/ForceField.h>
#include <sofa/core/objectmodel/Data.h>
#include <sofa/core/topology/TopologySubsetIndices.h>
#include <sofa/type/vector.h>
#include <sofa/linearalgebra/EigenSparseMatrix.h>


namespace sofa::core::behavior
{

template< class T > class MechanicalState;

} // namespace sofa::core::behavior

namespace sofa::component::solidmechanics::spring
{

/**
* @brief This class describes a simple elastic springs ForceField between DOFs positions and rest positions.
*
* Springs are applied to given degrees of freedom between their current positions and their rest shape positions.
* An external MechanicalState reference can also be passed to the ForceField as rest shape position.
*/
template<class DataTypes>
class RestShapeSpringsForceField : public core::behavior::ForceField<DataTypes>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(RestShapeSpringsForceField, DataTypes), SOFA_TEMPLATE(core::behavior::ForceField, DataTypes));

    typedef core::behavior::ForceField<DataTypes> Inherit;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::CPos CPos;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename DataTypes::Real Real;
    typedef type::vector< sofa::Index > SetIndexArray;
    typedef sofa::core::topology::TopologySubsetIndices SetIndex;
    typedef type::vector< Real >	 VecReal;

    typedef core::objectmodel::Data<VecCoord> DataVecCoord;
    typedef core::objectmodel::Data<VecDeriv> DataVecDeriv;

    SetIndex d_points; ///< points controlled by the rest shape springs
    Data< VecReal > d_stiffness; ///< stiffness values between the actual position and the rest shape position
    Data< VecReal > d_angularStiffness; ///< angularStiffness assigned when controlling the rotation of the points
    Data< type::vector< CPos > > d_pivotPoints; ///< global pivot points used when translations instead of the rigid mass centers
    Data< SetIndexArray > d_external_points; ///< points from the external Mechancial State that define the rest shape springs
    Data< bool > d_recompute_indices; ///< Recompute indices (should be false for BBOX)
    Data< bool > d_drawSpring; ///< draw Spring
    Data< sofa::type::RGBAColor > d_springColor; ///< spring color. (default=[0.0,1.0,0.0,1.0])

    SingleLink<RestShapeSpringsForceField<DataTypes>, sofa::core::behavior::MechanicalState< DataTypes >, BaseLink::FLAG_STOREPATH|BaseLink::FLAG_STRONGLINK> l_restMState;
    linearalgebra::EigenBaseSparseMatrix<typename DataTypes::Real> matS;

protected:
    RestShapeSpringsForceField();

public:
    /// BaseObject initialization method.
    void bwdInit() override ;
    void parse(core::objectmodel::BaseObjectDescription *arg) override ;
    void reinit() override ;

    /// Add the forces.
    void addForce(const core::MechanicalParams* mparams, DataVecDeriv& f, const DataVecCoord& x, const DataVecDeriv& v) override;
    /// Link to be set to the topology container in the component graph.
    SingleLink<core::behavior::ForceField<DataTypes>, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;

    void addDForce(const core::MechanicalParams* mparams, DataVecDeriv& df, const DataVecDeriv& dx) override;

    SReal getPotentialEnergy(const core::MechanicalParams* mparams, const DataVecCoord& x) const override
    {
        SOFA_UNUSED(mparams);
        SOFA_UNUSED(x);

        msg_warning() << "Method getPotentialEnergy not implemented yet.";
        return 0.0;
    }

    /// Brings ForceField contribution to the global system stiffness matrix.
    void addKToMatrix(const core::MechanicalParams* mparams, const sofa::core::behavior::MultiMatrixAccessor* matrix ) override;

    void draw(const core::visual::VisualParams* vparams) override;


    const DataVecCoord* getExtPosition() const;
    const SetIndexArray& getIndices() const { return m_indices; }
    const SetIndexArray& getExtIndices() const { return (useRestMState ? m_ext_indices : m_indices); }

protected :

    void recomputeIndices();
    bool checkOutOfBoundsIndices();
    bool checkOutOfBoundsIndices(const SetIndexArray &indices, const sofa::Size dimension);

    SetIndexArray m_indices;
    SetIndexArray m_ext_indices;
    type::vector<CPos> m_pivots;

    SReal lastUpdatedStep;

private :

    bool useRestMState; /// An external MechanicalState is used as rest reference.
};

#if  !defined(SOFA_COMPONENT_FORCEFIELD_RESTSHAPESPRINGSFORCEFIELD_CPP)
extern template class SOFA_COMPONENT_SOLIDMECHANICS_SPRING_API RestShapeSpringsForceField<sofa::defaulttype::Vec3Types>;
extern template class SOFA_COMPONENT_SOLIDMECHANICS_SPRING_API RestShapeSpringsForceField<sofa::defaulttype::Vec1Types>;
extern template class SOFA_COMPONENT_SOLIDMECHANICS_SPRING_API RestShapeSpringsForceField<sofa::defaulttype::Rigid3Types>;
#endif

} // namespace sofa::component::solidmechanics::spring
