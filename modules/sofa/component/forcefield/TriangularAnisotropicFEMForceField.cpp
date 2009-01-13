/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#define SOFA_COMPONENT_FORCEFIELD_TRIANGULARANISOTROPICFEMFORCEFIELD_CPP
#include <sofa/component/forcefield/TriangularAnisotropicFEMForceField.h>
#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/helper/gl/template.h>
#include <sofa/component/topology/TriangleData.inl>
#include <sofa/component/topology/EdgeData.inl>
#include <sofa/component/topology/PointData.inl>
#include <sofa/helper/system/gl.h>
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





SOFA_DECL_CLASS(TriangularAnisotropicFEMForceField)

template <class DataTypes>
TriangularAnisotropicFEMForceField<DataTypes>::TriangularAnisotropicFEMForceField()
    : f_young2(initData(&f_young2,(Real)(0.5*Inherited::f_young.getValue()),"transverseYoungModulus","Young modulus along transverse direction"))
    , f_theta(initData(&f_theta,(Real)(0.0),"fiberAngle","Fiber angle in global reference frame (in degrees)"))
    , f_fiberCenter(initData(&f_fiberCenter,"fiberCenter","Concentric fiber center in global reference frame"))
    , showFiber(initData(&showFiber,true,"showFiber","Flag activating rendering of fiber directions within each triangle"))
{
    this->_anisotropicMaterial = true;
}

template< class DataTypes>
void TriangularAnisotropicFEMForceField<DataTypes>::TRQSTriangleCreationFunction (int triangleIndex, void* param,
        TriangleInformation &/*tinfo*/,
        const Triangle& /*t*/,
        const sofa::helper::vector< unsigned int > &,
        const sofa::helper::vector< double >&)
{
    TriangularAnisotropicFEMForceField<DataTypes> *ff= (TriangularAnisotropicFEMForceField<DataTypes> *)param;
    if (ff)
    {

        const Triangle &t = ff->_topology->getTriangle(triangleIndex);

        Index a = t[0];
        Index b = t[1];
        Index c = t[2];

        switch(ff->method)
        {
        case SMALL :
            ff->initSmall(triangleIndex,a,b,c);
            ff->computeMaterialStiffness(triangleIndex, a, b, c);
            break;
        case LARGE :
            ff->initLarge(triangleIndex,a,b,c);
            ff->computeMaterialStiffness(triangleIndex, a, b, c);
            break;
        }
    }
}

template< class DataTypes>
void TriangularAnisotropicFEMForceField<DataTypes>::init()
{
    _topology = getContext()->getMeshTopology();

    Inherited::init();
    //reinit();
}

template <class DataTypes>void TriangularAnisotropicFEMForceField<DataTypes>::reinit()
{
    f_poisson2.setValue(Inherited::f_poisson.getValue()*(f_young2.getValue()/Inherited::f_young.getValue()));
    localFiberDirection.resize(_topology->getNbTriangles());
    Inherited::reinit();
}

template <class DataTypes>void TriangularAnisotropicFEMForceField<DataTypes>::handleTopologyChange()
{
    std::list<const TopologyChange *>::const_iterator itBegin=_topology->firstChange();
    std::list<const TopologyChange *>::const_iterator itEnd=_topology->lastChange();

    localFiberDirection.handleTopologyEvents(itBegin,itEnd);
    Inherited::handleTopologyChange();
}

template <class DataTypes>
void TriangularAnisotropicFEMForceField<DataTypes>::getFiberDir(int element, Deriv& dir)
{
    if ((unsigned)element < localFiberDirection.size())
    {
        const Deriv& ref = localFiberDirection[element];
        const VecCoord& x = *this->mstate->getX();
        topology::Triangle t = _topology->getTriangle(element);
        dir = (x[t[1]]-x[t[0]])*ref[0] + (x[t[2]]-x[t[0]])*ref[1];
    }
    else
    {
        dir.clear();
    }
}

template <class DataTypes>
void TriangularAnisotropicFEMForceField<DataTypes>::computeMaterialStiffness(int i, Index& v1, Index& v2, Index& v3)
{
    Real Q11, Q12, Q22, Q66;
    Coord fiberDirGlobal;  // orientation of the fiber in the global frame of reference

    Coord fiberDirLocalOrtho; //  // orientation of the fiber in the local orthonormal frame of the element
    Mat<3,3,Real> T, Tinv;
    TriangleInformation *tinfo = &Inherited::triangleInfo[i];

    Q11 = Inherited::f_young.getValue()/(1-Inherited::f_poisson.getValue()*f_poisson2.getValue());
    Q12 = Inherited::f_poisson.getValue()*f_young2.getValue()/(1-Inherited::f_poisson.getValue()*f_poisson2.getValue());
    Q22 = f_young2.getValue()/(1-Inherited::f_poisson.getValue()*f_poisson2.getValue());
    Q66 = (Real)(Inherited::f_young.getValue() / (2.0*(1 + Inherited::f_poisson.getValue())));

    //if (i >= (int) localFiberDirection.size())
    //	localFiberDirection.resize(i+1);

    T[0] = (*Inherited::_initialPoints)[v2]-(*Inherited::_initialPoints)[v1];
    T[1] = (*Inherited::_initialPoints)[v3]-(*Inherited::_initialPoints)[v1];
    T[2] = cross(T[0], T[1]);

    if (!f_fiberCenter.getValue().empty()) // in case we have concentric fibers
    {
        Coord tcenter = ((*Inherited::_initialPoints)[v1]+(*Inherited::_initialPoints)[v2]+(*Inherited::_initialPoints)[v3])*(Real)(1.0/3.0);
        Coord fcenter = f_fiberCenter.getValue()[0];
        fiberDirGlobal = cross(T[2], fcenter-tcenter);  // was fiberDir
    }
    else // for unidirectional fibers
    {
        double theta = (double)f_theta.getValue()*M_PI/180.0;
        fiberDirGlobal = Coord((Real)cos(theta), (Real)sin(theta), 0); // was fiberDir
    }

    Deriv& fiberDirLocal = localFiberDirection[i]; // orientation of the fiber in the local frame of the element (orthonormal frame)
    //[1] = cross(T[2], T[0]);
    //T[0].normalize();
    //T[1].normalize();
    //T[2].normalize();
    T.transpose();
    Tinv.invert(T);
    fiberDirLocal = Tinv * fiberDirGlobal;
    fiberDirLocal[2] = 0;
    fiberDirLocal.normalize();

    T[0] = (*Inherited::_initialPoints)[v2]-(*Inherited::_initialPoints)[v1];
    T[1] = (*Inherited::_initialPoints)[v3]-(*Inherited::_initialPoints)[v1];
    T[2] = cross(T[0], T[1]);
    T[1] = cross(T[2], T[0]);
    T[0].normalize();
    T[1].normalize();
    T[2].normalize();
    T.transpose();
    Tinv.invert(T);
    fiberDirLocalOrtho = Tinv * fiberDirGlobal;
    fiberDirLocalOrtho[2] = 0;
    fiberDirLocalOrtho.normalize();

    Real c, s, c2, s2, c3, s3,c4, s4;
    c = fiberDirLocalOrtho[0];
    s = fiberDirLocalOrtho[1];

    c2 = c*c;
    s2 = s*s;
    c3 = c2*c;
    s3 = s2*s;
    s4 = s2*s2;
    c4 = c2*c2;

    // K(1,1)=Q11*COS(THETA)^4 * + 2.0*(Q12+2*Q66)*SIN(THETA)^2*COS(THETA)^2 + Q22*SIN(THETA)^4 => c4*Q11+2*s2*c2*(Q12+2*Q66)+s4*Q22
    // K(1,2)=(Q11+Q22-4*Q66)*SIN(THETA)^2*COS(THETA)^2 + Q12*(SIN(THETA)^4+COS(THETA)^4) => s2*c2*(Q11+Q22-4*Q66) + (s4+c4)*Q12
    // K(2,1)=K(1,2)
    // K(2,2)=Q11*SIN(THETA)^4 + 2.0*(Q12+2*Q66)*SIN(THETA)^2*COS(THETA)^2 + Q22*COS(THETA)^4 => s4*Q11 + 2.0*s2*c2*(Q12+2*Q66) + c4*Q22
    // K(6,1)=(Q11-Q12-2*Q66)*SIN(THETA)*COS(THETA)^3 + (Q12-Q22+2*Q66)*SIN(THETA)^3*COS(THETA) => s*c3*(Q11-Q12-2*Q66)+s3*c*(Q12-Q22+2*Q66)
    // K(1,6)=K(6,1)
    // K(6,2)=(Q11-Q12-2*Q66)*SIN(THETA)^3*COS(THETA)+(Q12-Q22+2*Q66)*SIN(THETA)*COS(THETA)^3 => (Q11-Q12-2*Q66)*s3*c + (Q12-Q22+2*Q66)*s*c3
    // K(2,6)=K(6,2)
    // K(6,6)=(Q11+Q22-2*Q12-2*Q66)*SIN(THETA)^2 * COS(THETA)^2+ Q66*(SIN(THETA)^4+COS(THETA)^4) => (Q11+Q22-2*Q12-2*Q66)*s2*c2+ Q66*(s4+c4)

    Real K11= c4 * Q11 + 2 * c2 * s2 * (Q12+2*Q66) + s4 * Q22;
    Real K12 = c2 * s2 * (Q11+Q22-4*Q66) + (c4+s4) * Q12;
    Real K22 = s4* Q11 + 2 * c2 * s2 * (Q12+2*Q66) + c4 * Q22;
    Real K16 = s * c3 * (Q11-Q12-2*Q66) + s3 * c * (Q12-Q22+2*Q66);
    Real K26 = s3 * c * (Q11-Q12-2*Q66) + s * c3 * (Q12-Q22+2*Q66);
    Real K66 = c2 * s2 * (Q11+Q22-2*Q12-2*Q66) + (c4+s4) * Q66;

    tinfo->materialMatrix[0][0] = K11;
    tinfo->materialMatrix[0][1] = K12;
    tinfo->materialMatrix[0][2] = K16;
    tinfo->materialMatrix[1][0] = K12;
    tinfo->materialMatrix[1][1] = K22;
    tinfo->materialMatrix[1][2] = K26;
    tinfo->materialMatrix[2][0] = K16;
    tinfo->materialMatrix[2][1] = K26;
    tinfo->materialMatrix[2][2] = K66;

    tinfo->materialMatrix *= (Real)(1.0/12.0);

    //sout << "Young1=" << Inherited::f_young.getValue() << endl;
    //sout << "Young2=" << f_young2.getValue() << endl;
    //sout << "Poisson1=" << Inherited::f_poisson.getValue() << endl;
    //sout << "Poisson2=" << f_poisson2.getValue() << endl;
}

// ----------------------------------------------------------------
// ---	Display
// ----------------------------------------------------------------
template <class DataTypes>
void TriangularAnisotropicFEMForceField<DataTypes>::draw()
{
    glPolygonOffset(1.0, 2.0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    Inherited::draw();
    glDisable(GL_POLYGON_OFFSET_FILL);
    if (!getContext()->getShowForceFields())
        return;
    if (showFiber.getValue() && localFiberDirection.size() >= (unsigned)_topology->getNbTriangles())
    {
        const VecCoord& x = *this->mstate->getX();
        int nbTriangles=_topology->getNbTriangles();
        glColor3f(0,0,0);
        glBegin(GL_LINES);

        for(int i=0; i<nbTriangles; ++i)
        {
            Index a = _topology->getTriangle(i)[0];
            Index b = _topology->getTriangle(i)[1];
            Index c = _topology->getTriangle(i)[2];
            /*
            Mat<3,3,Real> T;

            T[0] = (*Inherited::_initialPoints)[b]-(*Inherited::_initialPoints)[a];
            T[1] = (*Inherited::_initialPoints)[c]-(*Inherited::_initialPoints)[a];
            T[2] = cross(T[0], T[1]);
            T[1] = cross(T[2], T[0]);
            T[1].normalize();

            Coord y = T[1];
            Coord ab = (*Inherited::_initialPoints)[b]-(*Inherited::_initialPoints)[a];
            y *= ab.norm();*/
            Coord center = (x[a]+x[b]+x[c])/3;
            //Coord d = (x[b]-x[a])*localFiberDirection[i][0] + y*localFiberDirection[i][1];
            Coord d = (x[b]-x[a])*localFiberDirection[i][0] + (x[c]-x[a])*localFiberDirection[i][1];
            d*=0.25;
            helper::gl::glVertexT(center-d);
            helper::gl::glVertexT(center+d);

            //Coord testDir(1, 0, 0);
            //Real s;
            //computeStressAlongDirection(s, i, testDir, Inherited::triangleInfo[i].stress);
        }
        glEnd();
    }
}


// Register in the Factory
int TriangularAnisotropicFEMForceFieldClass = core::RegisterObject("Triangular finite element model using anisotropic material")
#ifndef SOFA_FLOAT
        .add< TriangularAnisotropicFEMForceField<Vec3dTypes> >()
#endif
#ifndef SOFA_DOUBLE
        .add< TriangularAnisotropicFEMForceField<Vec3fTypes> >()
#endif
        ;

#ifndef SOFA_FLOAT
template class SOFA_COMPONENT_FORCEFIELD_API TriangularAnisotropicFEMForceField<Vec3dTypes>;
#endif
#ifndef SOFA_DOUBLE
template class SOFA_COMPONENT_FORCEFIELD_API TriangularAnisotropicFEMForceField<Vec3fTypes>;
#endif


} // namespace forcefield

} // namespace component

} // namespace sofa
