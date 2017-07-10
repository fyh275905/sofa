/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2017 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_COMPONENT_ENGINE_SMOOTHMESHENGINE_INL
#define SOFA_COMPONENT_ENGINE_SMOOTHMESHENGINE_INL

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include "SmoothMeshEngine.h"
#include <sofa/helper/gl/template.h>

#include <sofa/core/visual/VisualParams.h>

namespace sofa
{

namespace component
{

namespace engine
{

template <class DataTypes>
SmoothMeshEngine<DataTypes>::SmoothMeshEngine()
    : l_topology( initLink( "topology", "Link to a BaseTopology component"))
    , input_position( initData (&input_position, "input_position", "Input position") )
    , input_indices( initData (&input_indices, "input_indices", "Position indices that need to be smoothed, leave empty for all positions") )
    , output_position( initData (&output_position, "output_position", "Output position") )
    , nb_iterations( initData (&nb_iterations, (unsigned int)1, "nb_iterations", "Number of iterations of laplacian smoothing"))
    , d_method(initData(&d_method,"method","Laplacian formulation (simple, central, cotangent)"))
{
    this->addAlias(&input_position,"inputPosition");
    this->addAlias(&output_position,"outputPosition");
    this->addAlias(&nb_iterations,"iterations");

    helper::OptionsGroup methodOptions(2, "umbrealla", "cotangent");
    methodOptions.setSelectedItem(0); // umbrealla
    d_method.setValue(methodOptions);
}

template <class DataTypes>
void SmoothMeshEngine<DataTypes>::init()
{
    if( !l_topology )
    {
        l_topology = this->getContext()->getMeshTopology();
        if (!l_topology)
            serr << "requires a mesh topology" << sendl;
    }

    addInput(&input_position);
    addInput(&input_indices);
    addInput(&d_method);
    addInput(&nb_iterations);
    addOutput(&output_position);

    setDirtyValue();
}

template <class DataTypes>
void SmoothMeshEngine<DataTypes>::reinit()
{
    update();
}


template <class DataTypes>
inline void SmoothMeshEngine<DataTypes>::laplacian( unsigned method, VecCoord& out, size_t index, const VecCoord& in )
{
    // note that out[i] == in[i] when entering

    using sofa::core::topology::BaseMeshTopology;

    switch( method )
    {

        case 1: // cotangent
        {
            // iterating over triangles surrounding the current vertex
            const BaseMeshTopology::TrianglesAroundVertex& vt = l_topology->getTrianglesAroundVertex(index);
            if( !vt.empty() )
            {
                const Coord& curPt = in[index];
                out[index].clear();
                Real cumWeight = 0; // weight sum

                for( unsigned int j = 0 ; j < vt.size() ; j++ )
                {
                    const BaseMeshTopology::Triangle& t = l_topology->getTriangle( vt[j] );
                    for( int i=0 ; i<3 ; ++i ) // finding the current vertex in neighbour triangle
                    {
                        if( t[i] != index ) continue;

                        const Coord& curNeib = in[t[(i+1)%3]];
                        const Coord& nextNeib = in[t[(i+2)%3]];


                        // TODO some edge length computations are redundant in-between triangles

                        Coord curEdge = curPt-curNeib;
                        if(!curEdge.normalize()) break; // degenerated triangle, go to the next triangle

                        Coord nextEdge = curPt-nextNeib;
                        if(!nextEdge.normalize()) break; // degenerated triangle, go to the next triangle

                        Coord facingEdge = curNeib-nextNeib;
                        if(!facingEdge.normalize()) break; // degenerated triangle, go to the next triangle

                        const Real curAngle = acos(-curEdge*facingEdge);
                        const Real nextAngle = acos(nextEdge*facingEdge);

                        const Real curW  = 1./tan(curAngle);
                        const Real nextW = 1./tan(nextAngle);

                        out[index] += nextW * curNeib;
                        out[index] += curW * nextNeib;
                        cumWeight += curW + nextW;
                        break;
                    }
                }

                if( cumWeight )
                    out[index] /= cumWeight;
                else
                    out[index] = curPt;

            }

            break;
        }

        default:
            serr<<"unknown method"<<sendl;
        case 0: // umbrealla scheme - simply using neighbours pos w/o weights
        {
            BaseMeshTopology::VerticesAroundVertex v = l_topology->getVerticesAroundVertex(index);
            if( !v.empty() )
            {
                out[index] = in[v[0]];
                for( unsigned int j = 1 ; j < v.size() ; j++ )
                    out[index] += in[v[j]];
                out[index] /= v.size();
            }
            break;
        }

    }

}

template <class DataTypes>
void SmoothMeshEngine<DataTypes>::update()
{
    helper::ReadAccessor< Data<VecCoord> > in(input_position);
    helper::ReadAccessor< Data<helper::vector <unsigned int > > > indices(input_indices);

    unsigned method = d_method.getValue().getSelectedId();
    unsigned iterations = nb_iterations.getValue();

    cleanDirty();

    if (!l_topology) return;

    VecCoord& out = *output_position.beginWriteOnly();

    out.resize(in.size());
    VecCoord t( out.size() ); // temp

    for (size_t i =0; i<in.size();i++) t[i] = out[i] = in[i];

    
    for (size_t n=0; n < iterations ; n++)
    {
        if( indices.empty() )
        {
            for (size_t i = 0; i < out.size(); i++) laplacian( method, t, i, out );
            for (size_t i=0 ; i<in.size() ; i++ ) out[i] = t[i];
        }
        else
        {
            for(size_t i = 0; i < indices.size(); i++) laplacian( method, t, indices[i], out );
            for(size_t i = 0; i < indices.size(); i++) out[indices[i]] = t[indices[i]];
        }
    }

    output_position.endEdit();

}

} // namespace engine

} // namespace component

} // namespace sofa

#endif
