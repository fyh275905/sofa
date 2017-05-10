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
#ifndef SOFA_COMPONENT_MAPPING_SquareDistanceMapping_H
#define SOFA_COMPONENT_MAPPING_SquareDistanceMapping_H
#include "config.h"

#include <sofa/core/Mapping.h>
#include <sofa/core/MultiMapping.h>
#include <SofaEigen2Solver/EigenSparseMatrix.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/defaulttype/Vec.h>


namespace sofa
{

namespace component
{

namespace mapping
{



/** Maps point positions to square distances.
  Type TOut corresponds to a scalar value.

  The pairs are given as couples (dof_i, dof_j) in Data 'pairs'.

    In: parent point positions
    Out: square distance between point pairs (minus a square rest distance.)

    @warning No restLength (imposed null rest length) for now
    TODO: compute Jacobians for non null restLength

@author Matthieu Nesme
  */


// If the rest lengths are not defined, they are set using the initial values.
// If computeDistance is set to true, the rest lengths are set to 0.
template <class TIn, class TOut>
class SquareDistanceMapping : public core::Mapping<TIn, TOut>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(SquareDistanceMapping,TIn,TOut), SOFA_TEMPLATE2(core::Mapping,TIn,TOut));

    typedef core::Mapping<TIn, TOut> Inherit;
    typedef TIn In;
    typedef TOut Out;
    typedef typename Out::VecCoord OutVecCoord;
    typedef typename Out::VecDeriv OutVecDeriv;
    typedef typename Out::Coord OutCoord;
    typedef typename Out::Deriv OutDeriv;
    typedef typename Out::MatrixDeriv OutMatrixDeriv;
    typedef typename Out::Real Real;
    typedef typename In::Deriv InDeriv;
    typedef typename In::MatrixDeriv InMatrixDeriv;
    typedef typename In::Coord InCoord;
    typedef typename In::VecCoord InVecCoord;
    typedef typename In::VecDeriv InVecDeriv;
    typedef linearsolver::EigenSparseMatrix<TIn,TOut>   SparseMatrixEigen;
    typedef linearsolver::EigenSparseMatrix<TIn,TIn>    SparseKMatrixEigen;
    typedef Data<InVecCoord> InDataVecCoord;
    typedef Data<InVecDeriv> InDataVecDeriv;
    typedef Data<InMatrixDeriv> InDataMatrixDeriv;
    typedef Data<OutVecCoord> OutDataVecCoord;
    typedef Data<OutVecDeriv> OutDataVecDeriv;
    typedef Data<OutMatrixDeriv> OutDataMatrixDeriv;
    enum {Nin = In::deriv_total_size, Nout = Out::deriv_total_size };
    typedef defaulttype::Vec<In::spatial_dimensions,Real> Direction;


//    Data< bool >		   f_computeDistance;	///< computeDistance = true ---> restDistance = 0
//    Data< helper::vector< Real > > f_restLengths;		///< rest length of each link


    typedef defaulttype::Vec2u Pair; ///< links (dof_0, dof_1)
    typedef helper::vector<Pair> VecPair;
    Data<VecPair> d_pairs;

    Data< Real >           d_showObjectScale;   ///< drawing size
    Data< defaulttype::Vec4f > d_color;         ///< drawing color
    Data< unsigned >       d_geometricStiffness; ///< how to compute geometric stiffness (0->no GS, 1->exact GS, 2->stabilized GS)

    virtual void init();

    using Inherit::apply;

    virtual void apply(const core::MechanicalParams *mparams, Data<OutVecCoord>& out, const Data<InVecCoord>& in);

    virtual void applyJ(const core::MechanicalParams *mparams, Data<OutVecDeriv>& out, const Data<InVecDeriv>& in);

    virtual void applyJT(const core::MechanicalParams *mparams, Data<InVecDeriv>& out, const Data<OutVecDeriv>& in);

    virtual void applyJT(const core::ConstraintParams *cparams, Data<InMatrixDeriv>& out, const Data<OutMatrixDeriv>& in);

    virtual void applyDJT(const core::MechanicalParams* mparams, core::MultiVecDerivId parentForce, core::ConstMultiVecDerivId  childForce );

    virtual const sofa::defaulttype::BaseMatrix* getJ();
    virtual const helper::vector<sofa::defaulttype::BaseMatrix*>* getJs();

    virtual void updateK( const core::MechanicalParams* mparams, core::ConstMultiVecDerivId childForce );
    virtual const defaulttype::BaseMatrix* getK();

    virtual void draw(const core::visual::VisualParams* vparams);

    virtual void updateForceMask();

protected:
    SquareDistanceMapping();
    virtual ~SquareDistanceMapping();

    SparseMatrixEigen jacobian;                         ///< Jacobian of the mapping
    helper::vector<defaulttype::BaseMatrix*> baseMatrices;      ///< Jacobian of the mapping, in a vector
    SparseKMatrixEigen K;                               ///< Assembled geometric stiffness matrix

    /// r=b-a only for position (eventual rotation, affine transform... remains null)
    void computeCoordPositionDifference( Direction& r, const InCoord& a, const InCoord& b );
};


//////////////////////////




/** Maps point positions from several mstates to square distances (in distance unit).
  Type TOut corresponds to a scalar value.

  The link indices are given as ((mstate0_index,dof0_index),(mstate1_index,dof1_index)) in pairs.
  If 'pairs' is empty and two mstates have the same size -> computing a links in-between each mstate dofs.

  (If the rest lengths are not defined, they are set using the initial values.)
  (If computeDistance is set to true, the rest lengths are set to 0.)

    In: parent point positions
    Out: distance between point pairs, minus a rest distance.




    @warning No restLength (imposed null rest length) for now
    TODO: compute Jacobians for non null restLength

@author Matthieu Nesme @date 2017
  */
template <class TIn, class TOut>
class SquareDistanceMultiMapping : public core::MultiMapping<TIn, TOut>
{
public:
    SOFA_CLASS(SOFA_TEMPLATE2(SquareDistanceMultiMapping,TIn,TOut), SOFA_TEMPLATE2(core::MultiMapping,TIn,TOut));

    typedef core::MultiMapping<TIn, TOut> Inherit;
    typedef TIn In;
    typedef TOut Out;
    typedef typename Out::VecCoord OutVecCoord;
    typedef typename Out::VecDeriv OutVecDeriv;
    typedef typename Out::Coord OutCoord;
    typedef typename Out::Deriv OutDeriv;
    typedef typename Out::MatrixDeriv OutMatrixDeriv;
    typedef typename Out::Real Real;
    typedef typename In::Real InReal;
    typedef typename In::Deriv InDeriv;
    typedef typename In::MatrixDeriv InMatrixDeriv;
    typedef typename In::Coord InCoord;
    typedef typename In::VecCoord InVecCoord;
    typedef typename In::VecDeriv InVecDeriv;
    typedef Data<InVecCoord> InDataVecCoord;
    typedef Data<InVecDeriv> InDataVecDeriv;
    typedef Data<InMatrixDeriv> InDataMatrixDeriv;
    typedef Data<OutVecCoord> OutDataVecCoord;
    typedef Data<OutVecDeriv> OutDataVecDeriv;
    typedef Data<OutMatrixDeriv> OutDataMatrixDeriv;
    typedef linearsolver::EigenSparseMatrix<TIn,TOut>   SparseMatrixEigen;
    typedef linearsolver::EigenSparseMatrix<TIn,TIn>    SparseKMatrixEigen;
    enum {Nin = In::deriv_total_size, Nout = Out::deriv_total_size };
    typedef typename helper::vector <const InVecCoord*> vecConstInVecCoord;
    typedef defaulttype::Vec<In::spatial_dimensions,Real> Direction;



    typedef defaulttype::Vec2u Index; ///< (mstate_index, dof_index)
    typedef defaulttype::Vec<2,Index> Pair; ///< a link between two dofs
    typedef helper::vector<Pair> VecPair;
    Data<VecPair> d_pairs; ///< vector of ((mstate_0,dof_0),(mstate_1,dof_1))


//    Data< bool >		   f_computeDistance;	///< computeDistance = true ---> restDistance = 0
//    Data< helper::vector< Real > > f_restLengths;		///< rest length of each link
    Data< Real >           d_showObjectScale;   ///< drawing size
    Data< defaulttype::Vec4f > d_color;         ///< drawing color
    Data< unsigned >       d_geometricStiffness; ///< how to compute geometric stiffness (0->no GS, 1->exact GS, 2->stabilized GS)

    virtual void init();
    virtual void parse( sofa::core::objectmodel::BaseObjectDescription* arg );

    virtual void apply(const core::MechanicalParams *mparams, const helper::vector<OutDataVecCoord*>& dataVecOutPos, const helper::vector<const InDataVecCoord*>& dataVecInPos)
    {
        //Not optimized at all...
        helper::vector<OutVecCoord*> vecOutPos;
        for(unsigned int i=0; i<dataVecOutPos.size(); i++)
            vecOutPos.push_back(dataVecOutPos[i]->beginEdit(mparams));

        helper::vector<const InVecCoord*> vecInPos;
        for(unsigned int i=0; i<dataVecInPos.size(); i++)
            vecInPos.push_back(&dataVecInPos[i]->getValue(mparams));

        this->apply(vecOutPos, vecInPos);

        //Really Not optimized at all...
        for(unsigned int i=0; i<dataVecOutPos.size(); i++)
            dataVecOutPos[i]->endEdit(mparams);

    }

    virtual void applyJ(const core::MechanicalParams *mparams, const helper::vector<OutDataVecDeriv*>& dataVecOutVel, const helper::vector<const InDataVecDeriv*>& dataVecInVel)
    {
        //Not optimized at all...
        helper::vector<OutVecDeriv*> vecOutVel;
        for(unsigned int i=0; i<dataVecOutVel.size(); i++)
            vecOutVel.push_back(dataVecOutVel[i]->beginEdit(mparams));

        helper::vector<const InVecDeriv*> vecInVel;
        for(unsigned int i=0; i<dataVecInVel.size(); i++)
            vecInVel.push_back(&dataVecInVel[i]->getValue(mparams));

        this->applyJ(vecOutVel, vecInVel);

        //Really Not optimized at all...
        for(unsigned int i=0; i<dataVecOutVel.size(); i++)
            dataVecOutVel[i]->endEdit(mparams);

    }

    virtual void applyJT(const core::MechanicalParams *mparams, const helper::vector<InDataVecDeriv*>& dataVecOutForce, const helper::vector<const OutDataVecDeriv*>& dataVecInForce)
    {
        //Not optimized at all...
        helper::vector<InVecDeriv*> vecOutForce;
        for(unsigned int i=0; i<dataVecOutForce.size(); i++)
            vecOutForce.push_back(dataVecOutForce[i]->beginEdit(mparams));

        helper::vector<const OutVecDeriv*> vecInForce;
        for(unsigned int i=0; i<dataVecInForce.size(); i++)
            vecInForce.push_back(&dataVecInForce[i]->getValue(mparams));

        this->applyJT(vecOutForce, vecInForce);

        //Really Not optimized at all...
        for(unsigned int i=0; i<dataVecOutForce.size(); i++)
            dataVecOutForce[i]->endEdit(mparams);

    }

    using Inherit::apply;
    using Inherit::applyJ;
    using Inherit::applyJT;

    virtual void apply(const helper::vector<OutVecCoord*>& outPos, const vecConstInVecCoord& inPos);
    virtual void applyJ(const helper::vector<OutVecDeriv*>& outDeriv, const helper::vector<const  InVecDeriv*>& inDeriv);
    virtual void applyJT(const helper::vector< InVecDeriv*>& outDeriv, const helper::vector<const OutVecDeriv*>& inDeriv);
    virtual void applyJT( const core::ConstraintParams* /* cparams */, const helper::vector< InDataMatrixDeriv* >& /* dataMatOutConst */, const helper::vector< const OutDataMatrixDeriv* >& /* dataMatInConst */ ) {}
    virtual void applyDJT(const core::MechanicalParams*, core::MultiVecDerivId inForce, core::ConstMultiVecDerivId outForce);

    virtual const helper::vector<sofa::defaulttype::BaseMatrix*>* getJs();

    virtual void updateK( const core::MechanicalParams* mparams, core::ConstMultiVecDerivId childForce );
    virtual const defaulttype::BaseMatrix* getK();

    virtual void draw(const core::visual::VisualParams* vparams);

    virtual void updateForceMask();

protected:
    SquareDistanceMultiMapping();
    virtual ~SquareDistanceMultiMapping();

    helper::vector<defaulttype::BaseMatrix*> baseMatrices;      ///< Jacobian of the mapping, in a vector
    helper::vector<Direction> directions;                         ///< Unit vectors in the directions of the lines
    helper::vector< Real > invlengths;                          ///< inverse of current distances. Null represents the infinity (null distance)

    SparseKMatrixEigen K;

    /// r=b-a only for position (eventual rotation, affine transform... remains null)
    void computeCoordPositionDifference( Direction& r, const InCoord& a, const InCoord& b );


private:

  // allocate jacobians
  virtual void alloc() {
      const unsigned n = this->getFrom().size();
      if( n != baseMatrices.size() ) {
          release( n ); // will only do something if n<oldsize
          size_t oldsize = baseMatrices.size();
          baseMatrices.resize( n );
          for( unsigned i = oldsize ; i < n ; ++i ) // will only do something if n>oldsize
              baseMatrices[i] = new SparseMatrixEigen;
      }
  }

  // delete jacobians
  void release( size_t from=0 ) {
      for( unsigned i = from, n = baseMatrices.size(); i < n; ++i) {
          delete baseMatrices[i];
          baseMatrices[i] = 0;
      }
  }


};


#if defined(SOFA_EXTERN_TEMPLATE) && !defined(SOFA_COMPONENT_MAPPING_SquareDistanceMapping_CPP)
#ifndef SOFA_FLOAT
extern template class SOFA_MISC_MAPPING_API SquareDistanceMapping< defaulttype::Vec3dTypes, defaulttype::Vec1dTypes >;
extern template class SOFA_MISC_MAPPING_API SquareDistanceMapping< defaulttype::Rigid3dTypes, defaulttype::Vec1dTypes >;
extern template class SOFA_MISC_MAPPING_API SquareDistanceMultiMapping< defaulttype::Vec3dTypes, defaulttype::Vec1dTypes >;
extern template class SOFA_MISC_MAPPING_API SquareDistanceMultiMapping< defaulttype::Rigid3dTypes, defaulttype::Vec1dTypes >;
#endif
#ifndef SOFA_DOUBLE
extern template class SOFA_MISC_MAPPING_API SquareDistanceMapping< defaulttype::Vec3fTypes, defaulttype::Vec1fTypes >;
extern template class SOFA_MISC_MAPPING_API SquareDistanceMapping< defaulttype::Rigid3fTypes, defaulttype::Vec1fTypes >;
extern template class SOFA_MISC_MAPPING_API SquareDistanceMultiMapping< defaulttype::Vec3fTypes, defaulttype::Vec1fTypes >;
extern template class SOFA_MISC_MAPPING_API SquareDistanceMultiMapping< defaulttype::Rigid3fTypes, defaulttype::Vec1fTypes >;
#endif

#endif

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
