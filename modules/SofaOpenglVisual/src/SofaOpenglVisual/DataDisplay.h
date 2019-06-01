/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2019 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_COMPONENT_VISUALMODEL_DATADISPLAY_H
#define SOFA_COMPONENT_VISUALMODEL_DATADISPLAY_H
#include "config.h"

#include <sofa/core/visual/VisualModel.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <SofaOpenglVisual/OglColorMap.h>
#include <SofaBaseVisual/VisualModelImpl.h>

#include <sofa/defaulttype/RGBAColor.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{

class SOFA_OPENGL_VISUAL_API DataDisplay : public core::visual::VisualModel, public ExtVec3State
{
public:
    SOFA_CLASS2(DataDisplay, core::visual::VisualModel, ExtVec3State);

    typedef core::topology::BaseMeshTopology::Triangle Triangle;
    typedef core::topology::BaseMeshTopology::Quad     Quad;

    typedef helper::vector<Real> VecPointData;
    typedef helper::vector<Real> VecCellData;

public:
    Data<bool> d_maximalRange; ///< Keep the maximal range through all timesteps
    Data<VecPointData> d_pointData; ///< Data associated with nodes
    Data<VecCellData> d_triangleData; ///< Data associated with triangles
    Data<VecCellData> d_quadData; ///< Data associated with quads
    Data<VecPointData> d_pointTriangleData; ///< Data associated with nodes per triangle
    Data<VecPointData> d_pointQuadData; ///< Data associated with nodes per quad
    Data<defaulttype::RGBAColor> d_colorNaN; ///< Color for NaNs
    Data<defaulttype::Vec2f> d_userRange; ///< Clamp to this values (if max>min)
    Data<float> d_currentMin; ///< Current min range
    Data<float> d_currentMax; ///< Current max range
    Data<float> d_shininess; ///< Shininess for rendering point-based data [0,128].  <0 means no specularity

    visualmodel::OglColorMap *colorMap;
    core::State<DataTypes> *state;
    core::topology::BaseMeshTopology* topology;
    Real oldMin, oldMax;

    void init() override;
    void drawVisual(const core::visual::VisualParams* vparams) override;
    void updateVisual() override;

    bool insertInNode( core::objectmodel::BaseNode* node ) override { Inherit1::insertInNode(node); Inherit2::insertInNode(node); return true; }
    bool removeInNode( core::objectmodel::BaseNode* node ) override { Inherit1::removeInNode(node); Inherit2::removeInNode(node); return true; }

protected:
    void computeNormals();
    helper::vector<defaulttype::Vec3f> m_normals;

    DataDisplay();
};

} // namespace visualmodel

} // namespace component

} // namespace sofa

#endif // #ifndef SOFA_COMPONENT_VISUALMODEL_DATADISPLAY_H
