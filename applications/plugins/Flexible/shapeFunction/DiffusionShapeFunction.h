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
#ifndef FLEXIBLE_DiffusionShapeFunction_H
#define FLEXIBLE_DiffusionShapeFunction_H

#include "../initFlexible.h"
#include "../shapeFunction/BaseShapeFunction.h"
#include "../shapeFunction/BaseImageShapeFunction.h"
#include "../types/PolynomialBasis.h"

#include <image/ImageTypes.h>
#include <image/ImageAlgorithms.h>

#include <sofa/helper/OptionsGroup.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include <Eigen/SparseCore>
//#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>

#define HARMONIC 0
#define BIHARMONIC 1
#define ANISOTROPIC 2

#define GAUSS_SEIDEL 0
#define LDLT 1
#define CG 2
#define biCG 3


namespace sofa
{
namespace component
{
namespace shapefunction
{

using core::behavior::BaseShapeFunction;
using defaulttype::Mat;
using defaulttype::Vec;

/**
Shape functions computed using heat diffusion in images
  */


/// Default implementation does not compile
template <int imageTypeLabel>
struct DiffusionShapeFunctionSpecialization
{
};

/// Specialization for regular Image
template <>
struct DiffusionShapeFunctionSpecialization<defaulttype::IMAGELABEL_IMAGE>
{
    template<class DiffusionShapeFunction>
    static void init(DiffusionShapeFunction* This)
    {
        typedef typename DiffusionShapeFunction::ImageTypes ImageTypes;
        typedef typename DiffusionShapeFunction::raImage raImage;
        typedef typename DiffusionShapeFunction::DistTypes DistTypes;
        typedef typename DiffusionShapeFunction::waDist waDist;
        typedef typename DiffusionShapeFunction::DistT DistT;
        typedef typename DiffusionShapeFunction::IndTypes IndTypes;
        typedef typename DiffusionShapeFunction::waInd waInd;

        // retrieve data
        raImage in(This->image);
        if(in->isEmpty())  { This->serr<<"Image not found"<<This->sendl; return; }
//        const typename ImageTypes::CImgT& inimg = in->getCImg(0);  // suppose time=0

        // init dist
        typename DiffusionShapeFunction::imCoord dim = in->getDimensions(); dim[ImageTypes::DIMENSION_S]=dim[ImageTypes::DIMENSION_T]=1;

        waDist distData(This->f_distances);         distData->setDimensions(dim);
//        typename DistTypes::CImgT& dist = distData->getCImg();

        // init indices and weights images
        unsigned int nbref=This->f_nbRef.getValue();        dim[ImageTypes::DIMENSION_S]=nbref;

        waInd indData(This->f_index); indData->setDimensions(dim);
        typename IndTypes::CImgT& indices = indData->getCImg(); indices.fill(0);

        waDist weightData(This->f_w);         weightData->setDimensions(dim);
        typename DistTypes::CImgT& weights = weightData->getCImg(); weights.fill(0);
    }
    /*
    template<class DiffusionShapeFunction>
    static Eigen::SparseMatrix<double> fillH(DiffusionShapeFunction* This, const CImg<double>& stencil, const CImg<unsigned long> ind, const unsigned long N)
    {
        Eigen::SparseMatrix<double> mat(N,N);         // default is column major

        // count non null entries
        unsigned int sparsity=0;    cimg_forXYZ(stencil,dx,dy,dz) if(stencil(dx,dy,dz)) sparsity++;
        unsigned int ox=0.5*(stencil.width()-1),oy=0.5*(stencil.height()-1),oz=0.5*(stencil.depth()-1);
        unsigned int border=max(ox,max(oy,oz));

        mat.reserve(Eigen::VectorXi::Constant(N,sparsity));

        cimg_for_insideXYZ(label,x,y,z,border)
                if(label(x,y,z)==1)
        {
            cimg_forXYZ(stencil,dx,dy,dz)
                    if(stencil(dx,dy,dz))
            {
                if(label(x+dx-ox,y+dy-oy,z+dz-oz)==1) mat.coeffRef(ind(x,y,z),ind(x+dx-ox,y+dy-oy,z+dz-oz))+=stencil(dx,dy,dz);
                else if(label(x+dx-ox,y+dy-oy,z+dz-oz)==0) mat.coeffRef(ind(x,y,z),ind(x,y,z))+=stencil(dx,dy,dz); // neumann bc
            }
        }
        mat.makeCompressed();
        return mat;
    }

    /// Apply Dirichlet conditions based on chosen parent
    template<class DiffusionShapeFunction>
    static Eigen::VectorXd fillb(DiffusionShapeFunction* This, const unsigned index)
                                 //const CImg<double>& stencil,const CImg<unsigned long> ind, const unsigned long N, const unsigned int index,const double temp)
    {
        Eigen::VectorXd b = Eigen::VectorXd::Zero(N);

        unsigned int ox=0.5*(stencil.width()-1),oy=0.5*(stencil.height()-1),oz=0.5*(stencil.depth()-1);
        unsigned int border=max(ox,max(oy,oz));

        cimg_for_insideXYZ(label,x,y,z,border)
                if(label(x,y,z)==1)
        {
            cimg_forXYZ(stencil,dx,dy,dz)
                    if(stencil(dx,dy,dz))
                    if(label(x+dx-ox,y+dy-oy,z+dz-oz)==index) b(ind(x,y,z))-=temp*stencil(dx,dy,dz);

        }

        return b;
    }
*/

    /// update weights and indices images based on computed diffusion image and current node index
    template<class DiffusionShapeFunction>
    static void updateWeights(DiffusionShapeFunction* This, const unsigned index)
    {
        // retrieve data
        typename DiffusionShapeFunction::raDist distData(This->f_distances);           const typename DiffusionShapeFunction::DistTypes::CImgT& dist = distData->getCImg();
        typename DiffusionShapeFunction::waInd indData(This->f_index);                 typename DiffusionShapeFunction::IndTypes::CImgT& indices = indData->getCImg();
        typename DiffusionShapeFunction::waDist weightData(This->f_w);                 typename DiffusionShapeFunction::DistTypes::CImgT& weights = weightData->getCImg();

        // copy from dist
        unsigned int nbref=This->f_nbRef.getValue();
        cimg_forXYZ(dist,x,y,z) if(dist(x,y,z)>0)
        {
            unsigned int j=0;
            while(j!=nbref && weights(x,y,z,j)>=dist(x,y,z)) j++;
            if(j!=nbref)
            {
                if(j!=nbref-1) for(unsigned int k=nbref-1; k>j; k--) { weights(x,y,z,k)=weights(x,y,z,k-1); indices(x,y,z,k)=indices(x,y,z,k-1); }
                weights(x,y,z,j)=dist(x,y,z);
                indices(x,y,z,j)=index+1;
            }
        }
    }




    template<class DiffusionShapeFunction>
    static void initTemp(DiffusionShapeFunction* This, const unsigned index)
    {
        typedef typename DiffusionShapeFunction::DistT DistT;

        // retrieve data
        typename DiffusionShapeFunction::raImage in(This->image);
        typename DiffusionShapeFunction::raTransform inT(This->transform);
        if(in->isEmpty())  { This->serr<<"Image not found"<<This->sendl; return; }
        const typename DiffusionShapeFunction::ImageTypes::CImgT& inimg = in->getCImg(0);  // suppose time=0

        typename DiffusionShapeFunction::waDist distData(This->f_distances);     typename DiffusionShapeFunction::DistTypes::CImgT& dist = distData->getCImg();

        typename DiffusionShapeFunction::raVecCoord parent(This->f_position);
        if(!parent.size()) { This->serr<<"Parent nodes not found"<<This->sendl; return; }

        // init temperatures
        dist.fill(-1);
        cimg_foroff(inimg,off) if(inimg[off]) dist[off]=DiffusionShapeFunction::MAXTEMP*0.5;

        for(unsigned int i=0; i<parent.size(); i++)
        {
            typename DiffusionShapeFunction::Coord p = inT->toImageInt(parent[i]);
            if(in->isInside(p[0],p[1],p[2])) dist(p[0],p[1],p[2])=(i==index)?DiffusionShapeFunction::MAXTEMP:0;
        }
    }


    template<class DiffusionShapeFunction>
    static double GaussSeidelStep(DiffusionShapeFunction* This) //, const CImg<typename DiffusionShapeFunction::DistT>& stencil
    {
        // laplacian stencil
        CImg<typename DiffusionShapeFunction::DistT> stencil(3,3,3);
        stencil.fill(0);
        stencil(1,1,1)=-6.0;
        stencil(0,1,1)=stencil(2,1,1)=stencil(1,0,1)=stencil(1,2,1)=stencil(1,1,0)=stencil(1,1,2)=1.0;

        typename DiffusionShapeFunction::waDist distData(This->f_distances);     typename DiffusionShapeFunction::DistTypes::CImgT& dist = distData->getCImg();

        unsigned int ox=0.5*(stencil.width()-1),oy=0.5*(stencil.height()-1),oz=0.5*(stencil.depth()-1);
        unsigned int border=std::max(ox,std::max(oy,oz));

        double res=0; // return maximum absolute change
        cimg_for_insideXYZ(dist,x,y,z,border)
                if(dist(x,y,z)>0 && dist(x,y,z)<1)
        {
            typename DiffusionShapeFunction::DistT val=0;
            cimg_forXYZ(stencil,dx,dy,dz)
                    if(stencil(dx,dy,dz))
                    if(dx!=(int)ox || dy!=(int)oy || dz!=(int)oz)
            {
                if(dist(x+dx-ox,y+dy-oy,z+dz-oz)!=-1.0) val+=dist(x+dx-ox,y+dy-oy,z+dz-oz)*stencil(dx,dy,dz);
                else val+=dist(x,y,z)*stencil(dx,dy,dz);  // neumann boundary conditions
            }
            val = -val/stencil(ox,oy,oz);
            if(res<fabs(val-dist(x,y,z))) res=fabs(val-dist(x,y,z));
            dist(x,y,z)=val;
        }
        return res;
    }


    /**
    * do one gauss seidel diffusion step : each pixel is replaced by the weighted average of its neighbors
    * weights are based on the (biased) distance : w= e(-d'²/sigma²) with d'=d/min(stiffness)
    * distances must be initialized to 1 or 0 at constrained pixels, to -1 outside, and between 0 and 1 elsewhere
    */
/*
    template<typename real,typename T>
    real GaussSeidelDiffusionStep (CImg<real>& distances, const sofa::defaulttype::Vec<3,real>& voxelsize, const CImg<T>* biasFactor=NULL)
    {
        typedef sofa::defaulttype::Vec<3,int> iCoord;
        const iCoord dim(distances.width(),distances.height(),distances.depth());

        // init
        sofa::defaulttype::Vec<6,  iCoord > offset; // image coord offsets related to neighbors
        sofa::defaulttype::Vec<6,  real > lD;      // precomputed local distances (supposing that the transformation is linear)
        offset[0]=iCoord(-1,0,0);           lD[0]=voxelsize[0];
        offset[1]=iCoord(+1,0,0);           lD[1]=voxelsize[0];
        offset[2]=iCoord(0,-1,0);           lD[2]=voxelsize[1];
        offset[3]=iCoord(0,+1,0);           lD[3]=voxelsize[1];
        offset[4]=iCoord(0,0,-1);           lD[4]=voxelsize[2];
        offset[5]=iCoord(0,0,+1);           lD[5]=voxelsize[2];
        unsigned int nbOffset=offset.size();

        const real inv_variance=5.0; // =1/sigma² arbitrary

        real res=0; // return maximum absolute change
        cimg_for_insideXYZ(distances,x,y,z,1)
                if(distances(x,y,z)>0 && distances(x,y,z)<1)
        {
            real b1; if(biasFactor) b1=(real)(*biasFactor)(x,y,z); else  b1=1.0;
            real val=0;
            real W=0;
            for(unsigned int i=0;i<nbOffset;i++)
            {
                real b2; if(biasFactor) b2=(real)(*biasFactor)(x+offset[0],y+offset[1],z+offset[2]); else  b2=1.0;
                real d = lD[i]*1.0/sofa::helper::rmin(b1,b2);
                real w=exp(-d*d*inv_variance);
                if(distances(x+offset[0],y+offset[1],z+offset[2])!=-1.0) val+= w*distances(x+offset[0],y+offset[1],z+offset[2]);
                else val+= w*distances(x,y,z);
                W+=w;
            }
            if(W!=0) val /= W;
            if(res<fabs(val-distances(x,y,z))) res=fabs(val-distances(x,y,z));
            distances(x,y,z)=val;
        }
        return res;
    }*/
};


template <class ShapeFunctionTypes_,class ImageTypes_>
class DiffusionShapeFunction : public BaseImageShapeFunction<ShapeFunctionTypes_,ImageTypes_>
{
    friend struct DiffusionShapeFunctionSpecialization<defaulttype::IMAGELABEL_IMAGE>;
    friend struct DiffusionShapeFunctionSpecialization<defaulttype::IMAGELABEL_BRANCHINGIMAGE>;

public:
    SOFA_CLASS(SOFA_TEMPLATE2(DiffusionShapeFunction, ShapeFunctionTypes_,ImageTypes_) , SOFA_TEMPLATE2(BaseImageShapeFunction, ShapeFunctionTypes_,ImageTypes_));
    typedef BaseImageShapeFunction<ShapeFunctionTypes_,ImageTypes_> Inherit;

    /** @name  Shape function types */
    //@{
    typedef typename Inherit::Real Real;
    typedef typename Inherit::Coord Coord;
    typedef helper::ReadAccessor<Data<vector<Coord> > > raVecCoord;
    //@}

    /** @name  Image data */
    //@{
    typedef ImageTypes_ ImageTypes;
    typedef typename Inherit::T T;
    typedef typename Inherit::imCoord imCoord;
    typedef typename Inherit::raImage raImage;

    typedef typename Inherit::raTransform raTransform;

    typedef typename Inherit::DistT DistT;
    typedef typename Inherit::DistTypes DistTypes;
    typedef typename Inherit::raDist raDist;
    typedef typename Inherit::waDist waDist;
    Data< DistTypes > f_distances;

    typedef typename Inherit::IndT IndT;
    typedef typename Inherit::IndTypes IndTypes;
    typedef typename Inherit::waInd waInd;
    //@}

    /** @name  Options */
    //@{
    Data<bool> f_clearData;
    Data<helper::OptionsGroup> method;
    Data<helper::OptionsGroup> solver;
    Data<unsigned int> iterations;
    Data<Real> tolerance;
    Data<bool> biasDistances;

    static const DistT MAXTEMP;
    //@}

    virtual std::string getTemplateName() const    { return templateName(this); }
    static std::string templateName(const DiffusionShapeFunction<ShapeFunctionTypes_,ImageTypes_>* = NULL) { return ShapeFunctionTypes_::Name()+std::string(",")+ImageTypes_::Name(); }


    virtual void init()
    {
        Inherit::init();

        // init weight and indice image
        DiffusionShapeFunctionSpecialization<ImageTypes::label>::init( this );

        if (this->method.getValue().getSelectedId() == HARMONIC)
        {
            if (this->solver.getValue().getSelectedId() == GAUSS_SEIDEL)
            {
                for(unsigned int i=0; i<this->f_position.getValue().size(); i++)
                {
                    DiffusionShapeFunctionSpecialization<ImageTypes::label>::initTemp(this,i);

                    double res=this->tolerance.getValue();
                    for(unsigned int it=0; it<this->iterations.getValue(); it++)
                        if(res>=this->tolerance.getValue())
                            res = DiffusionShapeFunctionSpecialization<ImageTypes::label>::GaussSeidelStep(this);

                    DiffusionShapeFunctionSpecialization<ImageTypes::label>::updateWeights(this,i);
                }
            }
            /* else
            {
                // assemble system Hx = b
                Eigen::SparseMatrix<double> H = fillH(label,stencil,ind,N);
                Eigen::VectorXd b = fillb(label,stencil,ind,N,index,temp);

                // solve using Eigen
                if (this->solver.getValue().getSelectedId() == LDLT)
                {
                    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double> > chol(H);
                    std::cout<<"done"<<std::endl;
                    std::cout<<"solve"<<std::endl;
                    Eigen::VectorXd X = chol.solve(b);
                    //    Eigen::VectorXd X = chol.solve(X1+b);
                    std::cout<<"done"<<std::endl;
                }
                else if (this->solver.getValue().getSelectedId() == CG)
                {
                        Eigen::ConjugateGradient<Eigen::SparseMatrix<double>,  Eigen::IncompleteLUT<double> > cg;
                        cg.setMaxIterations(500);
                        cg.setTolerance(1e-8);
                        cg.compute(H);
                        std::cout<<"done"<<std::endl;
                        std::cout<<"solve"<<std::endl;
                        Eigen::VectorXd X = cg.solve(b);
                        std::cout<<"done"<<std::endl;
                        std::cout << "#iterations:     " << cg.iterations() << std::endl;
                        std::cout << "estimated error: " << cg.error()      << std::endl;
                }
                else if (this->solver.getValue().getSelectedId() == biCG)
                {
                            Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IncompleteLUT<double> > cg(H);
                                cg.setTolerance(1e-8);
                            std::cout<<"done"<<std::endl;
                                std::cout<<"solve"<<std::endl;
                                Eigen::VectorXd X = cg.solve(b);
                                std::cout<<"done"<<std::endl;
                                std::cout << "#iterations:     " << cg.iterations() << std::endl;
                                std::cout << "estimated error: " << cg.error()      << std::endl;
                }
            }*/


        }

        if(this->f_clearData.getValue())
        {
            waDist dist(this->f_distances); dist->clear();
        }
    }

protected:
    DiffusionShapeFunction()
        :Inherit()
        , f_distances(initData(&f_distances,DistTypes(),"distances",""))
        , f_clearData(initData(&f_clearData,true,"clearData","clear diffusion image after computation?"))
        , method ( initData ( &method,"method","method" ) )
        , solver ( initData ( &solver,"solver","solver (param)" ) )
        , iterations(initData(&iterations,(unsigned int)100,"iterations","Max number of iterations for iterative solvers"))
        , tolerance(initData(&tolerance,(Real)1e-8,"tolerance","Error tolerance for iterative solvers"))
        , biasDistances(initData(&biasDistances,false,"bias","Bias distances using inverse pixel values"))
    {
        helper::OptionsGroup methodOptions(3,"0 - Harmonic"
                                           ,"1 - bi-Harmonic"
                                           ,"2 - Anisotropic"
                                           );
        methodOptions.setSelectedItem(HARMONIC);
        method.setValue(methodOptions);
        method.setGroup("parameters");

        helper::OptionsGroup solverOptions(4,"0 - Gauss-Seidel"
                                           ,"1 - LDLT"
                                           ,"2 - CG"
                                           ,"3 - biCG"
                                           );
        solverOptions.setSelectedItem(GAUSS_SEIDEL);
        solver.setValue(solverOptions);
        solver.setGroup("parameters");

        biasDistances.setGroup("parameters");

    }

    virtual ~DiffusionShapeFunction()
    {

    }
};


}
}
}


#endif

