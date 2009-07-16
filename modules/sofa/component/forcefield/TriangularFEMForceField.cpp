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
#define SOFA_COMPONENT_FORCEFIELD_TRIANGULARFEMFORCEFIELD_CPP
#include <sofa/component/forcefield/TriangularFEMForceField.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include <sofa/helper/gl/template.h>
#include <sofa/component/topology/TriangleData.inl>
#include <sofa/component/topology/EdgeData.inl>
#include <sofa/component/topology/PointData.inl>
#include <sofa/helper/system/gl.h>
#include <sofa/helper/system/thread/debug.h>
#include <fstream> // for reading the file
#include <iostream> //for debugging
#include <vector>
#include <algorithm>
#include <sofa/defaulttype/Vec3Types.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif


// #define DEBUG_TRIANGLEFEM

namespace sofa
{
namespace component
{
namespace forcefield
{
using namespace sofa::defaulttype;
using namespace	sofa::component::topology;
using namespace core::componentmodel::topology;





static Vec3d ColorMap[64] =
{
    Vec3d( 0.0,        0.0,       0.5625 ),
    Vec3d( 0.0,        0.0,       0.625  ),
    Vec3d( 0.0,        0.0,       0.6875 ),
    Vec3d( 0.0,        0.0,         0.75 ),
    Vec3d( 0.0,        0.0,       0.8125 ),
    Vec3d( 0.0,        0.0,        0.875 ),
    Vec3d( 0.0,        0.0,       0.9375 ),
    Vec3d( 0.0,        0.0,          1.0 ),
    Vec3d( 0.0,     0.0625,          1.0 ),
    Vec3d( 0.0,      0.125,          1.0 ),
    Vec3d( 0.0,     0.1875,          1.0 ),
    Vec3d( 0.0,       0.25,          1.0 ),
    Vec3d( 0.0,     0.3125,          1.0 ),
    Vec3d( 0.0,      0.375,          1.0 ),
    Vec3d( 0.0,     0.4375,          1.0 ),
    Vec3d( 0.0,        0.5,          1.0 ),
    Vec3d( 0.0,     0.5625,          1.0 ),
    Vec3d( 0.0,      0.625,          1.0 ),
    Vec3d( 0.0,     0.6875,          1.0 ),
    Vec3d( 0.0,       0.75,          1.0 ),
    Vec3d( 0.0,     0.8125,          1.0 ),
    Vec3d( 0.0,     0.875,           1.0 ),
    Vec3d( 0.0,     0.9375,          1.0 ),
    Vec3d( 0.0,        1.0,          1.0 ),
    Vec3d( 0.0625,     1.0,          1.0 ),
    Vec3d( 0.125,      1.0,       0.9375 ),
    Vec3d( 0.1875,     1.0,        0.875 ),
    Vec3d( 0.25,       1.0,       0.8125 ),
    Vec3d( 0.3125,     1.0,         0.75 ),
    Vec3d( 0.375,      1.0,       0.6875 ),
    Vec3d( 0.4375,     1.0,        0.625 ),
    Vec3d( 0.5,        1.0,       0.5625 ),
    Vec3d( 0.5625,     1.0,          0.5 ),
    Vec3d( 0.625,      1.0,       0.4375 ),
    Vec3d( 0.6875,     1.0,        0.375 ),
    Vec3d( 0.75,       1.0,       0.3125 ),
    Vec3d( 0.8125,     1.0,         0.25 ),
    Vec3d( 0.875,      1.0,       0.1875 ),
    Vec3d( 0.9375,     1.0,        0.125 ),
    Vec3d( 1.0,        1.0,       0.0625 ),
    Vec3d( 1.0,        1.0,          0.0 ),
    Vec3d( 1.0,       0.9375,        0.0 ),
    Vec3d( 1.0,        0.875,        0.0 ),
    Vec3d( 1.0,       0.8125,        0.0 ),
    Vec3d( 1.0,         0.75,        0.0 ),
    Vec3d( 1.0,       0.6875,        0.0 ),
    Vec3d( 1.0,        0.625,        0.0 ),
    Vec3d( 1.0,       0.5625,        0.0 ),
    Vec3d( 1.0,          0.5,        0.0 ),
    Vec3d( 1.0,       0.4375,        0.0 ),
    Vec3d( 1.0,        0.375,        0.0 ),
    Vec3d( 1.0,       0.3125,        0.0 ),
    Vec3d( 1.0,         0.25,        0.0 ),
    Vec3d( 1.0,       0.1875,        0.0 ),
    Vec3d( 1.0,        0.125,        0.0 ),
    Vec3d( 1.0,       0.0625,        0.0 ),
    Vec3d( 1.0,          0.0,        0.0 ),
    Vec3d( 0.9375,       0.0,        0.0 ),
    Vec3d( 0.875,        0.0,        0.0 ),
    Vec3d( 0.8125,       0.0,        0.0 ),
    Vec3d( 0.75,         0.0,        0.0 ),
    Vec3d( 0.6875,       0.0,        0.0 ),
    Vec3d( 0.625,        0.0,        0.0 ),
    Vec3d( 0.5625,       0.0,        0.0 )
};


// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template< class DataTypes>
void TriangularFEMForceField<DataTypes>::TRQSTriangleCreationFunction(int triangleIndex, void* param, TriangleInformation &/*tinfo*/,	const Triangle& t, const sofa::helper::vector< unsigned int > &, const sofa::helper::vector< double >&)
{
    TriangularFEMForceField<DataTypes> *ff= (TriangularFEMForceField<DataTypes> *)param;
    if (ff)
    {

        Index a = t[0];
        Index b = t[1];
        Index c = t[2];

        switch(ff->method)
        {
        case SMALL :
            ff->initSmall(triangleIndex,a,b,c);
            ff->computeMaterialStiffness(triangleIndex,a,b,c);
            break;

        case LARGE :
            ff->initLarge(triangleIndex,a,b,c);
            ff->computeMaterialStiffness(triangleIndex,a,b,c);
            break;
        }
    }
}


// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
TriangularFEMForceField<DataTypes>::TriangularFEMForceField()
    : method(LARGE)
    , f_method(initData(&f_method,std::string("large"),"method","large: large displacements, small: small displacements"))
    , f_poisson(initData(&f_poisson,(Real)0.3,"poissonRatio","Poisson ratio in Hooke's law"))
    , f_young(initData(&f_young,(Real)1000.,"youngModulus","Young modulus in Hooke's law"))
    , f_damping(initData(&f_damping,(Real)0.,"damping","Ratio damping/stiffness"))
    , f_fracturable(initData(&f_fracturable,false,"fracturable","the forcefield computes the next fracturable Edge"))
    , showStressValue(initData(&showStressValue,false,"showStressValue","Flag activating rendering of stress values as a color in each triangle"))
    , showStressVector(initData(&showStressVector,false,"showStressVector","Flag activating rendering of stress directions within each triangle"))
{
    _anisotropicMaterial = false;
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes> void TriangularFEMForceField<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=_topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=_topology->lastChange();

    triangleInfo.handleTopologyEvents(itBegin,itEnd);
    edgeInfo.handleTopologyEvents(itBegin,itEnd);
    vertexInfo.handleTopologyEvents(itBegin,itEnd);
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
TriangularFEMForceField<DataTypes>::~TriangularFEMForceField()
{
}

// --------------------------------------------------------------------------------------
// --- Initialization stage
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::init()
{
    serr << "initializing TriangularFEMForceField" << sendl;
    this->Inherited::init();

    _topology = getContext()->getMeshTopology();

    if (f_method.getValue() == "small")
        method = SMALL;
    else if (f_method.getValue() == "large")
        method = LARGE;

    if (_topology->getNbTriangles()==0)
    {
        serr << "ERROR(TriangularFEMForceField): object must have a Triangular Set Topology."<<sendl;
        return;
    }

    lastFracturedEdgeIndex = -1;

    reinit();
}

// --------------------------------------------------------------------------------------
// --- Re-initialization (called when we change a parameter through the GUI)
// --------------------------------------------------------------------------------------
template <class DataTypes>void TriangularFEMForceField<DataTypes>::reinit()
{
    if (f_method.getValue() == "small")
        method = SMALL;
    else if (f_method.getValue() == "large")
        method = LARGE;

    helper::vector<EdgeInformation>& edgeInf = *(edgeInfo.beginEdit());

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    /// prepare to store info in the triangle array
    triangleInf.resize(_topology->getNbTriangles());
    /// prepare to store info in the edge array
    edgeInf.resize(_topology->getNbEdges());
    unsigned int nbPoints = _topology->getNbPoints();
    helper::vector<VertexInformation>& vi = *(vertexInfo.beginEdit());
    vi.resize(nbPoints);
    vertexInfo.endEdit();
    // set initial position of the nodes
    _initialPoints = this->mstate->getX0();

    for (int i=0; i<_topology->getNbTriangles(); ++i)
        TRQSTriangleCreationFunction(i, (void*) this, triangleInf[i],  _topology->getTriangle(i),  (const sofa::helper::vector< unsigned int > )0, (const sofa::helper::vector< double >)0);

    triangleInfo.setCreateFunction(TRQSTriangleCreationFunction);
    triangleInfo.setCreateParameter( (void *) this );
    triangleInfo.setDestroyParameter( (void *) this );

    edgeInfo.endEdit();
    triangleInfo.endEdit();
}


// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
double TriangularFEMForceField<DataTypes>::getPotentialEnergy(const VecCoord& /*x*/)
{
    serr<<"TriangularFEMForceField::getPotentialEnergy-not-implemented !!!"<<sendl;
    return 0;
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeRotationLarge( Transformation &r, const VecCoord &p, const Index &a, const Index &b, const Index &c)
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::computeRotationLarge"<<sendl;
#endif

    // first vector on first edge
    // second vector in the plane of the two first edges
    // third vector orthogonal to first and second

    Coord edgex = p[b] - p[a];
    edgex.normalize();

    Coord edgey = p[c] - p[a];
    edgey.normalize();

    Coord edgez;
    edgez = cross(edgex, edgey);
    edgez.normalize();

    edgey = cross(edgez, edgex);
    edgey.normalize();

    r[0][0] = edgex[0];
    r[0][1] = edgex[1];
    r[0][2] = edgex[2];
    r[1][0] = edgey[0];
    r[1][1] = edgey[1];
    r[1][2] = edgey[2];
    r[2][0] = edgez[0];
    r[2][1] = edgez[1];
    r[2][2] = edgez[2];
}

// --------------------------------------------------------------------------------------
// --- Store the initial position of the nodes
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::initSmall(int i, Index&a, Index&b, Index&c)
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::initSmall"<<sendl;
#endif

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    TriangleInformation *tinfo = &triangleInf[i];

    tinfo->initialTransformation.identity();

    tinfo->rotatedInitialElements[0] = (*_initialPoints)[a] - (*_initialPoints)[a]; // always (0,0,0)
    tinfo->rotatedInitialElements[1] = (*_initialPoints)[b] - (*_initialPoints)[a];
    tinfo->rotatedInitialElements[2] = (*_initialPoints)[c] - (*_initialPoints)[a];

    computeStrainDisplacement(tinfo->strainDisplacementMatrix, tinfo->rotatedInitialElements[0], tinfo->rotatedInitialElements[1], tinfo->rotatedInitialElements[2]);

    triangleInfo.endEdit();
}

// --------------------------------------------------------------------------------------
// --- Store the initial position of the nodes
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::initLarge(int i, Index&a, Index&b, Index&c)
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::initLarge"<<sendl;
#endif

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    TriangleInformation *tinfo = &triangleInf[i];

    // Rotation matrix (initial triangle/world)
    // first vector on first edge
    // second vector in the plane of the two first edges
    // third vector orthogonal to first and second
    Transformation R_0_1;

    computeRotationLarge( R_0_1, (*_initialPoints), a, b, c );

    tinfo->initialTransformation = R_0_1;

    tinfo->rotatedInitialElements[0] = R_0_1 * ((*_initialPoints)[a] - (*_initialPoints)[a]); // always (0,0,0)
    tinfo->rotatedInitialElements[1] = R_0_1 * ((*_initialPoints)[b] - (*_initialPoints)[a]);
    tinfo->rotatedInitialElements[2] = R_0_1 * ((*_initialPoints)[c] - (*_initialPoints)[a]);

    triangleInfo.endEdit();
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::applyStiffness( VecCoord& v, Real h, const VecCoord& x )
{
    if (method == SMALL)
        applyStiffnessSmall( v,h,x );
    else
        applyStiffnessLarge( v,h,x );
}

// ---------------------------------------------------------------------------------------------------------------
// ---	Compute displacement vector D as the difference between current current position 'p' and initial position
// ---------------------------------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeDisplacementSmall(Displacement &D, Index elementIndex, const VecCoord &p)
{
    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];

    Coord deforme_a = Coord(0,0,0);
    Coord deforme_b = p[b]-p[a];
    Coord deforme_c = p[c]-p[a];

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    D[0] = 0;
    D[1] = 0;
    D[2] = triangleInf[elementIndex].rotatedInitialElements[1][0] - deforme_b[0];
    D[3] = triangleInf[elementIndex].rotatedInitialElements[1][1] - deforme_b[1];
    D[4] = triangleInf[elementIndex].rotatedInitialElements[2][0] - deforme_c[0];
    D[5] = triangleInf[elementIndex].rotatedInitialElements[2][1] - deforme_c[1];
    triangleInfo.endEdit();
}

// -------------------------------------------------------------------------------------------------------------
// --- Compute displacement vector D as the difference between current current position 'p' and initial position
// --- expressed in the co-rotational frame of reference
// -------------------------------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeDisplacementLarge(Displacement &D, Index elementIndex, const Transformation &R_0_2, const VecCoord &p)
{
    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];

    // positions of the deformed and displaced triangle in its frame
    Coord deforme_a = Coord(0,0,0);
    Coord deforme_b = R_0_2 * (p[b]-p[a]);
    Coord deforme_c = R_0_2 * (p[c]-p[a]);

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    // displacements
    D[0] = 0;
    D[1] = 0;
    D[2] = triangleInf[elementIndex].rotatedInitialElements[1][0] - deforme_b[0];
    D[3] = 0;
    D[4] = triangleInf[elementIndex].rotatedInitialElements[2][0] - deforme_c[0];
    D[5] = triangleInf[elementIndex].rotatedInitialElements[2][1] - deforme_c[1];

    triangleInfo.endEdit();
}

// ------------------------------------------------------------------------------------------------------------
// --- Compute the strain-displacement matrix where (a, b, c) are the coordinates of the 3 nodes of a triangle
// ------------------------------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStrainDisplacement( StrainDisplacement &J, Coord a, Coord b, Coord c )
{
    Real determinant;

    if (method == SMALL)
    {
        Coord ab_cross_ac = cross(b-a, c-a);
        determinant = ab_cross_ac.norm();

        Real x13 = (a[0]-c[0]) / determinant;
        Real x21 = (b[0]-a[0]) / determinant;
        Real x32 = (c[0]-b[0]) / determinant;
        Real y12 = (a[1]-b[1]) / determinant;
        Real y23 = (b[1]-c[1]) / determinant;
        Real y31 = (c[1]-a[1]) / determinant;

        J[0][0] = y23;
        J[0][1] = 0;
        J[0][2] = x32;

        J[1][0] = 0;
        J[1][1] = x32;
        J[1][2] = y23;

        J[2][0] = y31;
        J[2][1] = 0;
        J[2][2] = x13;

        J[3][0] = 0;
        J[3][1] = x13;
        J[3][2] = y31;

        J[4][0] = y12;
        J[4][1] = 0;
        J[4][2] = x21;

        J[5][0] = 0;
        J[5][1] = x21;
        J[5][2] = y12;
    }
    else
    {
        determinant = b[0] * c[1];

        Real x13 = -c[0] / determinant; // since a=(0,0)
        Real x21 = b[0] / determinant; // since a=(0,0)
        Real x32 = (c[0]-b[0]) / determinant;
        Real y12 = 0;		// since a=(0,0) and b[1] = 0
        Real y23 = -c[1] / determinant; // since a=(0,0) and b[1] = 0
        Real y31 = c[1] / determinant; // since a=(0,0)

        J[0][0] = y23;
        J[0][1] = 0;
        J[0][2] = x32;

        J[1][0] = 0;
        J[1][1] = x32;
        J[1][2] = y23;

        J[2][0] = y31;
        J[2][1] = 0;
        J[2][2] = x13;

        J[3][0] = 0;
        J[3][1] = x13;
        J[3][2] = y31;

        J[4][0] = y12;
        J[4][1] = 0;
        J[4][2] = x21;

        J[5][0] = 0;
        J[5][1] = x21;
        J[5][2] = y12;
    }
}

// --------------------------------------------------------------------------------------------------------
// --- Strain = StrainDisplacement * Displacement = JtD = Bd
// --------------------------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStrain(Vec<3,Real> &strain, const StrainDisplacement &J, const Displacement &D)
{
    Mat<3,6,Real> Jt;
    Jt.transpose(J);

    if (_anisotropicMaterial || method == SMALL)
    {
        strain = Jt * D;
    }
    else
    {
        strain[0] = Jt[0][0] * D[0] + /* Jt[0][1] * Depl[1] + */ Jt[0][2] * D[2] /* + Jt[0][3] * Depl[3] + Jt[0][4] * Depl[4] + Jt[0][5] * Depl[5] */ ;
        strain[1] = /* Jt[1][0] * Depl[0] + */ Jt[1][1] * D[1] + /* Jt[1][2] * Depl[2] + */ Jt[1][3] * D[3] + /* Jt[1][4] * Depl[4] + */ Jt[1][5] * D[5];
        strain[2] = Jt[2][0] * D[0] + Jt[2][1] * D[1] + Jt[2][2] * D[2] +	Jt[2][3] * D[3] + Jt[2][4] * D[4] /* + Jt[2][5] * Depl[5] */ ;
    }
}

// --------------------------------------------------------------------------------------------------------
// --- Stress = K * Strain = KJtD = KBd
// --------------------------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStress(Vec<3,Real> &stress, MaterialStiffness &K, Vec<3,Real> &strain)
{
    if (_anisotropicMaterial || method == SMALL)
    {
        stress = K * strain;
    }
    else
    {
        // Optimisations: The following values are 0 (per computeMaterialStiffnesses )
        // K[0][2]  K[1][2]  K[2][0] K[2][1]
        stress[0] = K[0][0] * strain[0] + K[0][1] * strain[1] + K[0][2] * strain[2];
        stress[1] = K[1][0] * strain[0] + K[1][1] * strain[1] + K[1][2] * strain[2];
        stress[2] = K[2][0] * strain[0] + K[2][1] * strain[1] + K[2][2] * strain[2];
    }
}

// --------------------------------------------------------------------------------------
// ---	Compute direction of maximum strain (strain = JtD = BD)
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computePrincipalStrain(Index elementIndex, Vec<3,Real> &strain )
{
    NEWMAT::SymmetricMatrix e(2);
    e = 0.0;

    NEWMAT::DiagonalMatrix D(2);
    D = 0.0;

    NEWMAT::Matrix V(2,2);
    V = 0.0;

    e(1,1) = strain[0];
    e(1,2) = strain[2];
    e(2,1) = strain[2];
    e(2,2) = strain[1];

    NEWMAT::Jacobi(e, D, V);

    Coord v((Real)V(1,1), (Real)V(2,1), 0.0);
    v.normalize();

//	Index a = _topology->getTriangle(elementIndex)[0];
//	Index b = _topology->getTriangle(elementIndex)[1];
//	Index c = _topology->getTriangle(elementIndex)[2];

//	const VecCoord& x = *this->mstate->getX();

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    triangleInf[elementIndex].maxStrain = (Real)D(1,1);
//	triangleInfo[elementIndex].principalStrainDirection = (x[b]-x[a])*v[0] + (x[c]-x[a])*v[1];
    triangleInf[elementIndex].principalStrainDirection = triangleInf[elementIndex].rotation * Coord(v[0], v[1], v[2]);
    triangleInf[elementIndex].principalStrainDirection *= triangleInf[elementIndex].maxStrain/100.0;

    triangleInfo.endEdit();
}

// --------------------------------------------------------------------------------------
// ---	Compute direction of maximum stress (stress = KJtD = KBD)
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computePrincipalStress(Index elementIndex, Vec<3,Real> &stress)
{

    NEWMAT::SymmetricMatrix e(2);
    e = 0.0;

    NEWMAT::DiagonalMatrix D(2);
    D = 0.0;

    NEWMAT::Matrix V(2,2);
    V = 0.0;

    e(1,1) = stress[0];
    e(1,2) = stress[2];
    e(2,1) = stress[2];
    e(2,2) = stress[1];

    NEWMAT::Jacobi(e, D, V);

    Coord v((Real)V(1,1), (Real)V(2,1), 0.0);
    v.normalize();

//	Index a = _topology->getTriangle(elementIndex)[0];
//	Index b = _topology->getTriangle(elementIndex)[1];
//	Index c = _topology->getTriangle(elementIndex)[2];

//	const VecCoord& x = *this->mstate->getX();

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    triangleInf[elementIndex].maxStress = (Real)D(1,1);
//	triangleInfo[elementIndex].principalStressDirection = (x[b]-x[a])*v[0] + (x[c]-x[a])*v[1];
    triangleInf[elementIndex].principalStressDirection = triangleInf[elementIndex].rotation * Coord(v[0], v[1], v[2]);
    triangleInf[elementIndex].principalStressDirection *= triangleInf[elementIndex].maxStress/100.0;

    triangleInfo.endEdit();
}

// --------------------------------------------------------------------------------------
// ---	Compute material stiffness
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeMaterialStiffness(int i, Index &/*a*/, Index &/*b*/, Index &/*c*/)
{
    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    TriangleInformation *tinfo = &triangleInf[i];

    tinfo->materialMatrix[0][0] = 1;
    tinfo->materialMatrix[0][1] = f_poisson.getValue();
    tinfo->materialMatrix[0][2] = 0;
    tinfo->materialMatrix[1][0] = f_poisson.getValue();
    tinfo->materialMatrix[1][1] = 1;
    tinfo->materialMatrix[1][2] = 0;
    tinfo->materialMatrix[2][0] = 0;
    tinfo->materialMatrix[2][1] = 0;
    tinfo->materialMatrix[2][2] = 0.5f * (1 - f_poisson.getValue());

    tinfo->materialMatrix *= (f_young.getValue() / (12 * (1 - f_poisson.getValue() * f_poisson.getValue())));

    triangleInfo.endEdit();
}

// --------------------------------------------------------------------------------------
// ---	Compute F = J * stress;
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeForce(Displacement &F, Index elementIndex, const VecCoord &p)
{
//	sofa::helper::system::thread::Trace::print(1, "Hello from computeForce()\n");

    Displacement D;
    StrainDisplacement J;
    Vec<3,Real> strain;
    Vec<3,Real> stress;
    Transformation R_0_2, R_2_0;

    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    if (method == SMALL)   // classic linear elastic method
    {
        computeDisplacementSmall(D, elementIndex, p);
        if (_anisotropicMaterial)
            computeStrainDisplacement(J, Coord(0,0,0), (p[b]-p[a]), (p[c]-p[a]));
        else
            J = triangleInf[elementIndex].strainDisplacementMatrix;
        computeStrain(strain, J, D);
        computeStress(stress, triangleInf[elementIndex].materialMatrix, strain);
        F = J * stress;

        // store newly computed values for next time
        triangleInf[elementIndex].strainDisplacementMatrix = J;
        triangleInf[elementIndex].strain = strain;
        triangleInf[elementIndex].stress = stress;
    }
    else   // co-rotational method
    {
        // first, compute rotation matrix into co-rotational frame
        computeRotationLarge( R_0_2, p, a, b, c);
        // then compute displacement in this frame
        computeDisplacementLarge(D, elementIndex, R_0_2, p);
        // and compute postions of a, b, c in the co-rotational frame
        Coord A = Coord(0, 0, 0);
        Coord B = R_0_2 * (p[b]-p[a]);
        Coord C = R_0_2 * (p[c]-p[a]);

        computeStrainDisplacement(J, A, B, C);
        computeStrain(strain, J, D);
        computeStress(stress, triangleInf[elementIndex].materialMatrix, strain);

//		sout << endl;
//		sout << "p[a]=(" << p[a] << ") - p[b]=(" << p[b] << ") - p[c]=(" << p[c] << ")" << endl;
//		sout << "D: " << D << endl;
//		sout << "J: " << J << endl;
//		sout << "Strain: " << strain << endl;
//		sout << "Stress: " << stress << endl;

        // Compute F = J * stress;
        // Optimisations: The following values are 0 (per computeStrainDisplacement )
        // J[0][1] J[1][0] J[2][1] J[3][0] J[4][0] J[4][1] J[5][0] J[5][2]

        F[0] = J[0][0] * stress[0] + /* J[0][1] * KJtD[1] + */ J[0][2] * stress[2];
        F[1] = /* J[1][0] * KJtD[0] + */ J[1][1] * stress[1] + J[1][2] * stress[2];
        F[2] = J[2][0] * stress[0] + /* J[2][1] * KJtD[1] + */ J[2][2] * stress[2];
        F[3] = /* J[3][0] * KJtD[0] + */ J[3][1] * stress[1] + J[3][2] * stress[2];
        F[4] = /* J[4][0] * KJtD[0] + J[4][1] * KJtD[1] + */ J[4][2] * stress[2];
        F[5] = /* J[5][0] * KJtD[0] + */ J[5][1] * stress[1] /* + J[5][2] * KJtD[2] */ ;

        // store newly computed values for next time
        R_2_0.transpose(R_0_2);
        triangleInf[elementIndex].strainDisplacementMatrix = J;
        triangleInf[elementIndex].rotation = R_2_0;
        triangleInf[elementIndex].strain = strain;
        triangleInf[elementIndex].stress = stress;
    }

    triangleInfo.endEdit();
}

/// Compute current stress
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStress(Vec<3,Real> &stress, Index elementIndex)
{
    //	sofa::helper::system::thread::Trace::print(1, "Hello from computeForce()\n");

    Displacement D;
    StrainDisplacement J;
    Vec<3,Real> strain;
    Transformation R_0_2, R_2_0;
    const VecCoord& p = *this->mstate->getX();
    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    if (method == SMALL)   // classic linear elastic method
    {
        R_0_2.identity();
        computeDisplacementSmall(D, elementIndex, p);
        if (_anisotropicMaterial)
            computeStrainDisplacement(J, Coord(0,0,0), (p[b]-p[a]), (p[c]-p[a]));
        else
            J = triangleInf[elementIndex].strainDisplacementMatrix;
        computeStrain(strain, J, D);
        computeStress(stress, triangleInf[elementIndex].materialMatrix, strain);
    }
    else   // co-rotational method
    {
        // first, compute rotation matrix into co-rotational frame
        computeRotationLarge( R_0_2, p, a, b, c);
        // then compute displacement in this frame
        computeDisplacementLarge(D, elementIndex, R_0_2, p);
        // and compute postions of a, b, c in the co-rotational frame
        Coord A = Coord(0, 0, 0);
        Coord B = R_0_2 * (p[b]-p[a]);
        Coord C = R_0_2 * (p[c]-p[a]);

        computeStrainDisplacement(J, A, B, C);
        computeStrain(strain, J, D);
        computeStress(stress, triangleInf[elementIndex].materialMatrix, strain);
    }
    // store newly computed values for next time
    R_2_0.transpose(R_0_2);
    triangleInf[elementIndex].strainDisplacementMatrix = J;
    triangleInf[elementIndex].rotation = R_2_0;
    triangleInf[elementIndex].strain = strain;
    triangleInf[elementIndex].stress = stress;

    triangleInfo.endEdit();
}
// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::accumulateDampingSmall(VecCoord&, Index )
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::accumulateDampingSmall"<<sendl;
#endif

}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::applyStiffnessSmall(VecCoord &v, Real h, const VecCoord &x)
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::applyStiffnessSmall"<<sendl;
#endif

    Mat<6,3,Real> J;
    Vec<3,Real> strain, stress;
    Displacement D, F;
    unsigned int nbTriangles=_topology->getNbTriangles();

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    for(unsigned int i=0; i<nbTriangles; i++)
    {
        Index a = _topology->getTriangle(i)[0];
        Index b = _topology->getTriangle(i)[1];
        Index c = _topology->getTriangle(i)[2];

        D[0] = x[a][0];
        D[1] = x[a][1];

        D[2] = x[b][0];
        D[3] = x[b][1];

        D[4] = x[c][0];
        D[5] = x[c][1];

        J = triangleInf[i].strainDisplacementMatrix;
        computeStrain(strain, J, D);
        computeStress(stress, triangleInf[i].materialMatrix, strain);
        F = J * stress;

        v[a] += Coord(-h*F[0], -h*F[1], 0);
        v[b] += Coord(-h*F[2], -h*F[3], 0);
        v[c] += Coord(-h*F[4], -h*F[5], 0);
    }
    triangleInfo.endEdit();
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::applyStiffnessLarge(VecCoord &v, Real h, const VecCoord &x)
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::applyStiffnessLarge"<<sendl;
#endif

    Mat<6,3,Real> J;
    Vec<3,Real> strain, stress;
    MaterialStiffness K;
    Displacement D;
    Coord x_2;
    unsigned int nbTriangles = _topology->getNbTriangles();

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    for(unsigned int i=0; i<nbTriangles; i++)
    {
        Index a = _topology->getTriangle(i)[0];
        Index b = _topology->getTriangle(i)[1];
        Index c = _topology->getTriangle(i)[2];

        Transformation R_0_2;
        R_0_2.transpose(triangleInf[i].rotation);

        VecCoord disp;
        disp.resize(3);

        x_2 = R_0_2 * x[a];
        disp[0] = x_2;

        D[0] = x_2[0];
        D[1] = x_2[1];

        x_2 = R_0_2 * x[b];
        disp[1] = x_2;
        D[2] = x_2[0];
        D[3] = x_2[1];

        x_2 = R_0_2 * x[c];
        disp[2] = x_2;
        D[4] = x_2[0];
        D[5] = x_2[1];

        Displacement F;

        K = triangleInf[i].materialMatrix;
        J = triangleInf[i].strainDisplacementMatrix;

        computeStrain(strain, J, D);
        computeStress(stress, triangleInf[i].materialMatrix, strain);

        F[0] = J[0][0] * stress[0] + /* J[0][1] * KJtD[1] + */ J[0][2] * stress[2];
        F[1] = /* J[1][0] * KJtD[0] + */ J[1][1] * stress[1] + J[1][2] * stress[2];
        F[2] = J[2][0] * stress[0] + /* J[2][1] * KJtD[1] + */ J[2][2] * stress[2];
        F[3] = /* J[3][0] * KJtD[0] + */ J[3][1] * stress[1] + J[3][2] * stress[2];
        F[4] = /* J[4][0] * KJtD[0] + J[4][1] * KJtD[1] + */ J[4][2] * stress[2];
        F[5] = /* J[5][0] * KJtD[0] + */ J[5][1] * stress[1] /* + J[5][2] * KJtD[2] */ ;

        v[a] += triangleInf[i].rotation * Coord(-h*F[0], -h*F[1], 0);
        v[b] += triangleInf[i].rotation * Coord(-h*F[2], -h*F[3], 0);
        v[c] += triangleInf[i].rotation * Coord(-h*F[4], -h*F[5], 0);
    }
    triangleInfo.endEdit();
}


// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::accumulateForceSmall( VecCoord &f, const VecCoord &p, Index elementIndex )
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::accumulateForceSmall"<<sendl;
#endif

    Displacement F;

    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];

    // compute force on element
    computeForce(F, elementIndex, p);

    f[a] += Coord( F[0], F[1], 0);
    f[b] += Coord( F[2], F[3], 0);
    f[c] += Coord( F[4], F[5], 0);
}


// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::accumulateForceLarge(VecCoord &f, const VecCoord &p, Index elementIndex )
{
#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::accumulateForceLarge"<<sendl;
#endif

    Displacement F;

    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];

    // compute force on element (in the co-rotational space)
    computeForce( F, elementIndex, p);

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    // transform force back into global ref. frame
    f[a] += triangleInf[elementIndex].rotation * Coord(F[0], F[1], 0);
    f[b] += triangleInf[elementIndex].rotation * Coord(F[2], F[3], 0);
    f[c] += triangleInf[elementIndex].rotation * Coord(F[4], F[5], 0);

    triangleInfo.endEdit();
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::getFractureCriteria(int elementIndex, Deriv& direction, Real& value)
{
    /// @TODO evaluate the criteria on the current position instead of relying on the computations during the force evaluation (based on the previous position)

    //computePrincipalStrain(elementIndex, J, D);

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    if ((unsigned)elementIndex < triangleInf.size())
    {
        helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());
        //direction = triangleInfo[elementIndex].principalStrainDirection;
        //value = fabs(triangleInfo[elementIndex].maxStrain);
        computePrincipalStress(elementIndex, triangleInf[elementIndex].stress);
        direction = triangleInf[elementIndex].principalStressDirection;
        value = fabs(triangleInf[elementIndex].maxStress);
        if (value < 0)
        {
            direction.clear();
            value = 0;
        }
        triangleInfo.endEdit();
    }
    else
    {
        direction.clear();
        value = 0;
    }
}

// ----------------------------------------------------------------------------------------------------------------------------------------
// ---	Compute value of stress along a given direction (typically the fiber direction and transverse direction in anisotropic materials)
// ----------------------------------------------------------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStressAlongDirection(Real &stress_along_dir, Index elementIndex, const Coord &dir, const Vec<3,Real> &stress)
{
    Mat<3,3,Real> R, Rt;

    helper::vector<TriangleInformation>& triangleInf = *(this->triangleInfo.beginEdit());

    // transform 'dir' into local coordinates
    R = triangleInf[elementIndex].rotation;
    Rt.transpose(R);
    Coord dir_local = Rt * dir;
    dir_local[2] = 0; // project direction
    dir_local.normalize();
    //	sout << "dir_local : " << dir_local << endl;

    // compute stress along specified direction 'dir'
    Real cos_theta = dir_local[0];
    Real sin_theta = dir_local[1];
    stress_along_dir = stress[0]*cos_theta*cos_theta + stress[1]*sin_theta*sin_theta + stress[2]*2*cos_theta*sin_theta;
    //sout << "computeStressAlongDirection :: stress ( " << stress << ") along local direction = (" << dir_local << ") = " <<  stress_along_dir << endl;
    triangleInfo.endEdit();
}

template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStressAcrossDirection(Real &stress_across_dir, Index elementIndex, const Coord &dir, const Vec<3,Real> &stress)
{
    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];
    const VecCoord& x = *this->mstate->getX();
    Coord n = cross(x[b]-x[a],x[c]-x[a]);
    Coord dir_t = cross(dir,n);
    this->computeStressAlongDirection(stress_across_dir, elementIndex, dir_t, stress);
}

template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStressAcrossDirection(Real &stress_across_dir, Index elementIndex, const Coord &dir)
{
    Index a = _topology->getTriangle(elementIndex)[0];
    Index b = _topology->getTriangle(elementIndex)[1];
    Index c = _topology->getTriangle(elementIndex)[2];
    const VecCoord& x = *this->mstate->getX();
    Coord n = cross(x[b]-x[a],x[c]-x[a]);
    Coord dir_t = cross(dir,n);
    this->computeStressAlongDirection(stress_across_dir, elementIndex, dir_t);
}


template <class DataTypes>
void TriangularFEMForceField<DataTypes>::computeStressAlongDirection(Real &stress_along_dir, Index elementIndex, const Coord &dir)
{
    Vec<3,Real> stress;
    this->computeStress(stress, elementIndex);
    this->computeStressAlongDirection(stress_along_dir, elementIndex, dir, stress);
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::accumulateDampingLarge(VecCoord &, Index )
{

#ifdef DEBUG_TRIANGLEFEM
    sout << "TriangularFEMForceField::accumulateDampingLarge"<<sendl;
#endif

}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::addForce(VecDeriv& f, const VecCoord& x, const VecDeriv& /*v*/)
{
    int nbTriangles=_topology->getNbTriangles();

    f.resize(x.size());

    if(f_damping.getValue() != 0)
    {
        if(method == SMALL)
        {
            for( int i=0; i<nbTriangles; i+=3 )
            {
                accumulateForceSmall( f, x, i/3 );
                accumulateDampingSmall( f, i/3 );
            }
        }
        else
        {
            for ( int i=0; i<nbTriangles; i+=3 )
            {
                accumulateForceLarge( f, x, i/3);
                accumulateDampingLarge( f, i/3 );
            }
        }
    }
    else
    {
        if (method==SMALL)
        {
            for(int i=0; i<nbTriangles; i+=1)
            {
                accumulateForceSmall( f, x, i );
            }
        }
        else
        {
            for ( int i=0; i<nbTriangles; i+=1)
            {
                accumulateForceLarge( f, x, i);
            }
        }
    }

    /*	if (f_fracturable.getValue())
    {
    // First Pass - Vertices Pass
    unsigned int nbPoints=_topology->getNbPoints();

    for( unsigned int i=0; i<nbPoints; i++ )
    {
    const sofa::helper::vector< unsigned int >& triangleNeighbors = _topology->getTrianglesAroundVertex(i);

    sofa::helper::vector< unsigned int >::const_iterator it = triangleNeighbors.begin();
    sofa::helper::vector< unsigned int >::const_iterator itEnd = triangleNeighbors.end();
    Coord meanStrainDirection, refStrainDirection;
    meanStrainDirection.clear();
    refStrainDirection.clear();

    bool b(true);

    while (it != itEnd)
    {
    if (b)
    {
    refStrainDirection = triangleInfo[*it].principalStrainDirection;
    if (refStrainDirection.norm() != 0.0)
    b=false;
    }

    (triangleInfo[*it].principalStrainDirection * refStrainDirection < 0)?
    meanStrainDirection -= triangleInfo[*it].principalStrainDirection : meanStrainDirection += triangleInfo[*it].principalStrainDirection;

    vertexInfo[i].sumEigenValues += triangleInfo[*it].eigenValue;

    ++it;
    }

    //meanStrainDirection.normalize();

    vertexInfo[i].meanStrainDirection = meanStrainDirection / (double)triangleNeighbors.size();
    vertexInfo[i].sumEigenValues = (double)vertexInfo[i].sumEigenValues / (double)triangleNeighbors.size();
    }


    // Second Pass - Edges Pass

    for(int i=0; i<_topology->getNbEdges(); i++ )
    edgeInfo[i].fracturable = false;

    if (nbPoints > 0)
    {
    double max = 0; //vertexInfo[0].sumEigenValues;
    int mostDeformableVertexIndex = -1; //0;

    for( unsigned int i=0; i<nbPoints; i++ )
    {
    bool vertexOnBorder = _topology->getTrianglesAroundVertex(i).size() < _topology->getEdgesAroundVertex(i).size() && _topology->getTrianglesAroundVertex(i).size() > 1;

    if (vertexOnBorder && vertexInfo[i].sumEigenValues > max)
    {
    mostDeformableVertexIndex = i;
    max = vertexInfo[i].sumEigenValues;
    }
    }

    //			std::vector< std::pair< double, unsigned int > > mostDeformableVertexIndexA(nbPoints);
    //			for( unsigned int i=0; i<nbPoints; i++ )
    //			{
    //				mostDeformableVertexIndexA[i].first = vertexInfo[i].sumEigenValues;
    //				mostDeformableVertexIndexA[i].second = i;
    //			}

    //			std::sort( mostDeformableVertexIndexA.begin(), mostDeformableVertexIndexA.end() );
    //		for( unsigned int i=0; i<nbPoints; i++ )
    //			{
    //				sout << mostDeformableVertexIndexA[i].first << " ";
    //			}
    //			sout << ""<<sendl;


    //			bool vertexOnBorder(false);
    //			int curIndex = nbPoints-1;

    //			while ((!vertexOnBorder)&&(curIndex>=0))
    //			{
    //				mostDeformableVertexIndex = mostDeformableVertexIndexA[curIndex].second;
    //				max=mostDeformableVertexIndexA[curIndex].first;
    // Check if the Vertex is on the border
    //				curIndex--;
    //			}

    // if (vertexOnBorder && (max > 0.05))

    const double THRESHOLD = 0.2;

    if ((mostDeformableVertexIndex!=-1) && (max > THRESHOLD))
    {
    //sout << "max=" << max << sendl;
    double minDotProduct = 1000.0;
    unsigned int fracturableIndex = 0;
    bool fracture(false);

    const sofa::helper::vector< unsigned int >& edgeNeighbors = _topology->getEdgesAroundVertex(mostDeformableVertexIndex);

    sofa::helper::vector< unsigned int >::const_iterator it = edgeNeighbors.begin();
    sofa::helper::vector< unsigned int >::const_iterator itEnd = edgeNeighbors.end();

    Index a;
    Index b;

    Coord n = vertexInfo[mostDeformableVertexIndex].meanStrainDirection;
    n.normalize();

    while (it != itEnd)
    {
    a = _topology->getEdge(*it)[0];
    b = _topology->getEdge(*it)[1];

    if (vertexInfo[mostDeformableVertexIndex].meanStrainDirection.norm() != 0.0)
    {
    Coord d = x[b]-x[a];
    d.normalize();
    if (fabs(n * d) < minDotProduct)
    {
    sofa::helper::vector< unsigned int > trianglesAroundEdge = _topology->getTrianglesAroundEdge(*it);
    if (trianglesAroundEdge.size() != 1)
    {

    //	bool bb(false);
    //	sofa::helper::vector< unsigned int >::iterator _it = trianglesAroundEdge.begin();
    //	sofa::helper::vector< unsigned int >::iterator _itEnd = trianglesAroundEdge.end();
    //	while (_it != _itEnd)
    //	{
    //		helper::fixed_array<unsigned int,3> edges = _topology->getEdgesInTriangle(*_it);

    //		int cptTest=0;
    //		for (int i=0; i<3; i++)
    //		{
    //			if (_topology->getTrianglesAroundEdge(edges[i]).size() < 2)
    //			{
    //				cptTest++;
    //			}
    //		}

    //		if (cptTest > 2)
    //		{
    //			if (max < 5 * THRESHOLD)
    //			{
    //				bb = true;
    //				break;
    //			}
    //		}

    //		++_it;
    //	}

    //	if (!bb)

    {
    minDotProduct = fabs(n * d);
    fracturableIndex = *it;
    fracture = true;
    }
    }
    }
    }

    ++it;
    }

    if (fracture) {
    //sout << "fracture at edge "<<fracturableIndex<<sendl;
    edgeInfo[fracturableIndex].fracturable = true;
    lastFracturedEdgeIndex = fracturableIndex;
    }
    }
    }
    }
    */

    //	sout << "EOF AddForce"<<sendl;
}

// --------------------------------------------------------------------------------------
// ---
// --------------------------------------------------------------------------------------
template <class DataTypes>
void TriangularFEMForceField<DataTypes>::addDForce(VecDeriv& df, const VecDeriv& dx)
{
    Real h=1;
    df.resize(dx.size());

    if (method == SMALL)
        applyStiffnessSmall( df,h,dx );
    else
        applyStiffnessLarge( df,h,dx );
}

template<class DataTypes>
int TriangularFEMForceField<DataTypes>::getFracturedEdge()
{
    helper::vector<EdgeInformation>& edgeInf = *(edgeInfo.beginEdit());

    if (f_fracturable.getValue())
    {
        int nbEdges = _topology->getNbEdges();

        for( int i=0; i<nbEdges; i++ )
        {
            if (edgeInf[i].fracturable)
            {
                return i;
            }
        }
    }

    edgeInfo.endEdit();

    return -1;
}


template<class DataTypes>
void TriangularFEMForceField<DataTypes>::draw()
{
    if (!getContext()->getShowForceFields())
        return;

    if (getContext()->getShowWireFrame())
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    const VecCoord& x = *this->mstate->getX();
    unsigned int nbTriangles=_topology->getNbTriangles();

    glDisable(GL_LIGHTING);
    if (!f_fracturable.getValue())
    {
        glBegin(GL_TRIANGLES);
        for(unsigned int i=0; i<nbTriangles; ++i)
        {
            Index a = _topology->getTriangle(i)[0];
            Index b = _topology->getTriangle(i)[1];
            Index c = _topology->getTriangle(i)[2];

            glColor4f(0,1,0,1);
            helper::gl::glVertexT(x[a]);
            glColor4f(0,0.5,0.5,1);
            helper::gl::glVertexT(x[b]);
            glColor4f(0,0,1,1);
            helper::gl::glVertexT(x[c]);
        }
        glEnd();
    }

    helper::vector<TriangleInformation>& triangleInf = *(triangleInfo.beginEdit());

    if (showStressVector.getValue() || showStressValue.getValue())
    {
        for(unsigned int i=0; i<nbTriangles; ++i)
            computePrincipalStress(i, triangleInf[i].stress);
    }

    if (showStressVector.getValue())
    {
        glColor4f(1,0,1,1);
        glBegin(GL_LINES);
        for(unsigned int i=0; i<nbTriangles; ++i)
        {
            Index a = _topology->getTriangle(i)[0];
            Index b = _topology->getTriangle(i)[1];
            Index c = _topology->getTriangle(i)[2];
            Coord center = (x[a]+x[b]+x[c])/3;
            Coord d = triangleInf[i].principalStressDirection*0.25;
            helper::gl::glVertexT(center-d);
            helper::gl::glVertexT(center+d);
        }
        glEnd();
    }

    if (showStressValue.getValue())
    {
        double max = fabs(triangleInf[0].maxStress);
        for( unsigned int i=1; i<nbTriangles; i++)
        {
            if (fabs(triangleInf[i].maxStress) > max)
                max = fabs(triangleInf[i].maxStress);
        }

        glBegin(GL_TRIANGLES);
        for(unsigned int i=0; i<nbTriangles; ++i)
        {
            Index a = _topology->getTriangle(i)[0];
            Index b = _topology->getTriangle(i)[1];
            Index c = _topology->getTriangle(i)[2];
            float v = (float)fabs(triangleInf[i].maxStress);
            v /= (float)(0.8*max);
            if (v > 1.0) v=1.0;
            Vec3d color = ColorMap[(int)(v*63)];
            glColor3dv(color.ptr());
            helper::gl::glVertexT(x[a]);
            helper::gl::glVertexT(x[b]);
            helper::gl::glVertexT(x[c]);
        }
        glEnd();

        /*
        		if (nbPoints > 0)
        		{
        			double max = vertexInfo[0].sumEigenValues;
        			unsigned int mostDeformableVertexIndex = 0;
        			for( unsigned int i=1; i<nbPoints; i++ )
        			{
        				if (vertexInfo[i].sumEigenValues > max)
        				{
        					mostDeformableVertexIndex = i;
        					max = vertexInfo[i].sumEigenValues;
        				}
        			}

        			glPointSize(8);
        			glBegin(GL_POINTS);
        			glColor4f(1,1,1,1);
        			helper::gl::glVertexT(x[mostDeformableVertexIndex]);
        			glEnd();
        			glPointSize(1);
        		}

        */
        /*
        			glBegin(GL_LINES);
        		for( unsigned int i=0; i<nbPoints; i++ )
        		{
        			glColor4f(1,0,1,1);
        			helper::gl::glVertexT(x[i]-vertexInfo[i].meanStrainDirection);
        			helper::gl::glVertexT(x[i]+vertexInfo[i].meanStrainDirection);
        			//helper::gl::glVertexT(x[i]-vertexInfo[i].meanStrainDirection * 2.5);
        			//helper::gl::glVertexT(x[i]+vertexInfo[i].meanStrainDirection * 2.5);
        		}
        			glEnd();
        */
    }
    /*

    		int nbEdges = _topology->getNbEdges();

    		for( unsigned int i=0; i<nbEdges; i++ )
    		{
    			if (edgeInfo[i].fracturable)
    			{
    				glLineWidth(7);
    				glBegin(GL_LINES);
    				glColor4f(1,0.5,0.25,1);
    				helper::gl::glVertexT(x[_topology->getEdge(i)[0]]);
    				helper::gl::glVertexT(x[_topology->getEdge(i)[1]]);
    				glEnd();
    				glLineWidth(1);
    			}
    		}
    */
//	}
    triangleInfo.endEdit();
    if (getContext()->getShowWireFrame())
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


SOFA_DECL_CLASS(TriangularFEMForceField)


// Register in the Factory
int TriangularFEMForceFieldClass = core::RegisterObject("Triangular finite elements")
#ifndef SOFA_FLOAT
        .add< TriangularFEMForceField<Vec3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< TriangularFEMForceField<Vec3fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_FORCEFIELD_API TriangularFEMForceField<Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_FORCEFIELD_API TriangularFEMForceField<Vec3fTypes>;
#endif


} // namespace forcefield

} // namespace component

} // namespace sofa
