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
#include <sofa/component/engine/select/SphereROI.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/type/RGBAColor.h>

namespace sofa::component::engine::select
{

template <class DataTypes>
SphereROI<DataTypes>::SphereROI()
    : d_centers( initData(&d_centers, "centers", "Center(s) of the sphere(s)") )
    , d_radii( initData(&d_radii, "radii", "Radius(i) of the sphere(s)") )
    , d_direction( initData(&d_direction, "direction", "Edge direction(if edgeAngle > 0)") )
    , d_normal( initData(&d_normal, "normal", "Normal direction of the triangles (if triAngle > 0)") )
    , d_edgeAngle( initData(&d_edgeAngle, (Real)0, "edgeAngle", "Max angle between the direction of the selected edges and the specified direction") )
    , d_triAngle( initData(&d_triAngle, (Real)0, "triAngle", "Max angle between the normal of the selected triangle and the specified normal direction") )
{
    //Adding alias to handle TrianglesInSphereROI input/output
    addAlias(&d_drawROI,"isVisible");
    addAlias(&d_triAngle,"angle");
    addAlias(&d_indices,"pointIndices");
    addAlias(&d_X0,"rest_position");

    centers.setParent(&d_centers);
    radii.setParent(&d_radii);
    direction.setParent(&d_direction);
    normal.setParent(&d_normal);
    edgeAngle.setParent(&d_edgeAngle);
    triAngle.setParent(&d_triAngle);
}

template <class DataTypes>
void SphereROI<DataTypes>::roiInit()
{
    ;
}

template <class DataTypes>
bool SphereROI<DataTypes>::isPointInSphere(const CPos& c, const Real& r, const CPos& p)
{
    if((p-c).norm() > r)
        return false;
    else
        return true;
}

template <class DataTypes>
bool SphereROI<DataTypes>::isEdgeInSphere(const CPos& c, const Real& r, const sofa::core::topology::BaseMeshTopology::Edge& edge)
{
    const auto& x0 = d_X0.getValue();
    for (unsigned int i=0; i<2; ++i)
    {
        if(isPointInSphere(c, r, DataTypes::getCPos(x0[edge[i]])))
            return false;
    }
    return true;
}

template <class DataTypes>
bool SphereROI<DataTypes>::isTriangleInSphere(const CPos& c, const Real& r, const sofa::core::topology::BaseMeshTopology::Triangle& triangle)
{
    const auto& x0 = d_X0.getValue();
    for (unsigned int i=0; i<3; ++i)
    {
        if (isPointInSphere(c, r, DataTypes::getCPos(x0[triangle[i]])))
            return false;
    }
    return true;
}


template <class DataTypes>
bool SphereROI<DataTypes>::isPointInROI(const CPos& p)
{
    bool isInSpheres = false;

    const auto& centers = (d_centers.getValue());
    const auto& radii = (d_radii.getValue());

    for (unsigned int j = 0; j < centers.size(); ++j)
    {
        if (isPointInSphere(centers[j], radii[j], p))
        {
            isInSpheres = true;
            break;
        }
    }
    return isInSpheres;
}

template <class DataTypes>
bool SphereROI<DataTypes>::isEdgeInROI(const Edge& e)
{
    const auto& x0 = d_X0.getValue();
    const auto& centers = (d_centers.getValue());
    const auto& radii = (d_radii.getValue());

    const auto& dir = d_direction.getValue();
    const auto eAngle = d_edgeAngle.getValue();

    for (unsigned int j = 0; j < centers.size(); ++j)
    {
        if (isEdgeInSphere(centers[j], radii[j], e))
        {
            if (eAngle > 0)
            {
                auto n = DataTypes::getCPos(x0[e[1]]) - DataTypes::getCPos(x0[e[0]]);
                n.normalize();
                if (fabs(dot(n, dir)) < fabs(cos(eAngle * M_PI / 180.0)))
                    continue;
            }
            return true;
        }
    }

    return false;
}

template <class DataTypes>
bool SphereROI<DataTypes>::isEdgeInStrictROI(const Edge& e)
{
    return isEdgeInROI(e);
}

template <class DataTypes>
bool SphereROI<DataTypes>::isTriangleInROI(const Triangle& t)
{
    const auto& x0 = d_X0.getValue();
    const auto& centers = (d_centers.getValue());
    const auto& radii = (d_radii.getValue());

    const auto tAngle = d_triAngle.getValue();
    const auto& normal = d_normal.getValue();

    for (unsigned int j = 0; j < centers.size(); ++j)
    {
        if (isTriangleInSphere(centers[j], radii[j], t))
        {
            if (tAngle > 0)
            {
                auto n = cross(
                    DataTypes::getCPos(x0[t[2]]) - DataTypes::getCPos(x0[t[0]]), 
                    DataTypes::getCPos(x0[t[1]]) - DataTypes::getCPos(x0[t[0]])
                );
                n.normalize();

                if (dot(n, normal) < std::cos(tAngle * M_PI / 180.0)) 
                    continue;
            }
            return true;
        }
    }
    return false;
}

template <class DataTypes>
bool SphereROI<DataTypes>::isTriangleInStrictROI(const Triangle& t)
{
    return isTriangleInROI(t);
}

template <class DataTypes>
bool SphereROI<DataTypes>::roiDoUpdate()
{
    const auto& cen = (d_centers.getValue());
    const auto& rad = (d_radii.getValue());

    if (cen.empty())
        return false;

    if (cen.size() != rad.size())
    {
		if (rad.size() == 1)
		{
			Real r = rad[0];
			helper::WriteOnlyAccessor< Data<type::vector<Real> > > radWriter = radii;
			for (unsigned int i = 0; i < cen.size() - 1; i++) 
                radWriter.push_back(r);
		}
		else
		{
			msg_warning() << "Number of sphere centers and radius doesn't match.";
			return false;
		}
    }

    const Real eAngle = d_edgeAngle.getValue();
    const Real tAngle = d_triAngle.getValue();

    if (eAngle > 0)
    {
        auto dir = sofa::helper::getWriteOnlyAccessor(d_direction);
        dir.wref().normalize();
    }

    if (tAngle > 0)
    {
        auto norm = sofa::helper::getWriteOnlyAccessor(d_normal);
        norm.wref().normalize();
    }

    return true;
}

template <class DataTypes>
void SphereROI<DataTypes>::roiDraw(const core::visual::VisualParams* vparams)
{
    const VecCoord* x0 = &d_X0.getValue();
    constexpr const sofa::type::RGBAColor& color = sofa::type::RGBAColor::cyan();

    const auto& c = d_centers.getValue();
    const auto& r = d_radii.getValue();
    std::vector<sofa::type::Vec3> drawcenters;
    std::vector<float> drawradii;

    for (unsigned int i=0; i<c.size() && i<r.size(); ++i)
    {

        drawcenters.push_back(c[i]);
        drawradii.push_back(float(r[i]));
            
        if (edgeAngle.getValue() > 0)
        {
            vparams->drawTool()->drawCone(c[i], c[i] + direction.getValue()*(cos(edgeAngle.getValue()*M_PI / 180.0)*r[i]), 0, (float)sin(edgeAngle.getValue()*M_PI / 180.0)*((float)r[i]), color);
        }

        if (triAngle.getValue() > 0)
        {
            vparams->drawTool()->drawCone(c[i], c[i] + normal.getValue()*(cos(triAngle.getValue()*M_PI / 180.0)*r[i]), 0, (float)sin(triAngle.getValue()*M_PI / 180.0)*((float)r[i]), color);
        }
    }

    vparams->drawTool()->setPolygonMode(0, true);
    vparams->drawTool()->drawSpheres(drawcenters, drawradii, color);
    vparams->drawTool()->setPolygonMode(0, false);

}

} //namespace sofa::component::engine::select
