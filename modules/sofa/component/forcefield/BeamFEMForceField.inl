/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
*                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
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
#ifndef SOFA_COMPONENT_FORCEFIELD_BEAMFEMFORCEFIELD_INL
#define SOFA_COMPONENT_FORCEFIELD_BEAMFEMFORCEFIELD_INL

#include <sofa/core/componentmodel/behavior/ForceField.inl>
#include <sofa/component/forcefield/BeamFEMForceField.h>
#include <sofa/core/componentmodel/topology/BaseMeshTopology.h>
#include <sofa/component/topology/GridTopology.h>
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/helper/PolarDecompose.h>
#include <sofa/helper/gl/template.h>
#include <sofa/helper/gl/Axis.h>
#include <sofa/helper/rmath.h>
#include <assert.h>
#include <iostream>
#include <set>
#include <sofa/helper/system/gl.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <sofa/defaulttype/VecTypes.h>
#include <sofa/defaulttype/RigidTypes.h>
#include <sofa/simulation/tree/GNode.h>


using std::set;


namespace sofa
{

namespace component
{

namespace forcefield
{

using namespace sofa::core;
using namespace sofa::core::componentmodel;
using namespace sofa::defaulttype;

template <class DataTypes>
void BeamFEMForceField<DataTypes>::init()
{
    this->core::componentmodel::behavior::ForceField<DataTypes>::init();
    sofa::core::objectmodel::BaseContext* context = this->getContext();

    _topology = context->getMeshTopology();


    stiffnessContainer = context->core::objectmodel::BaseContext::get<StiffnessContainer>();
    lengthContainer = context->core::objectmodel::BaseContext::get<LengthContainer>();
    poissonContainer = context->core::objectmodel::BaseContext::get<PoissonContainer>();
    radiusContainer = context->core::objectmodel::BaseContext::get<RadiusContainer>();

    if (_topology==NULL)
    {
        serr << "ERROR(BeamFEMForceField): object must have a BaseMeshTopology (i.e. EdgeSetTopology or MeshTopology)."<<sendl;
        return;
    }
    else
    {
        if(_topology->getNbEdges()==0)
        {
            serr << "ERROR(BeamFEMForceField): topology is empty."<<sendl;
            return;
        }
        _indexedElements = &_topology->getEdges();
    }

    beamsData.setCreateFunction(BeamFEMEdgeCreationFunction);
    beamsData.setCreateParameter( (void *) this );
    beamsData.setDestroyParameter( (void *) this );

    reinit();
}

template <class DataTypes>
void BeamFEMForceField<DataTypes>::reinit()
{
    unsigned int n = _indexedElements->size();
    //_beamQuat.resize( n );
    //_stiffnessMatrices.resize( n );
    _forces.resize( this->mstate->getSize() );

    initBeams( n );
    for (unsigned int i=0; i<n; ++i)
        reinitBeam(i);
    std::cerr<<"reinitBeam Ok"<<std::endl;
    sout << "BeamFEMForceField: init OK, "<<n<<" elements."<<sendl;
}

template <class DataTypes>
void BeamFEMForceField<DataTypes>::reinitBeam(unsigned int i)
{

    double stiffness, length, radius, poisson;
    Index a = (*_indexedElements)[i][0];
    Index b = (*_indexedElements)[i][1];

    const VecCoord& x0 = *this->mstate->getX0();
//    sout << "Beam "<<i<<" : ("<<a<<' '<<b<<") : beamsData size = "<<beamsData.size()<<" mstate size = "<<this->mstate->getSize()<<" x0 size = "<<x0.size()<<sendl;
    //if (needInit)
    if (stiffnessContainer)
        stiffness = stiffnessContainer->getStiffness(i) ;
    else
        stiffness =  _youngModulus.getValue() ;

    //if (lengthContainer)
    //	length = lengthContainer->getLength(i) ;
    //else
    length = (x0[a].getCenter()-x0[b].getCenter()).norm() ;

    //if (radiusContainer)
    //	radius = radiusContainer->getRadius(i) ;
    //else
    radius = _radius.getValue() ;
    //if (poissonContainer)
    //	poisson = poissonContainer->getPoisson(i) ;
    //else
    poisson = _poissonRatio.getValue() ;


    setBeam(i, stiffness, length, poisson, radius );

    computeStiffness(i,a,b);

    initLarge(i,a,b);
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::BeamFEMEdgeCreationFunction(int edgeIndex, void* param, BeamInfo &ei,
        const topology::Edge& /*e*/,  const sofa::helper::vector< unsigned int > & /*a*/,
        const sofa::helper::vector< double >&)
{
//    sout << "Create beam "<<edgeIndex<<" ("<<e<<") from "<<a<<sendl;
    BeamFEMForceField<DataTypes>* p = static_cast<BeamFEMForceField<DataTypes>*>(param);
    // p->beamsData.resize(edgeIndex+1);
    static_cast<BeamFEMForceField<DataTypes>*>(param)->reinitBeam(edgeIndex);
    ei = p->beamsData.getValue()[edgeIndex];
    // p->beamsData.resize(edgeIndex);
}

template <class DataTypes>
void BeamFEMForceField<DataTypes>::handleTopologyChange()
{
    //_beamQuat.resize( _indexedElements->size() );

    std::list<const sofa::core::componentmodel::topology::TopologyChange *>::const_iterator itBegin=_topology->firstChange();
    std::list<const sofa::core::componentmodel::topology::TopologyChange *>::const_iterator itEnd=_topology->lastChange();

    beamsData.handleTopologyEvents(itBegin,itEnd);
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::addForce (VecDeriv& f, const VecCoord& p, const VecDeriv& /*v*/)
{

    //std::cout<<" BeamFEMForceField<DataTypes>::addForce  "<<std::endl;
    f.resize(p.size());
    //_beamQuat.resize( _indexedElements->size() );

    // First compute each node rotation
    unsigned int i;

    _nodeRotations.resize(p.size());
    //Mat3x3d R; R.identity();
    for(i=0; i<p.size(); ++i)
    {
        //R = R * MatrixFromEulerXYZ(p[i][3], p[i][4], p[i][5]);
        //_nodeRotations[i] = R;
        p[i].getOrientation().toMatrix(_nodeRotations[i]);
    }

    typename VecElement::const_iterator it;

    for(it=_indexedElements->begin(),i=0; it!=_indexedElements->end(); ++it,++i)
    {
        Index a = (*it)[0];
        Index b = (*it)[1];

        initLarge(i,a,b);
        accumulateForceLarge( f, p, i, a, b );
    }

}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::addDForce (VecDeriv& df, const VecDeriv& dx)
{
    df.resize(dx.size());
    //if(_assembling) applyStiffnessAssembled(v,x);
    //else
    {
        unsigned int i=0;
        typename VecElement::const_iterator it;

        for(it = _indexedElements->begin() ; it != _indexedElements->end() ; ++it, ++i)
        {
            Index a = (*it)[0];
            Index b = (*it)[1];

            applyStiffnessLarge( df, dx, i, a, b );
        }
    }

}

template<class DataTypes>
typename BeamFEMForceField<DataTypes>::Real BeamFEMForceField<DataTypes>::peudo_determinant_for_coef ( const Mat<2, 3, Real>&  M )
{
    return  M[0][1]*M[1][2] - M[1][1]*M[0][2] -  M[0][0]*M[1][2] + M[1][0]*M[0][2] + M[0][0]*M[1][1] - M[1][0]*M[0][1];
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::computeStiffness(int i, Index , Index )
{
    Real   phiy, phiz;
    Real _L = (Real)beamsData.getValue()[i]._L;
    Real _A = (Real)beamsData.getValue()[i]._A;
    Real _nu = (Real)beamsData.getValue()[i]._nu;
    Real _E = (Real)beamsData.getValue()[i]._E;
    Real _Iy = (Real)beamsData.getValue()[i]._Iy;
    Real _Iz = (Real)beamsData.getValue()[i]._Iz;
    Real _Asy = (Real)beamsData.getValue()[i]._Asy;
    Real _Asz = (Real)beamsData.getValue()[i]._Asz;
    Real _G = (Real)beamsData.getValue()[i]._G;
    Real _J = (Real)beamsData.getValue()[i]._J;
    Real L2 = (Real) (_L * _L);
    Real L3 = (Real) (L2 * _L);
    Real EIy = (Real)(_E * _Iy);
    Real EIz = (Real)(_E * _Iz);

    //std::cout<<" Young Modulus :"<<_E<<std::endl;

    // Find shear-deformation parameters
    if (_Asy == 0)
        phiy = 0.0;
    else
        phiy = (Real)(24.0*(1.0+_nu)*_Iz/(_Asy*L2));

    if (_Asz == 0)
        phiz = 0.0;
    else
        phiz = (Real)(24.0*(1.0+_nu)*_Iy/(_Asz*L2));

    helper::vector<BeamInfo>& bd = *(beamsData.beginEdit());
    StiffnessMatrix& k_loc = bd[i]._k_loc;

    // Define stiffness matrix 'k' in local coordinates
    k_loc.clear();
    k_loc[6][6]   = k_loc[0][0]   = _E*_A/_L;
    k_loc[7][7]   = k_loc[1][1]   = (Real)(12.0*EIz/(L3*(1.0+phiy)));
    k_loc[8][8]   = k_loc[2][2]   = (Real)(12.0*EIy/(L3*(1.0+phiz)));
    k_loc[9][9]   = k_loc[3][3]   = _G*_J/_L;
    k_loc[10][10] = k_loc[4][4]   = (Real)((4.0+phiz)*EIy/(_L*(1.0+phiz)));
    k_loc[11][11] = k_loc[5][5]   = (Real)((4.0+phiy)*EIz/(_L*(1.0+phiy)));

    k_loc[4][2]   = (Real)(-6.0*EIy/(L2*(1.0+phiz)));
    k_loc[5][1]   = (Real)( 6.0*EIz/(L2*(1.0+phiy)));
    k_loc[6][0]   = -k_loc[0][0];
    k_loc[7][1]   = -k_loc[1][1];
    k_loc[7][5]   = -k_loc[5][1];
    k_loc[8][2]   = -k_loc[2][2];
    k_loc[8][4]   = -k_loc[4][2];
    k_loc[9][3]   = -k_loc[3][3];
    k_loc[10][2]  = k_loc[4][2];
    k_loc[10][4]  = (Real)((2.0-phiz)*EIy/(_L*(1.0+phiz)));
    k_loc[10][8]  = -k_loc[4][2];
    k_loc[11][1]  = k_loc[5][1];
    k_loc[11][5]  = (Real)((2.0-phiy)*EIz/(_L*(1.0+phiy)));
    k_loc[11][7]  = -k_loc[5][1];

    for (int i=0; i<=10; i++)
        for (int j=i+1; j<12; j++)
            k_loc[i][j] = k_loc[j][i];

    beamsData.endEdit();
}

inline Quat qDiff(Quat a, const Quat& b)
{
    if (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3]<0)
    {
        a[0] = -a[0];
        a[1] = -a[1];
        a[2] = -a[2];
        a[3] = -a[3];
    }
    Quat q = b.inverse() * a;
    //sout << "qDiff("<<a<<","<<b<<")="<<q<<", bq="<<(b*q)<<sendl;
    return q;
}

////////////// large displacements method
template<class DataTypes>
void BeamFEMForceField<DataTypes>::initLarge(int i, Index a, Index b)
{
    const VecCoord& x = *this->mstate->getX();

    Quat quatA, quatB, dQ;
    Vec3d dW;

    quatA = x[a].getOrientation();
    quatB = x[b].getOrientation();



    dQ = qDiff(quatB, quatA);

    dW = dQ.toEulerVector();

    double Theta = dW.norm();

    if(Theta>0.0000001)
    {
        dW.normalize();

        beamQuat(i) = quatA*dQ.axisToQuat(dW, Theta/2);
        beamQuat(i).normalize();
    }
    else
        beamQuat(i)= quatA;

    beamsData.endEdit();
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::accumulateForceLarge( VecDeriv& f, const VecCoord & x, int i, Index a, Index b )
{
    const VecCoord& x0 = *this->mstate->getX0();

    beamQuat(i)= x[a].getOrientation();
    beamsData.endEdit();

    Vec<3,Real> u, P1P2, P1P2_0;
    // local displacement
    Displacement depl;

    // translations //
    P1P2_0 = x0[b].getCenter() - x0[a].getCenter();
    P1P2_0 = x0[a].getOrientation().inverseRotate(P1P2_0);
    P1P2 = x[b].getCenter() - x[a].getCenter();
    P1P2 = x[a].getOrientation().inverseRotate(P1P2);
    u = P1P2 - P1P2_0;

    depl[0] = 0.0; 	depl[1] = 0.0; 	depl[2] = 0.0;
    depl[6] = u[0]; depl[7] = u[1]; depl[8] = u[2];

    // rotations //
    Quat dQ0, dQ;

    // dQ = QA.i * QB ou dQ = QB * QA.i() ??
    dQ0 = qDiff(x0[b].getOrientation(), x0[a].getOrientation()); // x0[a].getOrientation().inverse() * x0[b].getOrientation();
    dQ =  qDiff(x[b].getOrientation(), x[a].getOrientation()); // x[a].getOrientation().inverse() * x[b].getOrientation();
    //u = dQ.toEulerVector() - dQ0.toEulerVector();
    u = qDiff(dQ,dQ0).toEulerVector(); //dQ.toEulerVector() - dQ0.toEulerVector();

    depl[3] = 0.0; 	depl[4] = 0.0; 	depl[5] = 0.0;
    depl[9] = u[0]; depl[10]= u[1]; depl[11]= u[2];

    // this computation can be optimised: (we know that half of "depl" is null)
    Displacement force = beamsData.getValue()[i]._k_loc * depl;


    // Apply lambda transpose (we use the rotation value of point a for the beam)

    Vec3d fa1 = x[a].getOrientation().rotate(Vec3d(force[0],force[1],force[2]));
    Vec3d fa2 = x[a].getOrientation().rotate(Vec3d(force[3],force[4],force[5]));

    Vec3d fb1 = x[a].getOrientation().rotate(Vec3d(force[6],force[7],force[8]));
    Vec3d fb2 = x[a].getOrientation().rotate(Vec3d(force[9],force[10],force[11]));


    f[a] += Deriv(-fa1,-fa2);
    f[b] += Deriv(-fb1,-fb2);

}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::applyStiffnessLarge( VecDeriv& df, const VecDeriv& dx, int i, Index a, Index b )
{
    //const VecCoord& x = *this->mstate->getX();

    Displacement local_depl;
    sofa::defaulttype::Vec<3,Real> u;
    const Quat& q = beamQuat(i); //x[a].getOrientation();

    u = q.inverseRotate(dx[a].getVCenter());
    local_depl[0] = u[0];
    local_depl[1] = u[1];
    local_depl[2] = u[2];

    u = q.inverseRotate(dx[a].getVOrientation());
    local_depl[3] = u[0];
    local_depl[4] = u[1];
    local_depl[5] = u[2];


    u = q.inverseRotate(dx[b].getVCenter());
    local_depl[6] = u[0];
    local_depl[7] = u[1];
    local_depl[8] = u[2];

    u = q.inverseRotate(dx[b].getVOrientation());
    local_depl[9] = u[0];
    local_depl[10] = u[1];
    local_depl[11] = u[2];

    Displacement local_force = beamsData.getValue()[i]._k_loc * local_depl;

    Vec3d fa1 = q.rotate(Vec3d(local_force[0],local_force[1] ,local_force[2] ));
    Vec3d fa2 = q.rotate(Vec3d(local_force[3],local_force[4] ,local_force[5] ));
    Vec3d fb1 = q.rotate(Vec3d(local_force[6],local_force[7] ,local_force[8] ));
    Vec3d fb2 = q.rotate(Vec3d(local_force[9],local_force[10],local_force[11]));

    df[a] += Deriv(-fa1,-fa2);
    df[b] += Deriv(-fb1,-fb2);
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::addKToMatrix(sofa::defaulttype::BaseMatrix *mat, SReal k, unsigned int &offset)
{
    //const VecCoord& x = *this->mstate->getX();

    unsigned int i=0;
    typename VecElement::const_iterator it;

    for(it = _indexedElements->begin() ; it != _indexedElements->end() ; ++it, ++i)
    {
        Index a = (*it)[0];
        Index b = (*it)[1];

        Displacement local_depl;
        Vec3d u;
        const Quat& q = beamQuat(i); //x[a].getOrientation();
        Transformation R,Rt;
        q.toMatrix(R);
        Rt.transpose(R);
        const StiffnessMatrix& K0 = beamsData.getValue()[i]._k_loc;
        StiffnessMatrix K;
        for (int x1=0; x1<12; x1+=3)
            for (int y1=0; y1<12; y1+=3)
            {
                Mat<3,3,Real> m;
                K0.getsub(x1,y1, m);
                m = R*m*Rt;
                K.setsub(x1,y1, m);
            }
        int index[12];
        for (int x1=0; x1<6; x1++)
            index[x1] = offset+a*6+x1;
        for (int x1=0; x1<6; x1++)
            index[6+x1] = offset+b*6+x1;
        for (int x1=0; x1<12; ++x1)
            for (int y1=0; y1<12; ++y1)
                mat->add(index[x1], index[y1], - K(x1,y1)*k);
    }
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::draw()
{
    if (!getContext()->getShowForceFields()) return;
    if (!this->mstate) return;

    const VecCoord& x = *this->mstate->getX();

    //sout << 	_indexedElements->size() << " edges, " << x.size() << " points."<<sendl;

    typename VecElement::const_iterator it;
    int i;

    std::vector< Vector3 > points[3];
    for(it = _indexedElements->begin(), i = 0 ; it != _indexedElements->end() ; ++it, ++i)
    {
        Index a = (*it)[0];
        Index b = (*it)[1];
        //sout << "edge " << i << " : "<<a<<" "<<b<<" = "<<x[a].getCenter()<<"  -  "<<x[b].getCenter()<<" = "<<beamsData[i]._L<<sendl;
        Vec3d p; p = (x[a].getCenter()+x[b].getCenter())*0.5;
        Vec3d beamVec;
        beamVec[0]=beamsData.getValue()[i]._L*0.5; beamVec[1] = 0.0; beamVec[2] = 0.0;

        const Quat& q = beamQuat(i);
        // axis X
        points[0].push_back(p - q.rotate(beamVec) );
        points[0].push_back(p + q.rotate(beamVec) );
        // axis Y
        beamVec[0]=0.0; beamVec[1] = beamsData.getValue()[i]._L*0.5;
        points[1].push_back(p );
        points[1].push_back(p + q.rotate(beamVec) );
        // axis Z
        beamVec[1]=0.0; beamVec[2] = beamsData.getValue()[i]._L*0.5;
        points[2].push_back(p);
        points[2].push_back(p + q.rotate(beamVec) );
    }
    simulation::tree::getSimulation()->DrawUtility.drawLines(points[0], 1, Vec<4,float>(1,0,0,1));
    simulation::tree::getSimulation()->DrawUtility.drawLines(points[1], 1, Vec<4,float>(0,1,0,1));
    simulation::tree::getSimulation()->DrawUtility.drawLines(points[2], 1, Vec<4,float>(0,0,1,1));

}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::initBeams(unsigned int size)
{
    helper::vector<BeamInfo>& bd = *(beamsData.beginEdit());
    bd.resize(size);
    beamsData.endEdit();
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::setBeam(unsigned int i, double E, double L, double nu, double r)
{
    helper::vector<BeamInfo>& bd = *(beamsData.beginEdit());
    bd[i].init(E,L,nu,r);
    beamsData.endEdit();
    //_indexedElements = &_topology->getEdges();
}

template<class DataTypes>
void BeamFEMForceField<DataTypes>::BeamInfo::init(double E, double L, double nu, double r)
{
    _E = E;
    _E0 = E;
    _nu = nu;
    _L = L;
    _r = r;

    _G=_E/(2.0*(1.0+_nu));
    _Iz = M_PI*r*r*r*r/4;
    _Iy = _Iz ;
    _J = _Iz+_Iy;
    _A = M_PI*r*r;

    _Asy = 0.0;
    _Asz = 0.0;

    //_k_loc.ReSize(12, 12);
    //_k_flex.ReSize(12,12);
    //_lambda.ReSize(12, 12);
    //_u_init.ReSize(12,1);
    //_u_actual.ReSize(12,1);
    //_f_k.ReSize(12,1);

    //_k_loc=0;
    //_u_init=0;

    //_Ke.ReSize(12,12);

    //localStiffness();
}
/*
void BeamInfo::localStiffness()
{
	int      i,j;
	double   phiy, phiz;
	double   L2 = _L * _L;
	double   L3 = L2 * _L;
	double   EIy = _E * _Iy;
	double   EIz = _E * _Iz;

	// Find shear-deformation parameters
	if (_Asy == 0)
		phiy = 0.0;
	else
		phiy = 24.0*(1.0+_nu)*_Iz/(_Asy*L2);

	if (_Asz == 0)
		phiz = 0.0;
	else
		phiz = 24.0*(1.0+_nu)*_Iy/(_Asz*L2);


	// Define stiffness matrix 'k' in local coordinates
	Try {
		_k_loc = 0;
		_k_loc(1,1)   = _E*_A/_L;
		_k_loc(2,2)   = 12.0*EIz/(L3*(1.0+phiy));
		_k_loc(3,3)   = 12.0*EIy/(L3*(1.0+phiz));
		_k_loc(4,4)   = _G*_J/_L;
		_k_loc(5,5)   = (4.0+phiz)*EIy/(_L*(1.0+phiz));
		_k_loc(6,6)   = (4.0+phiy)*EIz/(_L*(1.0+phiy));
		_k_loc(7,7)   = _k_loc(1,1);
		_k_loc(8,8)   = _k_loc(2,2);
		_k_loc(9,9)   = _k_loc(3,3);
		_k_loc(10,10) = _k_loc(4,4);
		_k_loc(11,11) = _k_loc(5,5);
		_k_loc(12,12) = _k_loc(6,6);

		_k_loc(5,3)   = -6.0*EIy/(L2*(1.0+phiz));
		_k_loc(6,2)   =  6.0*EIz/(L2*(1.0+phiy));
		_k_loc(7,1)   = -_k_loc(1,1);
		_k_loc(8,2)   = -_k_loc(2,2);
		_k_loc(8,6)   = -_k_loc(6,2);
		_k_loc(9,3)   = -_k_loc(3,3);
		_k_loc(9,5)   = -_k_loc(5,3);
		_k_loc(10,4)  = -_k_loc(4,4);
		_k_loc(11,3)  = _k_loc(5,3);
		_k_loc(11,5)  = (2.0-phiz)*EIy/(_L*(1.0+phiz));
		_k_loc(11,9)  = -_k_loc(5,3);
		_k_loc(12,2)  = _k_loc(6,2);
		_k_loc(12,6)  = (2.0-phiy)*EIz/(_L*(1.0+phiy));
		_k_loc(12,8)  = -_k_loc(6,2);

		for (i=1; i<=11; i++)
			for (j=i+1; j<=12; j++)
				_k_loc(i,j) = _k_loc(j,i);

		_k_flex = _k_loc;

// 		for (i=1; i<=12; i++)
// 			_k_flex(i,i) = FLEXIBILITY * _k_flex(i,i);

	}
	CatchAll { sout << "ERROR while computing '_k_loc'" << NewMAT::Exception::what() << endl;
	}
}
*/
/*
template<class DataTypes>
void BeamFEMForceField<DataTypes>::computeUinit(unsigned int i, Vec3d &P1, Vec3d &P2, Vec3d &LoX1, Vec3d &LoY1, Vec3d &LoZ1, Vec3d &LoX2, Vec3d &LoY2, Vec3d &LoZ2)
{
	beamsData[i].computeUinit(P1, P2, LoX1, LoY1, LoZ1, LoX2, LoY2, LoZ2);
}

void BeamInfo::computeUinit(Vec3d &P1, Vec3d &P2, Vec3d &LoX1, Vec3d &LoY1, Vec3d &LoZ1, Vec3d &LoX2, Vec3d &LoY2, Vec3d &LoZ2)
{
	Vec3d        p2; //position of point2 in local frame (LoX1,LoY1,LoZ1);
	Vec3d      P1P2, p1p2, p1p2_init, u_init;
	Vec3d      LoX1_loc, LoY1_loc, LoZ1_loc, LoX2_loc, LoY2_loc, LoZ2_loc;
	sofa::defaulttype::Mat3x3d    R1;
	NewMAT::ColumnVector Finit(9);
	NewMAT::ColumnVector Uinit(9);
	NewMAT::Matrix K(9,9);

	Finit=0; Uinit=0; K=0;

	LoX1_loc = Vec3d(1.0,0.0,0.0);
	LoY1_loc = Vec3d(0.0,1.0,0.0);
	LoZ1_loc = Vec3d(0.0,0.0,1.0);

 	Try{
		K.SubMatrix(1,6,1,6) = _k_loc.SubMatrix(7,12,7,12);
	}
	CatchAll {
		sout << "ERROR while computing 'K' in ComputeUinit" << NewMAT::Exception::what() << endl;
	}

	P1P2 = P2 - P1;
	p1p2_init = Vec3d(_L,0.0,0.0); //the beam is aligned along LoX1 if there is no initial deformation

	R1[0] = LoX1;
	R1[1] = LoY1;
	R1[2] = LoZ1;

	p1p2 = R1*P1P2;

	u_init = p1p2 - p1p2_init;

	// add lagrange multipliers:
	K(7,1) = 1.0; K(1,7) = 1.0;
	K(8,2) = 1.0; K(2,8) = 1.0;
	K(9,3) = 1.0; K(3,9) = 1.0;

	//_u_init must respect displacements
	Finit(7) = u_init[0];
	Finit(8) = u_init[1];
	Finit(9) = u_init[2];

	Try
	{
		Uinit = K.i() *	 Finit;
	}
	CatchAll
	{
		sout << "ERROR while computing 'Uinit = K.i() * Finit' in ComputeUinit" << NewMAT::Exception::what() << endl;
	}
	_u_init=0.0;
 	_u_init.SubMatrix(7,12,1,1) = Uinit.SubMatrix(1,6,1,1);
	Vec3d vtemp = Vec3d(Uinit(4), Uinit(5), Uinit(6));

	if (vtemp == Vec3d(0.0,0.0,0.0))
		vtemp = Vec3d(0.0,0.0,1.0);

	Quaternion q(vtemp, vtemp.norm());
	vtemp.normalize();

	LoX2_loc = q.rotate(LoX1_loc);
	LoY2_loc = q.rotate(LoY1_loc);
	LoZ2_loc = q.rotate(LoZ1_loc);

	LoX2 = LoX1 * LoX2_loc[0] + LoY1 * LoX2_loc[1] + LoZ1 * LoX2_loc[2] ;
	LoY2 = LoX1 * LoY2_loc[0] + LoY1 * LoY2_loc[1] + LoZ1 * LoY2_loc[2] ;
	LoZ2 = LoX1 * LoZ2_loc[0] + LoY1 * LoZ2_loc[1] + LoZ1 * LoZ2_loc[2] ;

	_u_actual = _u_init;
}
*/
} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
