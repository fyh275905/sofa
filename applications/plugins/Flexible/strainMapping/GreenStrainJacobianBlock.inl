/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 MGH, INRIA, USTL, UJF, CNRS                    *
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
*                               SOFA :: Plugins                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef FLEXIBLE_GreenStrainJacobianBlock_INL
#define FLEXIBLE_GreenStrainJacobianBlock_INL

#include "GreenStrainJacobianBlock.h"
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include "../types/DeformationGradientTypes.h"
#include "../types/StrainTypes.h"

namespace sofa
{

namespace defaulttype
{

//////////////////////////////////////////////////////////////////////////////////
////  macros
//////////////////////////////////////////////////////////////////////////////////
#define F321(type)  DefGradientTypes<3,2,0,type>
#define F331(type)  DefGradientTypes<3,3,0,type>
#define F332(type)  DefGradientTypes<3,3,1,type>
#define E221(type)  StrainTypes<2,2,0,type>
#define E331(type)  StrainTypes<3,3,0,type>
#define E332(type)  StrainTypes<3,3,1,type>
#define E333(type)  StrainTypes<3,3,2,type>

//////////////////////////////////////////////////////////////////////////////////
////  helpers
//////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////
////  F331 -> E331
//////////////////////////////////////////////////////////////////////////////////

template<class InReal,class OutReal>
class GreenStrainJacobianBlock< F331(InReal) , E331(OutReal) > :
    public  BaseJacobianBlock< F331(InReal) , E331(OutReal) >
{
public:
    typedef F331(InReal) In;
    typedef E331(OutReal) Out;

    typedef BaseJacobianBlock<In,Out> Inherit;
    typedef typename Inherit::InCoord InCoord;
    typedef typename Inherit::InDeriv InDeriv;
    typedef typename Inherit::OutCoord OutCoord;
    typedef typename Inherit::OutDeriv OutDeriv;
    typedef typename Inherit::MatBlock MatBlock;
    typedef typename Inherit::KBlock KBlock;
    typedef typename Inherit::Real Real;

    typedef typename In::Frame Frame;  ///< Matrix representing a deformation gradient
    typedef typename Out::StrainMat StrainMat;  ///< Matrix representing a strain
    enum { material_dimensions = In::material_dimensions };
    enum { spatial_dimensions = In::spatial_dimensions };
    enum { strain_size = Out::strain_size };
    enum { frame_size = spatial_dimensions*material_dimensions };

    /**
    Mapping:   \f$ E = [F^T.F - I ]/2  \f$
    Jacobian:    \f$  dE = [ F^T.dF + dF^T.F ]/2 \f$
      */

    static const bool constantJ=false;

    InCoord F;   ///< =  store deformation gradient to compute J

    void addapply( OutCoord& result, const InCoord& data )
    {
        F=data;
        StrainMat strainmat=F.getF().multTranspose( F.getF() );
        for(unsigned int j=0; j<material_dimensions; j++) strainmat[j][j]-=1.;
        strainmat*=(Real)0.5;
        result.getStrain() += StrainMatToVoigt( strainmat );
    }

    void addmult( OutDeriv& result,const InDeriv& data )
    {
        StrainMat strainmat=F.getF().multTranspose( data.getF() );
        result.getStrain() += StrainMatToVoigt( strainmat );
    }

    void addMultTranspose( InDeriv& result, const OutDeriv& data )
    {
        result.getF() += F.getF()*StressVoigtToMat( data.getStrain() );
    }

    MatBlock getJ()
    {
        MatBlock B = MatBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,Out::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eB(&B[0][0]);
        eB = assembleJ(F.getF());
        return B;
    }

    KBlock getK(const OutDeriv& childForce)
    {
        KBlock K = KBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,In::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eK(&K[0][0]);
        // order 0
        StrainMat sigma=StressVoigtToMat( childForce.getStrain() );
        typedef Eigen::Map<Eigen::Matrix<Real,material_dimensions,material_dimensions,Eigen::RowMajor> > KBlock;
        KBlock s(&sigma[0][0]);
        for(unsigned int j=0; j<spatial_dimensions; j++)
            eK.template block(j*material_dimensions,j*material_dimensions,material_dimensions,material_dimensions) = s;
        return K;
    }
    void addDForce( InDeriv& df, const InDeriv& dx, const OutDeriv& childForce, const double& kfactor )
    {
        df.getF() += dx.getF()*StressVoigtToMat( childForce.getStrain() )*kfactor;
    }
};


//////////////////////////////////////////////////////////////////////////////////
////  F321 -> E221
//////////////////////////////////////////////////////////////////////////////////

template<class InReal,class OutReal>
class GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) > :
    public  BaseJacobianBlock< F321(InReal) , E221(OutReal) >
{
public:
    typedef F321(InReal) In;
    typedef E221(OutReal) Out;

    typedef BaseJacobianBlock<In,Out> Inherit;
    typedef typename Inherit::InCoord InCoord;
    typedef typename Inherit::InDeriv InDeriv;
    typedef typename Inherit::OutCoord OutCoord;
    typedef typename Inherit::OutDeriv OutDeriv;
    typedef typename Inherit::MatBlock MatBlock;
    typedef typename Inherit::KBlock KBlock;
    typedef typename Inherit::Real Real;

    typedef typename In::Frame Frame;  ///< Matrix representing a deformation gradient
    typedef typename Out::StrainMat StrainMat;  ///< Matrix representing a strain
    enum { material_dimensions = In::material_dimensions };
    enum { spatial_dimensions = In::spatial_dimensions };
    enum { strain_size = Out::strain_size };
    enum { frame_size = spatial_dimensions*material_dimensions };

    /**
    Mapping:   \f$ E = [F^T.F - I ]/2  \f$
    Jacobian:    \f$  dE = [ F^T.dF + dF^T.F ]/2 \f$
      */

    static const bool constantJ=false;

    InCoord F;   ///< =  store deformation gradient to compute J

    void addapply( OutCoord& result, const InCoord& data )
    {
        F=data;
        StrainMat strainmat=F.getF().multTranspose( F.getF() );
        for(unsigned int j=0; j<material_dimensions; j++) strainmat[j][j]-=1.;
        strainmat*=(Real)0.5;
        result.getStrain() += StrainMatToVoigt( strainmat );
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addapply, data = " << data << endl;
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addapply, result = " << result << endl;
    }

    void addmult( OutDeriv& result,const InDeriv& data )
    {
        StrainMat strainmat=F.getF().multTranspose( data.getF() );
        result.getStrain() += StrainMatToVoigt( strainmat );
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addmult, data = " << data << endl;
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addmult, result = " << result << endl;
    }

    void addMultTranspose( InDeriv& result, const OutDeriv& data )
    {
        result.getF() += F.getF()*StressVoigtToMat( data.getStrain() );
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addmultTranspose, data = " << data << endl;
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addmultTranspose, result = " << result << endl;
    }

    MatBlock getJ()
    {
        MatBlock B = MatBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,Out::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eB(&B[0][0]);
        eB = assembleJ(F.getF());
        return B;
    }

    KBlock getK(const OutDeriv& childForce)
    {
        KBlock K = KBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,In::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eK(&K[0][0]);
        // order 0
        StrainMat sigma=StressVoigtToMat( childForce.getStrain() );
        typedef Eigen::Map<Eigen::Matrix<Real,material_dimensions,material_dimensions,Eigen::RowMajor> > KBlock;
        KBlock s(&sigma[0][0]);
        for(unsigned int j=0; j<spatial_dimensions; j++)
            eK.template block(j*material_dimensions,j*material_dimensions,material_dimensions,material_dimensions) = s;
        return K;
    }
    void addDForce( InDeriv& df, const InDeriv& dx, const OutDeriv& childForce, const double& kfactor )
    {
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addDForce, kfactor = " << kfactor << endl;
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addDForce, dx = " << dx << endl;
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addDForce, childForce = " << childForce << endl;
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addDForce, parent df before = " << df << endl;
        df.getF() += dx.getF()*StressVoigtToMat( childForce.getStrain() )*kfactor;
//        cerr<<"GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addDForce, parent df after = " << df << endl;
//        cerr<<"WARNING GreenStrainJacobianBlock< F321(InReal) , E221(OutReal) >::addDForce TEMPORARILY DEACTIVATED FOR DEBUG PURPOSES "  << endl;
    }
};


//////////////////////////////////////////////////////////////////////////////////
////  F332 -> E333
//////////////////////////////////////////////////////////////////////////////////

template<class InReal,class OutReal>
class GreenStrainJacobianBlock< F332(InReal) , E333(OutReal) > :
    public  BaseJacobianBlock< F332(InReal) , E333(OutReal) >
{
public:
    typedef F332(InReal) In;
    typedef E333(OutReal) Out;

    typedef BaseJacobianBlock<In,Out> Inherit;
    typedef typename Inherit::InCoord InCoord;
    typedef typename Inherit::InDeriv InDeriv;
    typedef typename Inherit::OutCoord OutCoord;
    typedef typename Inherit::OutDeriv OutDeriv;
    typedef typename Inherit::MatBlock MatBlock;
    typedef typename Inherit::KBlock KBlock;
    typedef typename Inherit::Real Real;

    typedef typename In::Frame Frame;  ///< Matrix representing a deformation gradient
    typedef typename Out::StrainMat StrainMat;  ///< Matrix representing a strain
    enum { material_dimensions = In::material_dimensions };
    enum { spatial_dimensions = In::spatial_dimensions };
    enum { strain_size = Out::strain_size };
    enum { frame_size = spatial_dimensions*material_dimensions };

    /**
    Mapping:
        - \f$ E = [F^T.F - I ]/2  \f$
        - \f$ E_k = [(F_k^T.F + F^T.F_k ]/2  \f$
        - \f$ E_jk = E_kj = [(F_k^T.F_j + F_j^T.F_k ]/2  \f$
        - \f$ E_kk = [(F_k^T.F_k ]/2  \f$
    where:
        - _k denotes derivative with respect to spatial dimension k
    Jacobian:
        - \f$  dE = [ F^T.dF + dF^T.F ]/2 \f$
        - \f$  dE_k = [ F_k^T.dF + dF^T.F_k + dF_k^T.F + F^T.dF_k]/2 \f$
        - \f$  dE_jk = [ F_k^T.dF_j + dF_j^T.F_k + dF_k^T.F_j + F_j^T.dF_k]/2 \f$
        - \f$  dE_kk = [ F_k^T.dF_k + dF_k^T.F_k ]/2 \f$
      */

    static const bool constantJ=false;

    InCoord F;   ///< =  store deformation gradient to compute J

    void addapply( OutCoord& result, const InCoord& data )
    {
        F=data;
        // order 0
        StrainMat strainmat=F.getF().multTranspose( F.getF() );
        for(unsigned int j=0; j<material_dimensions; j++) strainmat[j][j]-=1.;
        strainmat*=(Real)0.5;
        result.getStrain() += StrainMatToVoigt( strainmat );
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            strainmat = F.getF().multTranspose( F.getGradientF(k) );
            result.getStrainGradient(k) += StrainMatToVoigt( strainmat );
        }
        // order 2
        for(unsigned int k=0; k<spatial_dimensions; k++)
            for(unsigned int j=k+1; j<spatial_dimensions; j++)
            {
                strainmat =F.getGradientF(j).multTranspose( F.getGradientF(k) );
                result.getStrainHessian(j,k) += StrainMatToVoigt( strainmat );
            }
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            strainmat =F.getGradientF(k).multTranspose( F.getGradientF(k) );
            strainmat*=(Real)0.5;
            result.getStrainHessian(k,k) += StrainMatToVoigt( strainmat );
        }
    }

    void addmult( OutDeriv& result,const InDeriv& data )
    {
        // order 0
        result.getStrain() += StrainMatToVoigt( F.getF().multTranspose( data.getF() ) );
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            StrainMat strainmat = F.getF().multTranspose( data.getGradientF(k) ) + F.getGradientF(k).multTranspose( data.getF() );
            result.getStrainGradient(k) += StrainMatToVoigt( strainmat );
        }
        // order 2
        for(unsigned int k=0; k<spatial_dimensions; k++)
            for(unsigned int j=0; j<spatial_dimensions; j++)
            {
                StrainMat strainmat = F.getGradientF(k).multTranspose( data.getGradientF(j) );
                result.getStrainHessian(j,k) += StrainMatToVoigt( strainmat );
            }
    }

    void addMultTranspose( InDeriv& result, const OutDeriv& data )
    {
        // order 0
        result.getF() += F.getF()*StressVoigtToMat( data.getStrain() );
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            StrainMat strainmat=StressVoigtToMat( data.getStrainGradient(k) );
            result.getF() += F.getGradientF(k)*strainmat;
            result.getGradientF(k) += F.getF()*strainmat;
        }
        // order 2
        for(unsigned int k=0; k<spatial_dimensions; k++)
            for(unsigned int j=k; j<spatial_dimensions; j++)
            {
                StrainMat strainmat=StressVoigtToMat( data.getStrainHessian(k,j) );
                result.getGradientF(k) += F.getGradientF(j)*strainmat;
                if(j!=k) result.getGradientF(j) += F.getGradientF(k)*strainmat;
            }
    }

    MatBlock getJ()
    {
        MatBlock B = MatBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,Out::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eB(&B[0][0]);
        // order 0
        typedef Eigen::Matrix<Real,strain_size,frame_size,Eigen::RowMajor> JBlock;
        JBlock J = assembleJ(F.getF());
        eB.template block(0,0,strain_size,frame_size) = J;
        // order 1
        Vec<spatial_dimensions,JBlock> Jgrad;
        for(unsigned int k=0; k<spatial_dimensions; k++) Jgrad[k]= assembleJ(F.getGradientF(k));
        unsigned int offsetE=strain_size;
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            eB.template block(offsetE,0,strain_size,frame_size) = Jgrad[k];
            eB.template block(offsetE,(k+1)*frame_size,strain_size,frame_size) = J;
            offsetE+=strain_size;
        }
        // order 2
        for(unsigned int k=0; k<spatial_dimensions; k++)
            for(unsigned int j=k; j<spatial_dimensions; j++)
            {
                eB.template block(offsetE,(j+1)*frame_size,strain_size,frame_size) = Jgrad[k];
                if(j!=k) eB.template block(offsetE,(k+1)*frame_size,strain_size,frame_size) = Jgrad[j];
                offsetE+=strain_size;
            }
        return B;
    }

    KBlock getK(const OutDeriv& childForce)
    {
        KBlock K = KBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,In::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eK(&K[0][0]);
        // order 0
        StrainMat sigma=StressVoigtToMat( childForce.getStrain() );
        typedef Eigen::Map<Eigen::Matrix<Real,material_dimensions,material_dimensions,Eigen::RowMajor> > KBlock;
        KBlock s(&sigma[0][0]);
        for(unsigned int j=0; j<spatial_dimensions*spatial_dimensions; j++)
            eK.template block(j*material_dimensions,j*material_dimensions,material_dimensions,material_dimensions) += s;
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            sigma=StressVoigtToMat( childForce.getStrainGradient(k) );
            for(unsigned int i=0; i<spatial_dimensions; i++)
                for(unsigned int j=0; j<spatial_dimensions; j++)
                {
                    eK.template block(j*material_dimensions+i*material_dimensions*spatial_dimensions,j*material_dimensions,material_dimensions,material_dimensions) += s;
                    eK.template block(j*material_dimensions,j*material_dimensions+i*material_dimensions*spatial_dimensions,material_dimensions,material_dimensions) += s;
                }
        }
        // order 2
        for(unsigned int k=0; k<spatial_dimensions; k++)
            for(unsigned int l=k; l<spatial_dimensions; l++)
            {
                sigma=StressVoigtToMat( childForce.getStrainHessian(k,l) );
                for(unsigned int j=0; j<spatial_dimensions; j++)
                {
                    eK.template block(j*material_dimensions+k*material_dimensions*spatial_dimensions,j*material_dimensions+l*material_dimensions*spatial_dimensions,material_dimensions,material_dimensions) += s;
                    if(k!=l) eK.template block(j*material_dimensions+l*material_dimensions*spatial_dimensions,j*material_dimensions+k*material_dimensions*spatial_dimensions,material_dimensions,material_dimensions) += s;
                }
            }
        return K;
    }
    void addDForce( InDeriv& df, const InDeriv& dx, const OutDeriv& childForce, const double& kfactor )
    {
        // order 0
        df.getF() += dx.getF()*StressVoigtToMat( childForce.getStrain() )*kfactor;
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            StrainMat strainmat=StressVoigtToMat( childForce.getStrainGradient(k) )*kfactor;
            df.getF() += dx.getGradientF(k)*strainmat;
            df.getGradientF(k) += dx.getF()*strainmat;
        }
        // order 2
        for(unsigned int k=0; k<spatial_dimensions; k++)
            for(unsigned int j=k; j<spatial_dimensions; j++)
            {
                StrainMat strainmat=StressVoigtToMat( childForce.getStrainHessian(k,j) )*kfactor;
                df.getGradientF(k) += dx.getGradientF(j)*strainmat;
                if(j!=k) df.getGradientF(j) += dx.getGradientF(k)*strainmat;
            }
    }

};


//////////////////////////////////////////////////////////////////////////////////
////  F332 -> E332     =    clamped version of F332 -> E333
//////////////////////////////////////////////////////////////////////////////////

template<class InReal,class OutReal>
class GreenStrainJacobianBlock< F332(InReal) , E332(OutReal) > :
    public  BaseJacobianBlock< F332(InReal) , E332(OutReal) >
{
public:
    typedef F332(InReal) In;
    typedef E332(OutReal) Out;

    typedef BaseJacobianBlock<In,Out> Inherit;
    typedef typename Inherit::InCoord InCoord;
    typedef typename Inherit::InDeriv InDeriv;
    typedef typename Inherit::OutCoord OutCoord;
    typedef typename Inherit::OutDeriv OutDeriv;
    typedef typename Inherit::MatBlock MatBlock;
    typedef typename Inherit::KBlock KBlock;
    typedef typename Inherit::Real Real;

    typedef typename In::Frame Frame;  ///< Matrix representing a deformation gradient
    typedef typename Out::StrainMat StrainMat;  ///< Matrix representing a strain
    enum { material_dimensions = In::material_dimensions };
    enum { spatial_dimensions = In::spatial_dimensions };
    enum { strain_size = Out::strain_size };
    enum { frame_size = spatial_dimensions*material_dimensions };

    /**
    Mapping:
        - \f$ E = [F^T.F - I ]/2  \f$
        - \f$ E_k = [(F_k^T.F + F^T.F_k ]/2  \f$
    where:
        - _k denotes derivative with respect to spatial dimension k
    Jacobian:
        - \f$  dE = [ F^T.dF + dF^T.F ]/2 \f$
        - \f$  dE_k = [ F_k^T.dF + dF^T.F_k + dF_k^T.F + F^T.dF_k]/2 \f$
      */

    static const bool constantJ=false;

    InCoord F;   ///< =  store deformation gradient to compute J

    void addapply( OutCoord& result, const InCoord& data )
    {
        F=data;
        // order 0
        StrainMat strainmat=F.getF().multTranspose( F.getF() );
        for(unsigned int j=0; j<material_dimensions; j++) strainmat[j][j]-=1.;
        strainmat*=(Real)0.5;
        result.getStrain() += StrainMatToVoigt( strainmat );
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            strainmat = F.getF().multTranspose( F.getGradientF(k) );
            result.getStrainGradient(k) += StrainMatToVoigt( strainmat );
        }
    }

    void addmult( OutDeriv& result,const InDeriv& data )
    {
        // order 0
        result.getStrain() += StrainMatToVoigt( F.getF().multTranspose( data.getF() ) );
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            StrainMat strainmat = F.getF().multTranspose( data.getGradientF(k) ) + F.getGradientF(k).multTranspose( data.getF() );
            result.getStrainGradient(k) += StrainMatToVoigt( strainmat );
        }
    }

    void addMultTranspose( InDeriv& result, const OutDeriv& data )
    {
        // order 0
        result.getF() += F.getF()*StressVoigtToMat( data.getStrain() );
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            StrainMat strainmat=StressVoigtToMat( data.getStrainGradient(k) );
            result.getF() += F.getGradientF(k)*strainmat;
            result.getGradientF(k) += F.getF()*strainmat;
        }
    }

    MatBlock getJ()
    {
        MatBlock B = MatBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,Out::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eB(&B[0][0]);
        // order 0
        typedef Eigen::Matrix<Real,strain_size,frame_size,Eigen::RowMajor> JBlock;
        JBlock J = assembleJ(F.getF());
        eB.template block(0,0,strain_size,frame_size) = J;
        // order 1
        Vec<3,JBlock> Jgrad;
        for(unsigned int k=0; k<spatial_dimensions; k++) Jgrad[k]= assembleJ(F.getGradientF(k));
        unsigned int offsetE=strain_size;
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            eB.template block(offsetE,0,strain_size,frame_size) = Jgrad[k];
            eB.template block(offsetE,(k+1)*frame_size,strain_size,frame_size) = J;
            offsetE+=strain_size;
        }
        return B;
    }

    KBlock getK(const OutDeriv& childForce)
    {
        KBlock K = KBlock();
        typedef Eigen::Map<Eigen::Matrix<Real,In::deriv_total_size,In::deriv_total_size,Eigen::RowMajor> > EigenMap;
        EigenMap eK(&K[0][0]);
        // order 0
        StrainMat sigma=StressVoigtToMat( childForce.getStrain() );
        typedef Eigen::Map<Eigen::Matrix<Real,material_dimensions,material_dimensions,Eigen::RowMajor> > KBlock;
        KBlock s(&sigma[0][0]);
        for(unsigned int j=0; j<spatial_dimensions*spatial_dimensions; j++)
            eK.template block(j*material_dimensions,j*material_dimensions,material_dimensions,material_dimensions) += s;
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            sigma=StressVoigtToMat( childForce.getStrainGradient(k) );
            for(unsigned int i=0; i<spatial_dimensions; i++)
                for(unsigned int j=0; j<spatial_dimensions; j++)
                {
                    eK.template block(j*material_dimensions+i*material_dimensions*spatial_dimensions,j*material_dimensions,material_dimensions,material_dimensions) += s;
                    eK.template block(j*material_dimensions,j*material_dimensions+i*material_dimensions*spatial_dimensions,material_dimensions,material_dimensions) += s;
                }
        }
        return K;
    }
    void addDForce( InDeriv& df, const InDeriv& dx, const OutDeriv& childForce, const double& kfactor )
    {
        // order 0
        df.getF() += dx.getF()*StressVoigtToMat( childForce.getStrain() )*kfactor;
        // order 1
        for(unsigned int k=0; k<spatial_dimensions; k++)
        {
            StrainMat strainmat=StressVoigtToMat( childForce.getStrainGradient(k) )*kfactor;
            df.getF() += dx.getGradientF(k)*strainmat;
            df.getGradientF(k) += dx.getF()*strainmat;
        }
    }
};


} // namespace defaulttype
} // namespace sofa



#endif
