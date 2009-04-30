#include <sofa/component/collision/TriangleModelInRegularGrid.h>
#include <sofa/component/collision/CubeModel.h>
#include <sofa/component/collision/Triangle.h>
#include <sofa/component/topology/TriangleData.inl>
#include <sofa/simulation/common/Node.h>
#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/core/CollisionElement.h>
#include <sofa/core/ObjectFactory.h>
#include <vector>
#include <sofa/helper/gl/template.h>
#include <iostream>

#include <sofa/component/topology/PointSetTopologyChange.h>
#include <sofa/component/topology/TriangleSetTopologyChange.h>
#include <sofa/core/componentmodel/topology/TopologicalMapping.h>

#include <sofa/simulation/common/Simulation.h>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::core::componentmodel::topology;

SOFA_DECL_CLASS ( TriangleInRegularGrid );

int TriangleModelInRegularGridClass = core::RegisterObject ( "collision model using a triangular mesh in a regular grid, as described in BaseMeshTopology" )
        .add< TriangleModelInRegularGrid >()
        ;

TriangleModelInRegularGrid::TriangleModelInRegularGrid()
    :TriangleModel()
{

}


TriangleModelInRegularGrid::~TriangleModelInRegularGrid()
{

}


void TriangleModelInRegularGrid::init()
{
    TriangleModel::init();

    if( !mstate) { serr << "TriangleModel requires a Vec3 Mechanical Model" << sendl; return;}
    if (!_topology) { serr << "TriangleModel requires a BaseMeshTopology" << sendl; return;}

    // Test if _topology depend on an higher topology (to compute Bounding Tree faster) and get it
    TopologicalMapping* _topoMapping = NULL;
    vector<TopologicalMapping*> topoVec;
    getContext()->get<TopologicalMapping> ( &topoVec, core::objectmodel::BaseContext::SearchRoot );
    _higher_topo = _topology;
    _higher_mstate = mstate;
    bool found = true;
    while ( found )
    {
        found = false;
        for ( vector<TopologicalMapping*>::iterator it = topoVec.begin(); it != topoVec.end(); it++ )
        {
            if ( ( *it )->getTo() == _higher_topo )
            {
                found = true;
                _topoMapping = *it;
                _higher_topo = _topoMapping->getFrom();
                if ( !_higher_topo ) break;
                sofa::simulation::Node* node = static_cast< sofa::simulation::Node* > ( _higher_topo->getContext() );
                _higher_mstate = dynamic_cast< core::componentmodel::behavior::MechanicalState<Vec3Types>* > ( node->getMechanicalState() );
            }
        }
    }
    if ( _topoMapping && !_higher_topo ) { serr << "Topological Mapping " << _topoMapping->getName() << " returns a from topology pointer equal to NULL." << sendl; return;}
    else if ( _higher_topo != _topology ) sout << "Using the " << _higher_topo->getClassName() << " \"" << _higher_topo->getName() << "\" to compute the bounding trees." << sendl;
    else sout << "Keeping the TriangleModel to compute the bounding trees." << sendl;
}

void TriangleModelInRegularGrid::computeBoundingTree ( int )
{
    CubeModel* cubeModel = createPrevious<CubeModel>();
    updateFromTopology();
    if ( needsUpdate && !cubeModel->empty() ) cubeModel->resize ( 0 );
    if ( !isMoving() && !cubeModel->empty() && !needsUpdate ) return; // No need to recompute BBox if immobile

    needsUpdate=false;
    Vector3 minElem, maxElem;
    const VecCoord& x = *_higher_mstate->getX();

    const bool calcNormals = computeNormals.getValue();

    // no hierarchy
    if ( empty() )
        cubeModel->resize ( 0 );
    else
    {
        cubeModel->resize ( 1 );
        minElem = x[0];
        maxElem = x[0];
        for ( unsigned i=1; i<x.size(); i++ )
        {
            const Vector3& pt1 = x[i];
            if ( pt1[0] > maxElem[0] ) maxElem[0] = pt1[0];
            else if ( pt1[0] < minElem[0] ) minElem[0] = pt1[0];
            if ( pt1[1] > maxElem[1] ) maxElem[1] = pt1[1];
            else if ( pt1[1] < minElem[1] ) minElem[1] = pt1[1];
            if ( pt1[2] > maxElem[2] ) maxElem[2] = pt1[2];
            else if ( pt1[2] < minElem[2] ) minElem[2] = pt1[2];
        }
        cubeModel->setLeafCube ( 0, std::make_pair ( this->begin(),this->end() ), minElem, maxElem ); // define the bounding box of the current triangle
    }
}

}

}

}
