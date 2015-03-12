#include <SofaEigen2Solver/EigenSparseMatrix.h>

namespace sofa {




// some helpers
template<class Matrix>
bool zero(const Matrix& m) {
    return !m.nonZeros();
}

template<class Matrix>
bool notempty(const Matrix* m) {
    return m && m->rows();
}

template<class Matrix>
bool empty(const Matrix& m) {
    return !m.rows();
}


/// test if present values are all zero
/// @warning not optimized for eigen sparse matrices
//template<class SparseMatrix>
//bool fillWithZeros(const SparseMatrix& m) {
//    for( unsigned i=0 ; i<m.data().size() ; ++i )
//    {
//        if( m.valuePtr()[i] != 0 ) return false;
//    }
//    return true;
//}


template<class LValue, class RValue>
static void add(LValue& lval, const RValue& rval) {
    if( empty(lval) ) {
        lval = rval;
    } else {
        // paranoia, i has it
        lval += rval;
    }
}

// hopefully avoids a temporary alloc/dealloc for product
template<class LValue, class LHS, class RHS>
static void add_prod(LValue& lval, const LHS& lhs, const RHS& rhs) {
    if( empty(lval) ) {
        lval = lhs * rhs;
    } else {
        // paranoia, i has it
        lval = lval + lhs * rhs;
    }
}


template<class dofs_type>
static std::string pretty(dofs_type* dofs) {
    return dofs->getContext()->getName() + " / " + dofs->getName();
}


// right-shift matrix, size x (off + size) matrix: (0, id)
template<class mat>
static mat shift_right(unsigned off, unsigned size, unsigned total_cols, SReal value = 1.0 ) {
    mat res( size, total_cols);
    assert( total_cols >= (off + size) );

    res.reserve( size );

    for(unsigned i = 0; i < size; ++i) {
        res.startVec( i );
        res.insertBack(i, off + i) = value;
    }
    res.finalize();

    return res;
}

// left-shift matrix, (off + size) x size matrix: (0 ; id)
template<class mat>
static mat shift_left(unsigned off, unsigned size, unsigned total_rows, SReal value = 1.0 ) {
    mat res( total_rows, size);
    assert( total_rows >= (off + size) );

    res.reserve( size );

    for(unsigned i = 0; i < size; ++i) {
        res.startVec( off + i );
        res.insertBack(off + i, i) = value;
    }
    res.finalize();

    return res;
}

template<class Triplet, class mat>
static void add_shifted_right( std::vector<Triplet>& res, const mat& m, unsigned off, SReal factor = 1.0 )
{
    for( int k=0 ; k<m.outerSize() ; ++k )
        for( typename mat::InnerIterator it(m,k) ; it ; ++it )
        {
            res.push_back( Triplet( off+it.row(), off+it.col(), it.value()*factor ) );
        }
}

template<class mat>
static void add_shifted_right( mat& res, const mat& m, unsigned off, SReal factor = 1.0 )
{
    for( int k=0 ; k<m.outerSize() ; ++k )
        for( typename mat::InnerIterator it(m,k) ; it ; ++it )
        {
            res.coeffRef(off+it.row(), off+it.col()) += it.value()*factor;
        }
}

template<class densemat, class mat>
static void add_shifted_right( densemat& res, const mat& m, unsigned off, SReal factor = 1.0 )
{
    for( int k=0 ; k<m.outerSize() ; ++k )
        for( typename mat::InnerIterator it(m,k) ; it ; ++it )
        {
            res(off+it.row(), off+it.col()) += it.value()*factor;
        }
}


/// return the shifted matrix @m (so returns a larger matrix with nb col = @total_cols) where @m begin at the index @off (m is possibly multiplied by @factor)
template<class real>
static Eigen::SparseMatrix<real, Eigen::RowMajor> shifted_matrix( const Eigen::SparseMatrix<real, Eigen::RowMajor>& m, unsigned off, unsigned total_cols, real factor = 1.0 )
{
    // let's play with eigen black magic

#if 0

    Eigen::SparseMatrix<real, Eigen::RowMajor> res = m * factor; // weighted by factor
    res.conservativeResize( m.rows(), total_cols );
    for( unsigned i=0 ; i<res.data().size() ; ++i )
    {
        res.innerIndexPtr()[i] += off; // where the shifting occurs
    }

#else

    const_cast<Eigen::SparseMatrix<real, Eigen::RowMajor>&>(m).makeCompressed();
    Eigen::SparseMatrix<real, Eigen::RowMajor> res( m.rows(), total_cols );

    res.data() = m.data();
    for( unsigned i=0 ; i<res.data().size() ; ++i )
    {
        res.valuePtr()[i] *= factor; // weighted by factor
        res.innerIndexPtr()[i] += off; // where the shifting occurs
    }

    memcpy(res.outerIndexPtr(), m.outerIndexPtr(), (m.outerSize()+1)*sizeof(typename Eigen::SparseMatrix<real, Eigen::RowMajor>::Index));

    assert( !res.innerNonZeroPtr() ); // should be NULL because compressed // *res.innerNonZeroPtr() = *m.innerNonZeroPtr();

#endif

    return res;
}

template<class densemat, class mat>
static void convertDenseToSparse( mat& res, const densemat& m )
{
    for( int i=0 ; i<m.rows() ; ++i )
    {
        res.startVec( i );
        for( int j=0 ; j<m.cols() ; ++j )
            if( m(i,j) ) res.insertBack(i, j) = m(i,j);
    }
    res.finalize();
}

// convert a basematrix to a sparse matrix. TODO move this somewhere else ?
template<class mat>
mat convert( const defaulttype::BaseMatrix* m) {
    assert( m );

    {
    typedef component::linearsolver::EigenBaseSparseMatrix<double> matrixd;
    const matrixd* smd = dynamic_cast<const matrixd*> (m);
    if ( smd ) return smd->compressedMatrix.cast<SReal>();
    }

    {
    typedef component::linearsolver::EigenBaseSparseMatrix<float> matrixf;
    const matrixf* smf = dynamic_cast<const matrixf*>(m);
    if( smf ) return smf->compressedMatrix.cast<SReal>();
    }

    std::cerr << "warning: slow matrix conversion (AssemblyHelper)" << std::endl;

    mat res(m->rowSize(), m->colSize());

    res.reserve(res.rows() * res.cols());
    for(unsigned i = 0, n = res.rows(); i < n; ++i) {
        res.startVec( i );
        for(unsigned j = 0, k = res.cols(); j < k; ++j) {
            SReal e = m->element(i, j);
            if( e ) res.insertBack(i, j) = e;
        }
    }

    return res;
}



/// Smart pointer that can point to an existing data without taking ownership
/// Or that can point to a new temporary Data that must be deleted when this
/// smart pointer is deleted (taking ownership)
// maybe an equivalent smart pointer exists in boost but I do not now
template<class T>
class MySPtr
{
    const T* t;
    mutable bool ownership;
public:
    MySPtr() : t(NULL), ownership(false) {}
    MySPtr( const T* t, bool ownership ) : t(t), ownership(ownership) {}
    MySPtr( const MySPtr<T>& other ) : t(other.t), ownership(other.ownership) { other.ownership=false; }
    ~MySPtr() { if( ownership ) delete t; }
    void operator=(const MySPtr<T>& other) { t=other.t; ownership=other.ownership; other.ownership=false; }
    const T& operator*() const { return *t; }
    const T* operator->() const { return t; }
};


// convert a basematrix to a sparse matrix. TODO move this somewhere else ?
template<class mat>
MySPtr<mat> convertSPtr( const defaulttype::BaseMatrix* m) {
    assert( m );

    {
    typedef component::linearsolver::EigenBaseSparseMatrix<SReal> matrixr;
    const matrixr* smr = dynamic_cast<const matrixr*> (m);
    if ( smr ) return MySPtr<mat>(&smr->compressedMatrix, false);
    }

    {
    typedef component::linearsolver::EigenBaseSparseMatrix<double> matrixd;
    const matrixd* smd = dynamic_cast<const matrixd*> (m);
    if ( smd ) return MySPtr<mat>( new mat(smd->compressedMatrix.cast<SReal>()), true );
    }

    {
    typedef component::linearsolver::EigenBaseSparseMatrix<float> matrixf;
    const matrixf* smf = dynamic_cast<const matrixf*>(m);
    if( smf ) return MySPtr<mat>( new mat(smf->compressedMatrix.cast<SReal>()), true );
    }


    std::cerr << "warning: slow matrix conversion (AssemblyHelper)" << std::endl;

    mat* res = new mat(m->rowSize(), m->colSize());

    res->reserve(res->rows() * res->cols());
    for(unsigned i = 0, n = res->rows(); i < n; ++i) {
        res->startVec( i );
        for(unsigned j = 0, k = res->cols(); j < k; ++j) {
            SReal e = m->element(i, j);
            if( e ) res->insertBack(i, j) = e;
        }
    }

    return MySPtr<mat>(res, true);
}




}

