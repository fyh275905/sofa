/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
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
#ifndef WinchMultiMapping_H
#define WinchMultiMapping_H

#include <Compliant/config.h>
#include <Compliant/mapping/AssembledMultiMapping.h>

namespace sofa
{
	
namespace component
{

namespace mapping
{

/**
 Multi-maps a distance and a rotation vectors, to an error for associating a rotation to an elongation. 

 (d, r) -> a*r - d
 (Where a is a real factor that should depends on )

 This is used to obtain relative dofs
 on which a stiffness/compliance may be applied
*/

template <class TIn, class TOut >
class WinchMultiMapping : public AssembledMultiMapping<TIn, TOut>
{
    typedef WinchMultiMapping self;

public:
    SOFA_CLASS(SOFA_TEMPLATE2(WinchMultiMapping,TIn,TOut), SOFA_TEMPLATE2(core::MultiMapping,TIn,TOut));

    typedef AssembledMultiMapping<TIn, TOut> Inherit;
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
    typedef linearsolver::EigenSparseMatrix<TIn,TOut>    SparseMatrixEigen;

    typedef typename helper::vector <const InVecCoord*> vecConstInVecCoord;
    typedef typename helper::vector<OutVecCoord*> vecOutVecCoord;

    enum {Nin = In::deriv_total_size, Nout = Out::deriv_total_size };

    virtual void init()
    {
        this->getToModels()[0]->resize( this->getFromModels()[0]->getSize() );
        AssembledMultiMapping<TIn, TOut>::init();
    }

    virtual void reinit()
    {
        this->getToModels()[0]->resize( this->getFromModels()[0]->getSize() );
        AssembledMultiMapping<TIn, TOut>::reinit();
    }

    virtual void apply(typename self::out_pos_type& out,
                        const helper::vector<typename self::in_pos_type>& in)  {
        Real f = factor.getValue();

        for( size_t j = 0, m = in[0].size(); j < m; ++j) {
            out[j] = f * TIn::getCPos( in[1] [j] ) - TIn::getCPos( in[0] [j] );
        }

    }

    Data< Real > factor;

protected:

    WinchMultiMapping()
        : factor( initData(&factor, Real(1.0),"factor", "factor representing the ratio between a rotation and an elongation") ) {

    }

    void assemble(const helper::vector<typename self::in_pos_type>& in ) {

        Real f = factor.getValue();

        for(size_t i = 0, n = in.size(); i < n; ++i) {

            typename Inherit::jacobian_type::CompressedMatrix& J = this->jacobian(i).compressedMatrix;

            J.resize( Nout * in[i].size(), Nin * in[i].size());
            J.setZero();

            Real sign = (i == 0) ? -1 : f;

            for(size_t k = 0, n = in[i].size(); k < n; ++k) {
                write_block(J, k, k, sign);
            }

            J.finalize();
        }
    }



    // write sign * identity in jacobian(obj)
    void write_block(typename Inherit::jacobian_type::CompressedMatrix& J,
                        unsigned row, unsigned col,
                        SReal sign) {
        assert( Nout == Nin );

        // for each coordinate in matrix block
        for( unsigned i = 0, n = Nout; i < n; ++i ) {
            unsigned r = row * Nout + i;
            unsigned c = col * Nin + i;

            J.startVec(r);
            J.insertBack(r, c) = sign;
        }
    }


    virtual void updateForceMask()
    {

    }

};

#if !defined(SOFA_COMPONENT_MAPPING_WINCHMULTIMAPPING_CPP)
extern template class SOFA_Compliant_API WinchMultiMapping<sofa::defaulttype::Vec1Types, sofa::defaulttype::Vec1Types >;
#endif

}
}
}

#endif // WinchMultiMapping_H
