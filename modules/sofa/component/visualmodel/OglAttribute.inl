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
#ifndef _OGL_ATTRIBUTE_INL_
#define _OGL_ATTRIBUTE_INL_

#include <sofa/component/visualmodel/OglAttribute.h>
#include <sofa/component/topology/PointSetTopologyChange.h>
#include <sofa/component/topology/TriangleSetTopologyChange.h>
#include <sofa/component/topology/QuadSetTopologyChange.h>
#include <sofa/component/topology/TetrahedronSetTopologyChange.h>
#include <sofa/component/topology/HexahedronSetTopologyChange.h>
#include <sofa/core/ObjectFactory.h>

namespace sofa
{

namespace component
{

namespace visualmodel
{

using namespace sofa::core::componentmodel::topology;
using namespace sofa::component::topology;

template < int size, unsigned int type, class DataTypes>
OglAttribute< size, type, DataTypes>::OglAttribute() :
    OglShaderElement()
    , _abo ( -1 )
    , _usage( GL_STATIC_DRAW)
    ,value( initData(&value, "value", "internal Data"))
{
    _topology = NULL;
}


template < int size, unsigned int type, class DataTypes>
OglAttribute< size, type, DataTypes>::~OglAttribute()
{
    glDeleteBuffersARB(1, &_abo);
}


template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::init()
{
    OglShaderElement::init();
    getContext()->get( _topology);

    if ( ( int ) _abo == -1 ) glGenBuffers ( 1, &_abo );
}


template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::initVisual ()
{
    const ResizableExtVector<DataTypes>& data = value.getValue();
    unsigned int totalSize = data.size() *sizeof ( data[0] );
    glBindBufferARB ( GL_ARRAY_BUFFER, _abo );
    glBufferDataARB ( GL_ARRAY_BUFFER,
            totalSize,
            NULL,
            _usage );
    // Fill the buffer
    glBufferSubDataARB ( GL_ARRAY_BUFFER,
            0,
            totalSize,
            data.getData() );
    _index = shader->getAttribute ( indexShader.getValue(), id.getValue().c_str() );

    enable();
    glBindBufferARB(GL_ARRAY_BUFFER,0);
}


template < int size, unsigned int type, class DataTypes>
bool OglAttribute< size, type, DataTypes>::updateABO()
{
    const ResizableExtVector<DataTypes>& data = value.getValue();
    unsigned int totalSize = data.size() *sizeof ( data[0] );
    glBindBufferARB ( GL_ARRAY_BUFFER, _abo );
    glBufferDataARB ( GL_ARRAY_BUFFER,
            totalSize,
            NULL,
            _usage );
    // Fill the buffer
    glBufferSubDataARB ( GL_ARRAY_BUFFER,
            0,
            totalSize,
            (char*)data.getData() );

    enable();
    glBindBufferARB(GL_ARRAY_BUFFER,0);

    return true;
}



template < int size, unsigned int type, class DataTypes>
ResizableExtVector<DataTypes>* OglAttribute< size, type, DataTypes>::beginEdit()
{
    return value.beginEdit();
}


template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::endEdit()
{
    value.endEdit();
}


template < int size, unsigned int type, class DataTypes>
const ResizableExtVector<DataTypes>& OglAttribute< size, type, DataTypes>::getValue() const
{
    return value.getValue();
}


template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::setValue ( const ResizableExtVector<DataTypes>& value )
{
    ResizableExtVector<DataTypes>& val = * ( this->value.beginEdit() );
    val = value;
    this->value.endEdit();
}


template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::enable()
{

    glBindBufferARB(GL_ARRAY_BUFFER, _abo);
    glEnableVertexAttribArrayARB ( _index );
    glVertexAttribPointerARB ( _index, size, type, GL_FALSE, 0, ( char* ) NULL + 0);
    //glBindBufferARB(GL_ARRAY_BUFFER, 0);
}


template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::disable()
{
    glDisableVertexAttribArrayARB ( _index );
    glBindBufferARB(GL_ARRAY_BUFFER,0);
}

template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::fwdDraw(Pass)
{
    enable();
}

template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::bwdDraw(Pass)
{
    disable();
}

template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::reinit()
{
    init();
    initVisual();
}

// Only resizing and renumbering is done. 'value' has to be set by external components.
template < int size, unsigned int type, class DataTypes>
void OglAttribute< size, type, DataTypes>::handleTopologyChange()
{
    if( _topology)
    {
        std::list<const TopologyChange *>::const_iterator itBegin=_topology->firstChange();
        std::list<const TopologyChange *>::const_iterator itEnd=_topology->lastChange();

        while( itBegin != itEnd )
        {
            core::componentmodel::topology::TopologyChangeType changeType = (*itBegin)->getChangeType();

            switch( changeType )
            {
            case core::componentmodel::topology::ENDING_EVENT:
            {
                //sout << "INFO_print : Vis - ENDING_EVENT" << sendl;
                updateVisual();
                updateABO();
                break;
            }

            case core::componentmodel::topology::TRIANGLESADDED:
            {
                //sout << "INFO_print : Vis - TRIANGLESADDED" << sendl;
                break;
            }

            case core::componentmodel::topology::QUADSADDED:
            {
                //sout << "INFO_print : Vis - QUADSADDED" << sendl;
                break;
            }

            case core::componentmodel::topology::TRIANGLESREMOVED:
            {
                //sout << "INFO_print : Vis - TRIANGLESREMOVED" << sendl;
                break;
            }

            case core::componentmodel::topology::QUADSREMOVED:
            {
                //sout << "INFO_print : Vis - QUADSREMOVED" << sendl;
                break;
            }

            case core::componentmodel::topology::POINTSADDED:
            {
                //sout << "INFO_print : Vis - POINTSADDED" << sendl;

                unsigned int nbPoints = ( static_cast< const PointsAdded * >( *itBegin ) )->getNbAddedVertices();
                ResizableExtVector<DataTypes>& data = *value.beginEdit();
                data.resize( data.size() + nbPoints);
                value.endEdit();
                break;
            }

            // Case "POINTSREMOVED" added to propagate the treatment to the Visual Model

            case core::componentmodel::topology::POINTSREMOVED:
            {
                //sout << "INFO_print : Vis - POINTSREMOVED" << sendl;

                const sofa::helper::vector<unsigned int> tab = ( static_cast< const sofa::component::topology::PointsRemoved * >( *itBegin ) )->getArray();
                ResizableExtVector<DataTypes>& data = *value.beginEdit();
                unsigned int last = data.size();

                for ( unsigned int i = 0; i < tab.size(); ++i)
                {
                    last--;
                    data[tab[i]] = data[last];
                }
                data.resize( last);
                value.endEdit();

                break;
            }

            // Case "POINTSRENUMBERING" added to propagate the treatment to the Visual Model

            case core::componentmodel::topology::POINTSRENUMBERING:
            {
                //sout << "INFO_print : Vis - POINTSRENUMBERING" << sendl;

                const sofa::helper::vector<unsigned int> tab = ( static_cast< const sofa::component::topology::PointsRenumbering * >( *itBegin ) )->getinv_IndexArray();
                ResizableExtVector<DataTypes>& data = *value.beginEdit();
                vector<DataTypes> tmp;
                for ( unsigned int i = 0; i < tab.size(); ++i)
                {
                    tmp.push_back( data[tab[i]]);
                }
                for ( unsigned int i = 0; i < tab.size(); ++i)
                {
                    data[i] = tmp[i];
                }
                value.endEdit();

                break;
            }

            default:
                // Ignore events that are not Triangle  related.
                break;
            }; // switch( changeType )

            ++itBegin;
        } // while( changeIt != last; )
    }
}

}

}

}

#endif
