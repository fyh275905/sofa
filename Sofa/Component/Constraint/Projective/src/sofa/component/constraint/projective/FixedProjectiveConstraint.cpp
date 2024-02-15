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
#define SOFA_COMPONENT_PROJECTIVECONSTRAINTSET_FIXEDPROJECTIVECONSTRAINT_CPP
#include <sofa/component/constraint/projective/FixedProjectiveConstraint.inl>
#include <sofa/core/ObjectFactory.h>

#include <sofa/simulation/Node.h>

namespace sofa::component::constraint::projective
{

using namespace sofa::defaulttype;
using namespace sofa::helper;


int FixedProjectiveConstraintClass = core::RegisterObject("Attach given particles to their initial positions")
        .add< FixedProjectiveConstraint<Vec3Types> >()
        .add< FixedProjectiveConstraint<Vec2Types> >()
        .add< FixedProjectiveConstraint<Vec1Types> >()
        .add< FixedProjectiveConstraint<Vec6Types> >()
        .add< FixedProjectiveConstraint<Rigid3Types> >()
        .add< FixedProjectiveConstraint<Rigid2Types> >()
        ;


// methods specilizations declaration
template <> SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API
void FixedProjectiveConstraint<defaulttype::Rigid3Types >::draw(const core::visual::VisualParams* vparams);
template <> SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API
void FixedProjectiveConstraint<defaulttype::Rigid2Types >::draw(const core::visual::VisualParams* vparams);



template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API FixedProjectiveConstraint<Vec3Types>;
template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API FixedProjectiveConstraint<Vec2Types>;
template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API FixedProjectiveConstraint<Vec1Types>;
template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API FixedProjectiveConstraint<Vec6Types>;
template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API FixedProjectiveConstraint<Rigid3Types>;
template class SOFA_COMPONENT_CONSTRAINT_PROJECTIVE_API FixedProjectiveConstraint<Rigid2Types>;



// methods specilizations definition
template <>
void FixedProjectiveConstraint<Rigid3Types>::draw(const core::visual::VisualParams* vparams)
{
    if (this->d_componentState.getValue() != ComponentState::Valid) return;
    if (!d_showObject.getValue()) return;
    if (!this->isActive()) return;
    if (!vparams->displayFlags().getShowBehaviorModels()) return;

    const auto stateLifeCycle = vparams->drawTool()->makeStateLifeCycle();

    const SetIndexArray & indices = d_indices.getValue();
    const VecCoord& x = mstate->read(core::ConstVecCoordId::position())->getValue();

    std::vector< type::Vec3 > points;

    if (d_fixAll.getValue())
    {
        for (const auto& xi : x)
            points.push_back(xi.getCenter());
    }
    else
    {
        if( x.size() < indices.size() )
        {
            for (unsigned i=0; i<x.size(); i++ )
            {
                points.push_back(x[indices[i]].getCenter());
            }
        }
        else
        {
            for (const unsigned int index : indices)
            {
                points.push_back(x[index].getCenter());
            }
        }
    }

    if( d_drawSize.getValue() == 0) // old classical drawing by points
        vparams->drawTool()->drawPoints(points, 10, sofa::type::RGBAColor(1,0.5,0.5,1));
    else
        vparams->drawTool()->drawSpheres(points, (float)d_drawSize.getValue(), sofa::type::RGBAColor(1.0f,0.35f,0.35f,1.0f));


}

template <>
void FixedProjectiveConstraint<Rigid2Types>::draw(const core::visual::VisualParams* vparams)
{
    if (this->d_componentState.getValue() != ComponentState::Valid) return;
    if (!d_showObject.getValue()) return;
    if (!this->isActive()) return;
    if (!vparams->displayFlags().getShowBehaviorModels()) return;

    const auto stateLifeCycle = vparams->drawTool()->makeStateLifeCycle();

    const SetIndexArray& indices = d_indices.getValue();
    const VecCoord& x =mstate->read(core::ConstVecCoordId::position())->getValue();

    vparams->drawTool()->setLightingEnabled(false);
    constexpr sofa::type::RGBAColor color (1,0.5,0.5,1);
    std::vector<sofa::type::Vec3> vertices;

    if(d_fixAll.getValue())
    {
        for (const auto& xi : x)
            vertices.emplace_back(xi.getCenter()[0], xi.getCenter()[1], 0.0);
    }
    else
    {
        for (const unsigned int indice : indices)
            vertices.emplace_back(x[indice].getCenter()[0], x[indice].getCenter()[1], 0.0);
    }

    vparams->drawTool()->drawPoints(vertices, 10, color);

}

} // namespace sofa::component::constraint::projective
