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
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_COMPONENT_MAPPING_DistanceMapping_INL
#define SOFA_COMPONENT_MAPPING_DistanceMapping_INL

#include "DistanceMapping.h"
#include <sofa/core/visual/VisualParams.h>
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace component
{

namespace mapping
{

using namespace sofa::defaulttype;


template <class TIn, class TOut>
DistanceMapping<TIn, TOut>::DistanceMapping()
    : Inherit()
    , f_computeDistance(initData(&f_computeDistance, false, "computeDistance", "if 'computeDistance = true', then rest length of each element equal 0, otherwise rest length is the initial lenght of each of them"))
    , f_restLengths(initData(&f_restLengths, "restLengths", "Rest lengths of the connections"))
    , d_showObjectScale(initData(&d_showObjectScale, Real(0), "showObjectScale", "Scale for object display"))
    , d_color(initData(&d_color, defaulttype::Vec4f(1,1,0,1), "showColor", "Color for object display"))
{
}

template <class TIn, class TOut>
DistanceMapping<TIn, TOut>::~DistanceMapping()
{
}


template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::init()
{
    edgeContainer = dynamic_cast<topology::EdgeSetTopologyContainer*>( this->getContext()->getMeshTopology() );
    if( !edgeContainer ) serr<<"No EdgeSetTopologyContainer found ! "<<sendl;

    SeqEdges links = edgeContainer->getEdges();

    this->getToModel()->resize( links.size() );

    // compute the rest lengths if they are not known
    if( f_restLengths.getValue().size() != links.size() )
    {
        helper::WriteAccessor< Data<vector<Real> > > restLengths(f_restLengths);
        typename core::behavior::MechanicalState<In>::ReadVecCoord pos = this->getFromModel()->readPositions();
        restLengths.resize( links.size() );
        if(!(f_computeDistance.getValue()))
            for(unsigned i=0; i<links.size(); i++ )
                restLengths[i] = (pos[links[i][0]] - pos[links[i][1]]).norm();
        else
            for(unsigned i=0; i<links.size(); i++ )
                restLengths[i] = (Real)0.;
    }

    baseMatrices.resize( 1 );
    baseMatrices[0] = &jacobian;

    this->Inherit::init();  // applies the mapping, so after the Data init
}

template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::computeCoordPositionDifference( InDeriv& r, const InCoord& a, const InCoord& b )
{
    // default implementation
    TIn::setDPos(r, TIn::getDPos(TIn::coordDifference(b,a))); //Generic code working also for type!=particles but not optimize for particles
}

template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::apply(const core::MechanicalParams * /*mparams*/ , Data<OutVecCoord>& dOut, const Data<InVecCoord>& dIn)
{
    helper::WriteAccessor< Data<OutVecCoord> >  out = dOut;
    helper::ReadAccessor< Data<InVecCoord> >  in = dIn;
    helper::ReadAccessor<Data<vector<Real> > > restLengths(f_restLengths);
    SeqEdges links = edgeContainer->getEdges();

    //    jacobian.clear();
    jacobian.resizeBlocks(out.size(),in.size());
    directions.resize(out.size());
    invlengths.resize(out.size());

    for(unsigned i=0; i<links.size(); i++ )
    {
        InDeriv& gap = directions[i];

        // gap = in[links[i][1]] - in[links[i][0]] (only for position)
        computeCoordPositionDifference( gap, in[links[i][0]], in[links[i][1]] );

        Real gapNorm = gap.norm();
        out[i] = gapNorm - restLengths[i];  // output

        // normalize
        if( gapNorm>1.e-10 )
        {
            invlengths[i] = 1/gapNorm;
            gap *= invlengths[i];
        }
        else
        {
            invlengths[i] = 0;
            gap = InDeriv();
            gap[0]=1.0;  // arbitrary unit vector
        }

        // insert in increasing row and column order
//        jacobian.beginRow(i);
        if( links[i][1]<links[i][0])
        {
            for(unsigned j=0; j<Nout; j++)
            {
                for(unsigned k=0; k<Nin; k++ )
                {
                    jacobian.insertBack( i*Nout+j, links[i][1]*Nin+k, gap[k] );
//                    jacobian.add( i*Nout+j, links[i][1]*Nin+k, gap[k] );
                }
                for(unsigned k=0; k<Nin; k++ )
                {
//                    jacobian.add( i*Nout+j, links[i][0]*Nin+k, -gap[k] );
                    jacobian.insertBack( i*Nout+j, links[i][0]*Nin+k, -gap[k] );
                }
            }
        }
        else
        {
            for(unsigned j=0; j<Nout; j++)
            {
                for(unsigned k=0; k<Nin; k++ )
                {
//                    jacobian.add( i*Nout+j, links[i][0]*Nin+k, -gap[k] );
                    jacobian.insertBack( i*Nout+j, links[i][0]*Nin+k, -gap[k] );
                }
                for(unsigned k=0; k<Nin; k++ )
                {
//                    jacobian.add( i*Nout+j, links[i][1]*Nin+k, gap[k] );
                    jacobian.insertBack( i*Nout+j, links[i][1]*Nin+k, gap[k] );
                }
            }
        }
    }

    jacobian.compress();
    //      cerr<<"DistanceMapping<TIn, TOut>::apply, jacobian: "<<endl<< jacobian << endl;

}


template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::applyJ(const core::MechanicalParams * /*mparams*/ , Data<OutVecDeriv>& dOut, const Data<InVecDeriv>& dIn)
{
    if( jacobian.rowSize() > 0 )
        jacobian.mult(dOut,dIn);
}

template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::applyJT(const core::MechanicalParams * /*mparams*/ , Data<InVecDeriv>& dIn, const Data<OutVecDeriv>& dOut)
{
    if( jacobian.rowSize() > 0 )
        jacobian.addMultTranspose(dIn,dOut);
}

template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::applyDJT(const core::MechanicalParams* mparams, core::MultiVecDerivId parentDfId, core::ConstMultiVecDerivId )
{
    std::cerr<<SOFA_CLASS_METHOD<<mparams->kFactor()<<std::endl;
    helper::WriteAccessor<Data<InVecDeriv> > parentForce (*parentDfId[this->fromModel.get(mparams)].write());
    helper::ReadAccessor<Data<InVecDeriv> > parentDisplacement (*mparams->readDx(this->fromModel));  // parent displacement
    Real kfactor = mparams->kFactor();
    helper::ReadAccessor<Data<OutVecDeriv> > childForce (*mparams->readF(this->toModel));
    SeqEdges links = edgeContainer->getEdges();

    for(unsigned i=0; i<links.size(); i++ )
    {
        Mat<Nin,Nin,Real> b;  // = (I - uu^T)
        for(unsigned j=0; j<Nin; j++)
        {
            for(unsigned k=0; k<Nin; k++)
            {
                if( j==k )
                    b[j][k] = 1. - directions[i][j]*directions[i][k];
                else
                    b[j][k] =    - directions[i][j]*directions[i][k];
            }
        }
        b *= childForce[i][0] * invlengths[i] * kfactor;  // (I - uu^T)*f/l*kfactor     do not forget kfactor !
        // note that computing a block is not efficient here, but it would make sense for storing a stiffness matrix

        InDeriv dx = parentDisplacement[links[i][1]] - parentDisplacement[links[i][0]];
        InDeriv df;
        for(unsigned j=0; j<Nin; j++)
        {
            for(unsigned k=0; k<Nin; k++)
            {
                df[j]+=b[j][k]*dx[k];
            }
        }
        parentForce[links[i][0]] -= df;
        parentForce[links[i][1]] += df;
 //       cerr<<"DistanceMapping<TIn, TOut>::applyDJT, df = " << df << endl;
    }
}

template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::applyJT(const core::ConstraintParams*, Data<InMatrixDeriv>& , const Data<OutMatrixDeriv>& )
{
    //    cerr<<"DistanceMapping<TIn, TOut>::applyJT(const core::ConstraintParams*, Data<InMatrixDeriv>& , const Data<OutMatrixDeriv>& ) does nothing " << endl;
}


template <class TIn, class TOut>
const sofa::defaulttype::BaseMatrix* DistanceMapping<TIn, TOut>::getJ()
{
    return &jacobian;
}

template <class TIn, class TOut>
const vector<sofa::defaulttype::BaseMatrix*>* DistanceMapping<TIn, TOut>::getJs()
{
    return &baseMatrices;
}

template <class TIn, class TOut>
const defaulttype::BaseMatrix* DistanceMapping<TIn, TOut>::getK()
{
//    helper::ReadAccessor<Data<OutVecDeriv> > childForce (*this->toModel->read(core::ConstVecDerivId::force()));
    const OutVecDeriv& childForce = this->toModel->readForces().ref();
    SeqEdges links = edgeContainer->getEdges();

    unsigned int size = this->fromModel->getSize();
    K.resizeBlocks(size,size);
    for(size_t i=0; i<links.size(); i++)
    {

        Mat<Nin,Nin,Real> b;  // = (I - uu^T)
        for(unsigned j=0; j<Nin; j++)
        {
            for(unsigned k=0; k<Nin; k++)
            {
                if( j==k )
                    b[j][k] = 1. - directions[i][j]*directions[i][k];
                else
                    b[j][k] =    - directions[i][j]*directions[i][k];
            }
        }
        b *= childForce[i][0] * invlengths[i];  // (I - uu^T)*f/l

        K.beginBlockRow(links[i][0]);
        K.createBlock(links[i][0],b);
        K.createBlock(links[i][1],-b);
        K.endBlockRow();
        K.beginBlockRow(links[i][1]);
        K.createBlock(links[i][0],-b);
        K.createBlock(links[i][1],b);
        K.endBlockRow();
    }
    K.compress();

    return &K;
}

template <class TIn, class TOut>
void DistanceMapping<TIn, TOut>::draw(const core::visual::VisualParams* vparams)
{
    if( !vparams->displayFlags().getShowMechanicalMappings() ) return;

    typename core::behavior::MechanicalState<In>::ReadVecCoord pos = this->getFromModel()->readPositions();
    SeqEdges links = edgeContainer->getEdges();



    if( d_showObjectScale.getValue() == 0 )
    {
        vector< Vector3 > points;
        for(unsigned i=0; i<links.size(); i++ )
        {
            points.push_back( Vector3( TIn::getCPos(pos[links[i][0]]) ) );
            points.push_back( Vector3( TIn::getCPos(pos[links[i][1]]) ));
        }
        vparams->drawTool()->drawLines ( points, 1, d_color.getValue() );
    }
    else
    {
        for(unsigned i=0; i<links.size(); i++ )
        {
            Vector3 p0 = TIn::getCPos(pos[links[i][0]]);
            Vector3 p1 = TIn::getCPos(pos[links[i][1]]);
            vparams->drawTool()->drawCylinder( p0, p1, d_showObjectScale.getValue(), d_color.getValue() );
        }
    }
}






///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////




template <class TIn, class TOut>
DistanceMultiMapping<TIn, TOut>::DistanceMultiMapping()
    : Inherit()
    , f_computeDistance(initData(&f_computeDistance, false, "computeDistance", "if 'computeDistance = true', then rest length of each element equal 0, otherwise rest length is the initial lenght of each of them"))
    , f_restLengths(initData(&f_restLengths, "restLengths", "Rest lengths of the connections"))
    , d_showObjectScale(initData(&d_showObjectScale, Real(0), "showObjectScale", "Scale for object display"))
    , d_color(initData(&d_color, defaulttype::Vec4f(1,1,0,1), "showColor", "Color for object display"))
    , d_indexPairs(initData(&d_indexPairs, "indexPairs", "list of couples (parent index + index in the parent)"))
{
}

template <class TIn, class TOut>
DistanceMultiMapping<TIn, TOut>::~DistanceMultiMapping()
{
    release();
}


template <class TIn, class TOut>
void DistanceMultiMapping<TIn, TOut>::init()
{
    edgeContainer = dynamic_cast<topology::EdgeSetTopologyContainer*>( this->getContext()->getMeshTopology() );
    if( !edgeContainer ) serr<<"No EdgeSetTopologyContainer found ! "<<sendl;

    SeqEdges links = edgeContainer->getEdges();

    this->getToModels()[0]->resize( links.size() );

    const vector<defaulttype::Vec2i>& pairs = d_indexPairs.getValue();

    // compute the rest lengths if they are not known
    if( f_restLengths.getValue().size() != links.size() )
    {
        helper::WriteAccessor< Data<vector<Real> > > restLengths(f_restLengths);
        restLengths.resize( links.size() );
        if(!(f_computeDistance.getValue()))
            for(unsigned i=0; i<links.size(); i++ )
            {
                const Vec2f& pair0 = pairs[ links[i][0] ];
                const Vec2f& pair1 = pairs[ links[i][1] ];

                const InCoord& pos0 = this->getFromModels()[pair0[0]]->readPositions()[pair0[1]];
                const InCoord& pos1 = this->getFromModels()[pair1[0]]->readPositions()[pair1[1]];

                restLengths[i] = (pos0 - pos1).norm();
            }
        else
            for(unsigned i=0; i<links.size(); i++ )
                restLengths[i] = (Real)0.;
    }

    alloc();

    this->Inherit::init();  // applies the mapping, so after the Data init
}

template <class TIn, class TOut>
void DistanceMultiMapping<TIn, TOut>::computeCoordPositionDifference( InDeriv& r, const InCoord& a, const InCoord& b )
{
    // default implementation
    TIn::setDPos(r, TIn::getDPos(TIn::coordDifference(b,a))); //Generic code working also for type!=particles but not optimize for particles
}

template <class TIn, class TOut>
void DistanceMultiMapping<TIn, TOut>::apply(const helper::vector<OutVecCoord*>& outPos, const vecConstInVecCoord& inPos)
{
    OutVecCoord& out = *outPos[0];


    const vector<defaulttype::Vec2i>& pairs = d_indexPairs.getValue();
    helper::ReadAccessor<Data<vector<Real> > > restLengths(f_restLengths);
    SeqEdges links = edgeContainer->getEdges();


    unsigned totalInSize = 0;
    for( unsigned i=0 ; i<this->getFromModels().size() ; ++i )
    {
        size_t insize = inPos[i]->size();
        static_cast<SparseMatrixEigen*>(baseMatrices[i])->resizeBlocks(out.size(),insize);
        totalInSize += insize;
    }
//    fullJ.resizeBlocks( out.size(), totalInSize  );
    K.resizeBlocks( totalInSize, totalInSize  );

    directions.resize(out.size());
    invlengths.resize(out.size());

    for(unsigned i=0; i<links.size(); i++ )
    {
        InDeriv& gap = directions[i];

        const Vec2f& pair0 = pairs[ links[i][0] ];
        const Vec2f& pair1 = pairs[ links[i][1] ];

        const InCoord& pos0 = (*inPos[pair0[0]])[pair0[1]];
        const InCoord& pos1 = (*inPos[pair1[0]])[pair1[1]];

        // gap = pos1-pos0 (only for position)
        computeCoordPositionDifference( gap, pos0, pos1 );

        Real gapNorm = gap.norm();
        out[i] = gapNorm - restLengths[i];  // output

        // normalize
        if( gapNorm>1.e-10 )
        {
            invlengths[i] = 1/gapNorm;
            gap *= invlengths[i];
        }
        else
        {
            invlengths[i] = 0;
            gap = InDeriv();
            gap[0]=1.0;  // arbitrary unit vector
        }

        for(unsigned j=0; j<Nout; j++)
        {
            for(unsigned k=0; k<Nin; k++ )
            {
                static_cast<SparseMatrixEigen*>(baseMatrices[pair0[0]])->add( i*Nout+j, pair0[1]*Nin+k, -gap[k] );
                static_cast<SparseMatrixEigen*>(baseMatrices[pair1[0]])->add( i*Nout+j, pair1[1]*Nin+k,  gap[k] );
            }
        }

    }


    for( unsigned i=0 ; i<baseMatrices.size() ; ++i )
    {
        baseMatrices[i]->compress();
    }

}


template <class TIn, class TOut>
void DistanceMultiMapping<TIn, TOut>::applyJ(const helper::vector<OutVecDeriv*>& outDeriv, const helper::vector<const  InVecDeriv*>& inDeriv)
{
    unsigned n = baseMatrices.size();
    unsigned i = 0;

    // let the first valid jacobian set its contribution    out = J_0 * in_0
    for( ; i < n ; ++i ) {
        const SparseMatrixEigen& J = *static_cast<SparseMatrixEigen*>(baseMatrices[i]);
        if( J.rowSize() > 0 ) {
            J.mult(*outDeriv[0], *inDeriv[i]);
            break;
        }
    }

    ++i;

    // the next valid jacobians will add their contributions    out += J_i * in_i
    for( ; i < n ; ++i ) {
        const SparseMatrixEigen& J = *static_cast<SparseMatrixEigen*>(baseMatrices[i]);
        if( J.rowSize() > 0 ) J.addMult(*outDeriv[0], *inDeriv[i]);
    }
}

template <class TIn, class TOut>
void DistanceMultiMapping<TIn, TOut>::applyJT(const helper::vector< InVecDeriv*>& outDeriv, const helper::vector<const OutVecDeriv*>& inDeriv)
{
    for( unsigned i = 0, n = baseMatrices.size(); i < n; ++i) {
        const SparseMatrixEigen& J = *static_cast<SparseMatrixEigen*>(baseMatrices[i]);
        if( J.rowSize() > 0 ) J.addMultTranspose(*outDeriv[i], *inDeriv[0]);
    }
}

template <class TIn, class TOut>
void DistanceMultiMapping<TIn, TOut>::applyDJT(const core::MechanicalParams* mparams, core::MultiVecDerivId inForce, core::ConstMultiVecDerivId)
{
//    serr<<SOFA_CLASS_METHOD<<"is not implemented"<<sendl;

    // NOT OPTIMIZED AT ALL, but will do the job for now

//    getK();

//    if( !K.compressedMatrix.nonZeros() ) return;

//    core::State<In>* fromModel0 = this->getFromModels()[0];
//    const Data<InVecDeriv>& parentDisplacementData0 = *mparams->readDx(fromModel0);
//    const InVecDeriv& pd0 = parentDisplacementData0.getValue();

//    core::State<In>* fromModel1 = this->getFromModels()[1];
//    const Data<InVecDeriv>& parentDisplacementData1 = *mparams->readDx(fromModel1);
//    const InVecDeriv& pd1 = parentDisplacementData1.getValue();

//    InVecDeriv fulldp( fromModel0->getSize() + fromModel1->getSize() );


//    std::copy( pd0.begin(), pd0.end(), fulldp.begin() );
//    std::copy( pd1.begin(), pd1.end(), fulldp.begin()+fromModel0->getSize() );
////    std::copy( mparams->readDx(this->getFromModels()[1])->begin(), mparams->readDx(this->getFromModels()[1])->end(), fulldp.begin()+this->getFromModels()[0]->getSize() );

//    InVecDeriv fulldf(fromModel0->getSize() + fromModel1->getSize());
//    K.addMult( fulldf, fulldp, mparams->kFactor() );

//    std::cerr<<fulldf<<"     "<<mparams->kFactor()<<std::endl;


//    std::cerr<<SOFA_CLASS_METHOD<<K<<std::endl;

//    size_t offset = 0;

//    for( unsigned i=0 ; i<this->getFromModels().size() ; ++i )
//    {
//        core::State<In>* fromModel = this->getFromModels()[i];
//        Data<InVecDeriv>& parentForceData = *inForce[fromModel].write();
//        const Data<InVecDeriv>& parentDisplacementData = *mparams->readDx(fromModel);


//        InVecDeriv& pf = *parentForceData.beginEdit();
//        const InVecDeriv& pd = parentDisplacementData.getValue();

//        typedef Eigen::Matrix< InReal, Eigen::Dynamic, 1 > EigenType;
//        typedef Eigen::Map< EigenType > MapType;
//        typedef Eigen::Map< const EigenType > ConstMapType;

//        size_t size = fromModel->getSize()*Nin;

//        MapType pfm = MapType( &pf[0][0], size );
//        ConstMapType pdm = ConstMapType( &pd[0][0], size );

//        SparseKMatrixEigen Klocal;
//        Klocal.compressedMatrix = K.compressedMatrix.middleRows(offset, size);


//        pfm.noalias() = pfm + Klocal.compressedMatrix * (pdm * mparams->kFactor());


////        std::cerr<<i<<"    "<<pf<<"     "<<pd<<" "<<K.compressedMatrix.middleRows(offset, size)<<std::endl;
//        std::cerr<<"$$$$$"<<Klocal<<std::endl<<std::endl<<pdm<<std::endl<<std::endl<<Klocal.compressedMatrix*pdm<<std::endl<<std::endl<<pfm<<std::endl;

//        parentForceData.endEdit();

//        offset += size;
//    }
}




template <class TIn, class TOut>
const vector<sofa::defaulttype::BaseMatrix*>* DistanceMultiMapping<TIn, TOut>::getJs()
{
    return &baseMatrices;
}

template <class TIn, class TOut>
const defaulttype::BaseMatrix* DistanceMultiMapping<TIn, TOut>::getK()
{
    const OutVecDeriv& childForce = this->getToModels()[0]->readForces().ref();
    SeqEdges links = edgeContainer->getEdges();
    const vector<defaulttype::Vec2i>& pairs = d_indexPairs.getValue();

    for(size_t i=0; i<links.size(); i++)
    {

        Mat<Nin,Nin,Real> b;  // = (I - uu^T)
        for(unsigned j=0; j<Nin; j++)
        {
            for(unsigned k=0; k<Nin; k++)
            {
                if( j==k )
                    b[j][k] = 1. - directions[i][j]*directions[i][k];
                else
                    b[j][k] =    - directions[i][j]*directions[i][k];
            }
        }
        b *= childForce[i][0] * invlengths[i];  // (I - uu^T)*f/l


        const Vec2f& pair0 = pairs[ links[i][0] ];
        const Vec2f& pair1 = pairs[ links[i][1] ];

        // TODO optimize (precompute base Index per mechanicalobject)
        size_t globalIndex0 = 0;
        for( unsigned i=0 ; i<pair0[0] ; ++i )
        {
            size_t insize = this->getFromModels()[i]->getSize();
            globalIndex0 += insize;
        }
        globalIndex0 += pair0[1];

        size_t globalIndex1 = 0;
        for( unsigned i=0 ; i<pair1[0] ; ++i )
        {
            size_t insize = this->getFromModels()[i]->getSize();
            globalIndex1 += insize;
        }
        globalIndex1 += pair1[1];

        K.beginBlockRow(globalIndex0);
        K.createBlock(globalIndex0,b);
        K.createBlock(globalIndex1,-b);
        K.endBlockRow();
        K.beginBlockRow(globalIndex1);
        K.createBlock(globalIndex0,-b);
        K.createBlock(globalIndex1,b);
        K.endBlockRow();
    }
    K.compress();

//    std::cerr<<SOFA_CLASS_METHOD<<"K : "<<K<<std::endl;


    return &K;
}

template <class TIn, class TOut>
void DistanceMultiMapping<TIn, TOut>::draw(const core::visual::VisualParams* vparams)
{
    if( !vparams->displayFlags().getShowMechanicalMappings() ) return;

    SeqEdges links = edgeContainer->getEdges();

    const vector<defaulttype::Vec2i>& pairs = d_indexPairs.getValue();

    if( d_showObjectScale.getValue() == 0 )
    {
        vector< Vector3 > points;
        for(unsigned i=0; i<links.size(); i++ )
        {
            const Vec2f& pair0 = pairs[ links[i][0] ];
            const Vec2f& pair1 = pairs[ links[i][1] ];

            const InCoord& pos0 = this->getFromModels()[pair0[0]]->readPositions()[pair0[1]];
            const InCoord& pos1 = this->getFromModels()[pair1[0]]->readPositions()[pair1[1]];

            points.push_back( Vector3( TIn::getCPos(pos0) ) );
            points.push_back( Vector3( TIn::getCPos(pos1) ) );
        }
        vparams->drawTool()->drawLines ( points, 1, d_color.getValue() );
    }
    else
    {
        for(unsigned i=0; i<links.size(); i++ )
        {
            const Vec2f& pair0 = pairs[ links[i][0] ];
            const Vec2f& pair1 = pairs[ links[i][1] ];

            const InCoord& pos0 = this->getFromModels()[pair0[0]]->readPositions()[pair0[1]];
            const InCoord& pos1 = this->getFromModels()[pair1[0]]->readPositions()[pair1[1]];

            Vector3 p0 = TIn::getCPos(pos0);
            Vector3 p1 = TIn::getCPos(pos1);
            vparams->drawTool()->drawCylinder( p0, p1, d_showObjectScale.getValue(), d_color.getValue() );
        }
    }
}


} // namespace mapping

} // namespace component

} // namespace sofa

#endif
