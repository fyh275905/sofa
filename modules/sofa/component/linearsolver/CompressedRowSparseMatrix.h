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
#ifndef SOFA_COMPONENT_LINEARSOLVER_COMPRESSEDROWSPARSEMATRIX_H
#define SOFA_COMPONENT_LINEARSOLVER_COMPRESSEDROWSPARSEMATRIX_H

#include <sofa/defaulttype/BaseMatrix.h>
#include <sofa/component/linearsolver/MatrixLinearSolver.h>
#include "FullVector.h"
#include <algorithm>

namespace sofa
{

namespace component
{

namespace linearsolver
{

//#define SPARSEMATRIX_CHECK
//#define SPARSEMATRIX_VERBOSE

template<int TN> class bloc_index_func
{
public:
    enum { N = TN };
    static void split(int& index, int& modulo)
    {
        modulo = index % N;
        index  = index / N;
    }
};

template<> class bloc_index_func<1>
{
public:
    enum { N = 1 };
    static void split(int&, int&)
    {
    }
};

template<> class bloc_index_func<2>
{
public:
    enum { N = 2 };
    static void split(int& index, int& modulo)
    {
        modulo = index & 1;
        index  = index >> 1;
    }
};

template<> class bloc_index_func<4>
{
public:
    enum { N = 2 };
    static void split(int& index, int& modulo)
    {
        modulo = index & 3;
        index  = index >> 2;
    }
};

template<> class bloc_index_func<8>
{
public:
    enum { N = 2 };
    static void split(int& index, int& modulo)
    {
        modulo = index & 7;
        index  = index >> 3;
    }
};

template<class T>
class matrix_bloc_traits;

template <int L, int C, class real>
class matrix_bloc_traits < defaulttype::Mat<L,C,real> >
{
public:
    typedef defaulttype::Mat<L,C,real> Bloc;
    typedef real Real;
    enum { NL = L };
    enum { NC = C };
    static Real& v(Bloc& b, int row, int col) { return b[row][col]; }
    static const Real& v(const Bloc& b, int row, int col) { return b[row][col]; }
    static void clear(Bloc& b) { b.clear(); }
    static bool empty(const Bloc& b)
    {
        for (int i=0; i<NL; ++i)
            for (int j=0; j<NC; ++j)
                if (b[i][j] != 0) return false;
        return true;
    }
    static const char* Name();
};

template<> inline const char* matrix_bloc_traits<defaulttype::Mat<1,1,float > >::Name() { return "1f"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<1,1,double> >::Name() { return "1d"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<2,2,float > >::Name() { return "2f"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<2,2,double> >::Name() { return "2d"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<3,3,float > >::Name() { return "3f"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<3,3,double> >::Name() { return "3d"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<4,4,float > >::Name() { return "4f"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<4,4,double> >::Name() { return "4d"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<6,6,float > >::Name() { return "6f"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<6,6,double> >::Name() { return "6d"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<8,8,float > >::Name() { return "8f"; }
template<> inline const char* matrix_bloc_traits<defaulttype::Mat<8,8,double> >::Name() { return "8d"; }

template <>
class matrix_bloc_traits < float >
{
public:
    typedef float Bloc;
    typedef float Real;
    enum { NL = 1 };
    enum { NC = 1 };
    static Real& v(Bloc& b, int, int) { return b; }
    static const Real& v(const Bloc& b, int, int) { return b; }
    static void clear(Bloc& b) { b = 0; }
    static bool empty(const Bloc& b)
    {
        return b == 0;
    }
    static const char* Name() { return "f"; }
};

template <>
class matrix_bloc_traits < double >
{
public:
    typedef double Bloc;
    typedef double Real;
    enum { NL = 1 };
    enum { NC = 1 };
    static Real& v(Bloc& b, int, int) { return b; }
    static const Real& v(const Bloc& b, int, int) { return b; }
    static void clear(Bloc& b) { b = 0; }
    static bool empty(const Bloc& b)
    {
        return b == 0;
    }
    static const char* Name() { return "d"; }
};

template<typename TBloc, typename TVecBloc = helper::vector<TBloc>, typename TVecIndex = helper::vector<int> >
class CompressedRowSparseMatrix : public defaulttype::BaseMatrix
{
public:
    typedef CompressedRowSparseMatrix<TBloc,TVecBloc,TVecIndex> Matrix;
    typedef TBloc Bloc;
    typedef matrix_bloc_traits<Bloc> traits;
    typedef typename traits::Real Real;
    enum { NL = traits::NL };
    enum { NC = traits::NC };
    typedef int Index;
    typedef TVecBloc VecBloc;
    typedef TVecIndex VecIndex;
    struct IndexedBloc
    {
        Index l,c;
        Bloc value;
        IndexedBloc() {}
        IndexedBloc(Index i, Index j) : l(i), c(j) {}
        IndexedBloc(Index i, Index j, const Bloc& v) : l(i), c(j), value(v) {}
        bool operator < (const IndexedBloc& b) const
        {
            return (l < b.l) || (l == b.l && c < b.c);
        }
        bool operator <= (const IndexedBloc& b) const
        {
            return (l < b.l) || (l == b.l && c <= b.c);
        }
        bool operator > (const IndexedBloc& b) const
        {
            return (l > b.l) || (l == b.l && c > b.c);
        }
        bool operator >= (const IndexedBloc& b) const
        {
            return (l > b.l) || (l == b.l && c >= b.c);
        }
        bool operator == (const IndexedBloc& b) const
        {
            return (l == b.l) && (c == b.c);
        }
        bool operator != (const IndexedBloc& b) const
        {
            return (l != b.l) || (c != b.c);
        }
    };
    typedef helper::vector<IndexedBloc> VecIndexedBloc;

    static void split_row_index(int& index, int& modulo) { bloc_index_func<NL>::split(index, modulo); }
    static void split_col_index(int& index, int& modulo) { bloc_index_func<NC>::split(index, modulo); }

    class Range : public std::pair<Index, Index>
    {
        typedef std::pair<Index, Index> Inherit;
    public:
        Range() : Inherit(0,0) {}
        Range(Index begin, Index end) : Inherit(begin,end) {}
        Index begin() const { return this->first; }
        Index end() const { return this->second; }
        void setBegin(Index i) { this->first = i; }
        void setEnd(Index i) { this->second = i; }
        bool empty() const { return begin() == end(); }
        Index size() const { return end()-begin(); }
        typename VecBloc::iterator begin(VecBloc& b) const { return b.begin() + begin(); }
        typename VecBloc::iterator end  (VecBloc& b) const { return b.end  () + end  (); }
        typename VecBloc::const_iterator begin(const VecBloc& b) const { return b.begin() + begin(); }
        typename VecBloc::const_iterator end  (const VecBloc& b) const { return b.end  () + end  (); }
        typename VecIndex::iterator begin(VecIndex& b) const { return b.begin() + begin(); }
        typename VecIndex::iterator end  (VecIndex& b) const { return b.end  () + end  (); }
        typename VecIndex::const_iterator begin(const VecIndex& b) const { return b.begin() + begin(); }
        typename VecIndex::const_iterator end  (const VecIndex& b) const { return b.end  () + end  (); }
        void operator++() { ++first; }
        void operator++(int) { ++first; }
    };

    static bool sortedFind(const VecIndex& v, Range in, Index val, Index& result)
    {
        if (in.empty()) return false;
        Index candidate = (result >= in.begin() && result < in.end()) ? result : ((in.begin() + in.end()) >> 1);
        for(;;)
        {
            Index i = v[candidate];
            if (i == val) { result = candidate; return true; }
            if (i < val)  in.setBegin(candidate+1);
            else          in.setEnd(candidate);
            if (in.empty()) break;
            candidate = (in.begin() + in.end()) >> 1;
        }
        return false;
    }

    static bool sortedFind(const VecIndex& v, Index val, Index& result)
    {
        return sortedFind(v, Range(0,v.size()), val, result);
    }

protected:
    Index nRow,nCol;
    Index nBlocRow,nBlocCol;
    bool compressed;
    VecIndex rowIndex;
    VecIndex rowBegin;
    VecIndex colsIndex;
    VecBloc  colsValue;
    VecIndexedBloc btemp;

    // Temporary vectors used during compression
    VecIndex oldRowIndex;
    VecIndex oldRowBegin;
    VecIndex oldColsIndex;
    VecBloc  oldColsValue;
public:
    CompressedRowSparseMatrix()
        : nRow(0), nCol(0), nBlocRow(0), nBlocCol(0), compressed(true)
    {
    }

    CompressedRowSparseMatrix(int nbRow, int nbCol)
        : nRow(nbRow), nCol(nbCol), compressed(true)
    {
        nBlocRow = (nRow + NL-1) / NL;
        nBlocCol = (nCol + NC-1) / NC;
    }

    unsigned int rowBSize() const
    {
        return nBlocRow;
    }

    unsigned int colBSize() const
    {
        return nBlocCol;
    }

    const VecIndex& getRowIndex() const { return rowIndex; }
    const VecIndex& getRowBegin() const { return rowBegin; }
    Range getRowRange(int id) const { return Range(rowBegin[id], rowBegin[id+1]); }
    const VecIndex& getColsIndex() const { return colsIndex; }
    const VecBloc& getColsValue() const { return colsValue; }

    void resizeBloc(int nbBRow, int nbBCol)
    {
        if (nBlocRow == nbBRow && nBlocRow == nbBCol)
        {
            // just clear the matrix
            for (unsigned int i=0; i < colsValue.size(); ++i)
                traits::clear(colsValue[i]);
            compressed = colsValue.empty();
            btemp.clear();
        }
        else
        {
#ifdef SPARSEMATRIX_VERBOSE
            std::cout << /* this->Name()  <<  */": resize("<<nbBRow<<"*"<<NL<<","<<nbBCol<<"*"<<NC<<")"<<std::endl;
#endif
            nBlocRow = nbBRow;
            nBlocCol = nbBCol;
            rowIndex.clear();
            rowBegin.clear();
            colsIndex.clear();
            colsValue.clear();
            compressed = true;
            btemp.clear();
        }
    }

    void compress()
    {
        if (compressed && btemp.empty()) return;
        if (!btemp.empty())
        {
#ifdef SPARSEMATRIX_VERBOSE
            std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): sort "<<btemp.size()<<" temp blocs."<<std::endl;
#endif
            std::sort(btemp.begin(),btemp.end());
#ifdef SPARSEMATRIX_VERBOSE
            std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): blocs sorted."<<std::endl;
#endif
        }
        oldRowIndex.swap(rowIndex);
        oldRowBegin.swap(rowBegin);
        oldColsIndex.swap(colsIndex);
        oldColsValue.swap(colsValue);
        rowIndex.clear();
        rowBegin.clear();
        colsIndex.clear();
        colsValue.clear();
        rowIndex.reserve(oldRowIndex.empty() ? nBlocRow : oldRowIndex.size());
        rowBegin.reserve((oldRowIndex.empty() ? nBlocRow : oldRowIndex.size())+1);
        colsIndex.reserve(oldColsIndex.size() + btemp.size());
        colsValue.reserve(oldColsIndex.size() + btemp.size());
        const Index oldNRow = oldRowIndex.size();
        const Index EndRow = nBlocRow;
        const Index EndCol = nBlocCol;
        //const Index EndVal = oldColsIndex.size();
        Index inRowId = 0;
        Index inRowIndex = (inRowId < oldNRow ) ? oldRowIndex[inRowId] : EndRow;
        typename VecIndexedBloc::const_iterator itbtemp = btemp.begin(), endbtemp = btemp.end();
        Index bRowIndex = (itbtemp != endbtemp) ? itbtemp->l : EndRow;
        Index outValId = 0;
        while (inRowIndex < EndRow || bRowIndex < EndRow)
        {
#ifdef SPARSEMATRIX_VERBOSE
            std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): inRowIndex = "<<inRowIndex<<" , bRowIndex = "<<bRowIndex<<""<<std::endl;
#endif
            if (inRowIndex < bRowIndex)
            {
                // this row contains values only from old*
                rowIndex.push_back(inRowIndex);
                rowBegin.push_back(outValId);
                Range inRow( oldRowBegin[inRowId], oldRowBegin[inRowId+1] );
                while (!inRow.empty())
                {
                    if (!traits::empty(oldColsValue[inRow.begin()]))
                    {
                        colsIndex.push_back(oldColsIndex[inRow.begin()]);
                        colsValue.push_back(oldColsValue[inRow.begin()]);
                        ++outValId;
                    }
                    ++inRow;
                }
                //colsIndex.insert(colsIndex.end(), inRow.begin(oldColsIndex), inRow.end(oldColsIndex));
                //colsValue.insert(colsValue.end(), inRow.begin(oldColsValue), inRow.end(oldColsValue));
                //outValId += inRow.size();
                ++inRowId;
                inRowIndex = (inRowId < oldNRow ) ? oldRowIndex[inRowId] : EndRow;
            }
            else if (inRowIndex > bRowIndex)
            {
                // this row contains values only from btemp
                rowIndex.push_back(bRowIndex);
                rowBegin.push_back(outValId);
                while (itbtemp != endbtemp && itbtemp->l == bRowIndex)
                {
                    Index bColIndex = itbtemp->c;
                    colsIndex.push_back(bColIndex);
                    colsValue.push_back(itbtemp->value);
                    ++itbtemp;
                    Bloc& value = colsValue.back();
                    while (itbtemp != endbtemp && itbtemp->c == bColIndex && itbtemp->l == bRowIndex)
                    {
                        value += itbtemp->value;
                        ++itbtemp;
                    }
                    ++outValId;
                }
                bRowIndex = (itbtemp != endbtemp) ? itbtemp->l : EndRow;
            }
            else
            {
                // this row mixes values from old* and btemp
                rowIndex.push_back(inRowIndex);
                rowBegin.push_back(outValId);
                Range inRow( oldRowBegin[inRowId], oldRowBegin[inRowId+1] );
                Index inColIndex = (!inRow.empty()) ? oldColsIndex[inRow.begin()] : EndCol;
                Index bColIndex = (itbtemp != endbtemp && itbtemp->l == inRowIndex) ? itbtemp->c : EndCol;
                while (inColIndex < EndCol || bColIndex < EndCol)
                {
                    if (inColIndex < bColIndex)
                    {
                        if (!traits::empty(oldColsValue[inRow.begin()]))
                        {
                            colsIndex.push_back(inColIndex);
                            colsValue.push_back(oldColsValue[inRow.begin()]);
                            ++outValId;
                        }
                        ++inRow;
                        inColIndex = (!inRow.empty()) ? oldColsIndex[inRow.begin()] : EndCol;
                    }
                    else if (inColIndex > bColIndex)
                    {
                        colsIndex.push_back(bColIndex);
                        colsValue.push_back(itbtemp->value);
                        ++itbtemp;
                        Bloc& value = colsValue.back();
                        while (itbtemp != endbtemp && itbtemp->c == bColIndex && itbtemp->l == bRowIndex)
                        {
                            value += itbtemp->value;
                            ++itbtemp;
                        }
                        bColIndex = (itbtemp != endbtemp && itbtemp->l == bRowIndex) ? itbtemp->c : EndCol;
                        ++outValId;
                    }
                    else
                    {
                        colsIndex.push_back(inColIndex);
                        colsValue.push_back(oldColsValue[inRow.begin()]);
                        ++inRow;
                        inColIndex = (!inRow.empty()) ? oldColsIndex[inRow.begin()] : EndCol;
                        Bloc& value = colsValue.back();
                        while (itbtemp != endbtemp && itbtemp->c == bColIndex && itbtemp->l == bRowIndex)
                        {
                            value += itbtemp->value;
                            ++itbtemp;
                        }
                        bColIndex = (itbtemp != endbtemp && itbtemp->l == bRowIndex) ? itbtemp->c : EndCol;
                        ++outValId;
                    }
                }
                ++inRowId;
                inRowIndex = (inRowId < oldNRow ) ? oldRowIndex[inRowId] : EndRow;
                bRowIndex = (itbtemp != endbtemp) ? itbtemp->l : EndRow;
            }
        }
        rowBegin.push_back(outValId);
//#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): compressed " << oldColsIndex.size()<<" old blocs and " << btemp.size() << " temp blocs into " << rowIndex.size() << " lines and " << colsIndex.size() << " blocs."<<std::endl;
//#endif
        btemp.clear();
        compressed = true;
    }

    const Bloc& bloc(int i, int j) const
    {
        static Bloc empty;
        int rowId = i * rowIndex.size() / nBlocRow;
        if (sortedFind(rowIndex, i, rowId))
        {
            Range rowRange(rowBegin[rowId], rowBegin[rowId+1]);
            Index colId = rowRange.begin() + j * rowRange.size() / nBlocCol;
            if (sortedFind(colsIndex, rowRange, j, colId))
            {
                return colsValue[colId];
            }
        }
        return empty;
    }

    Bloc* wbloc(int i, int j, bool create = false)
    {
        int rowId = i * rowIndex.size() / nBlocRow;
        if (sortedFind(rowIndex, i, rowId))
        {
            Range rowRange(rowBegin[rowId], rowBegin[rowId+1]);
            int colId = rowRange.begin() + j * rowRange.size() / nBlocCol;
            if (sortedFind(colsIndex, rowRange, j, colId))
            {
#ifdef SPARSEMATRIX_VERBOSE
                std::cout << /* this->Name()  <<  */"("<<rowBSize()<<"*"<<NL<<","<<colBSize()<<"*"<<NC<<"): bloc("<<i<<","<<j<<") found at "<<colId<<" (line "<<rowId<<")."<<std::endl;
#endif
                return &colsValue[colId];
            }
        }
        if (create)
        {
            if (btemp.empty() || btemp.back().l != i || btemp.back().c != j)
            {
#ifdef SPARSEMATRIX_VERBOSE
                std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): new temp bloc ("<<i<<","<<j<<")"<<std::endl;
#endif
                btemp.push_back(IndexedBloc(i,j));
                traits::clear(btemp.back().value);
            }
            return &btemp.back().value;
        }
        return NULL;
    }

    unsigned int rowSize() const
    {
        return nRow;
    }

    unsigned int colSize() const
    {
        return nCol;
    }

    void resize(int nbRow, int nbCol)
    {
#ifdef SPARSEMATRIX_VERBOSE
        if (nbRow != (int)rowSize() || nbCol != (int)colSize())
            std::cout << /* this->Name()  <<  */": resize("<<nbRow<<","<<nbCol<<")"<<std::endl;
#endif
        nRow = nbRow;
        nCol = nbCol;
        resizeBloc((nRow + NL-1) / NL, (nCol + NC-1) / NC);
    }

    SReal element(int i, int j) const
    {
#ifdef SPARSEMATRIX_CHECK
        if ((unsigned)i >= (unsigned)rowSize() || (unsigned)j >= (unsigned)colSize())
        {
            std::cerr << "ERROR: invalid read access to element ("<<i<<","<<j<<") in "<</* this->Name() <<*/" of size ("<<rowSize()<<","<<colSize()<<")"<<std::endl;
            return 0.0;
        }
#endif
        int bi=0, bj=0; split_row_index(i, bi); split_col_index(j, bj);
        ((Matrix*)this)->compress();
        return traits::v(bloc(i, j), bi, bj);
    }

    void set(int i, int j, double v)
    {
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): element("<<i<<","<<j<<") = "<<v<<std::endl;
#endif
#ifdef SPARSEMATRIX_CHECK
        if ((unsigned)i >= (unsigned)rowSize() || (unsigned)j >= (unsigned)colSize())
        {
            std::cerr << "ERROR: invalid write access to element ("<<i<<","<<j<<") in "<</* this->Name() <<*/" of size ("<<rowSize()<<","<<colSize()<<")"<<std::endl;
            return;
        }
#endif
        int bi=0, bj=0; split_row_index(i, bi); split_col_index(j, bj);
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowBSize()<<"*"<<NL<<","<<colBSize()<<"*"<<NC<<"): bloc("<<i<<","<<j<<")["<<bi<<","<<bj<<"] = "<<v<<std::endl;
#endif
        traits::v(*wbloc(i,j,true), bi, bj) = (Real)v;
    }

    void add(int i, int j, double v)
    {
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): element("<<i<<","<<j<<") += "<<v<<std::endl;
#endif
#ifdef SPARSEMATRIX_CHECK
        if ((unsigned)i >= (unsigned)rowSize() || (unsigned)j >= (unsigned)colSize())
        {
            std::cerr << "ERROR: invalid write access to element ("<<i<<","<<j<<") in "<</* this->Name() <<*/" of size ("<<rowSize()<<","<<colSize()<<")"<<std::endl;
            return;
        }
#endif
        int bi=0, bj=0; split_row_index(i, bi); split_col_index(j, bj);
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowBSize()<<"*"<<NL<<","<<colBSize()<<"*"<<NC<<"): bloc("<<i<<","<<j<<")["<<bi<<","<<bj<<"] += "<<v<<std::endl;
#endif
        traits::v(*wbloc(i,j,true), bi, bj) += (Real)v;
    }

    void clear(int i, int j)
    {
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): element("<<i<<","<<j<<") = 0"<<std::endl;
#endif
#ifdef SPARSEMATRIX_CHECK
        if ((unsigned)i >= (unsigned)rowSize() || (unsigned)j >= (unsigned)colSize())
        {
            std::cerr << "ERROR: invalid write access to element ("<<i<<","<<j<<") in "<</* this->Name() <<*/" of size ("<<rowSize()<<","<<colSize()<<")"<<std::endl;
            return;
        }
#endif
        int bi=0, bj=0; split_row_index(i, bi); split_col_index(j, bj);
        compress();
        Bloc* b = wbloc(i,j,false);
        if (b)
            traits::v(*b, bi, bj) = 0;
    }

    void clearRow(int i)
    {
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): row("<<i<<") = 0"<<std::endl;
#endif
#ifdef SPARSEMATRIX_CHECK
        if ((unsigned)i >= (unsigned)rowSize())
        {
            std::cerr << "ERROR: invalid write access to row "<<i<<" in "<</* this->Name() <<*/" of size ("<<rowSize()<<","<<colSize()<<")"<<std::endl;
            return;
        }
#endif
        int bi=0; split_row_index(i, bi);
        compress();
        /*
        for (int j=0; j<nBlocCol; ++j)
        {
            Bloc* b = wbloc(i,j,false);
            if (b)
            {
                for (int bj = 0; bj < NC; ++bj)
                    traits::v(*b, bi, bj) = 0;
            }
        }
        */
        int rowId = i * rowIndex.size() / nBlocRow;
        if (sortedFind(rowIndex, i, rowId))
        {
            Range rowRange(rowBegin[rowId], rowBegin[rowId+1]);
            for (int xj = rowRange.begin(); xj < rowRange.end(); ++xj)
            {
                Bloc& b = colsValue[xj];
                for (int bj = 0; bj < NC; ++bj)
                    traits::v(b, bi, bj) = 0;
            }
        }
    }

    void clearCol(int j)
    {
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): col("<<j<<") = 0"<<std::endl;
#endif
#ifdef SPARSEMATRIX_CHECK
        if ((unsigned)j >= (unsigned)colSize())
        {
            std::cerr << "ERROR: invalid write access to column "<<j<<" in "<</* this->Name() <<*/" of size ("<<rowSize()<<","<<colSize()<<")"<<std::endl;
            return;
        }
#endif
        int bj=0; split_col_index(j, bj);
        compress();
        for (int i=0; i<nBlocRow; ++i)
        {
            Bloc* b = wbloc(i,j,false);
            if (b)
            {
                for (int bi = 0; bi < NL; ++bi)
                    traits::v(*b, bi, bj) = 0;
            }
        }
    }

    void clearRowCol(int i)
    {
#ifdef SPARSEMATRIX_VERBOSE
        std::cout << /* this->Name()  <<  */"("<<rowSize()<<","<<colSize()<<"): row("<<i<<") = 0 and col("<<i<<") = 0"<<std::endl;
#endif
#ifdef SPARSEMATRIX_CHECK
        if ((unsigned)i >= (unsigned)rowSize() || (unsigned)i >= (unsigned)colSize())
        {
            std::cerr << "ERROR: invalid write access to row and column "<<i<<" in "<</* this->Name() <<*/" of size ("<<rowSize()<<","<<colSize()<<")"<<std::endl;
            return;
        }
#endif
        if ((int)NL != (int)NC || nRow != nCol)
        {
            clearRow(i);
            clearCol(i);
        }
        else
        {
            // Here we assume the matrix is symmetric
            int bi=0; split_row_index(i, bi);
            compress();
            int rowId = i * rowIndex.size() / nBlocRow;
            if (sortedFind(rowIndex, i, rowId))
            {
                Range rowRange(rowBegin[rowId], rowBegin[rowId+1]);
                for (int xj = rowRange.begin(); xj < rowRange.end(); ++xj)
                {
                    Bloc& b = colsValue[xj];
                    for (int bj = 0; bj < NC; ++bj)
                        traits::v(b, bi, bj) = 0;
                    int j = colsIndex[xj];
                    if (j != i)
                    {
                        // non diagonal bloc
                        Bloc* b = wbloc(i,j,false);
                        if (b)
                        {
                            for (int bj = 0; bj < NL; ++bj)
                                traits::v(*b, bi, bj) = 0;
                        }
                    }
                }
            }
        }
    }

    void clear()
    {
        for (unsigned int i=0; i < colsValue.size(); ++i)
            traits::clear(colsValue[i]);
        compressed = colsValue.empty();
        btemp.clear();
    }

protected:
    template<class Real2>
    static Real vget(const defaulttype::BaseVector& vec, int i) { return vec.element(i); }
    template<class Real2> static Real2 vget(const FullVector<Real2>& vec, int i) { return vec[i]; }
    static void vset(defaulttype::BaseVector& vec, int i, Real v) { vec.set(i, v); }
    template<class Real2> static void vset(FullVector<Real2>& vec, int i, Real2 v) { vec[i] = v; }
    static void vadd(defaulttype::BaseVector& vec, int i, Real v) { vec.add(i, v); }
    template<class Real2> static void vadd(FullVector<Real2>& vec, int i, Real2 v) { vec[i] += v; }
public:

    template<class Real2, class V1, class V2>
    void tmul(V1& res, const V2& vec) const
    {
        ((Matrix*)this)->compress();
        res.resize(rowSize());
        for (unsigned int xi = 0; xi < rowIndex.size(); ++xi)
        {
            Index iN = rowIndex[xi] * NL;
            Range rowRange(rowBegin[xi], rowBegin[xi+1]);
            defaulttype::Vec<NL,Real2> r;
            for (int xj = rowRange.begin(); xj < rowRange.end(); ++xj)
            {
                Index jN = colsIndex[xj] * NC;
                const Bloc& b = colsValue[xj];
                defaulttype::Vec<NC,Real2> v;
                for (int bj = 0; bj < NC; ++bj)
                    v[bj] = vget(vec,jN + bj);
                for (int bi = 0; bi < NL; ++bi)
                    for (int bj = 0; bj < NC; ++bj)
                        r[bi] += traits::v(b, bi, bj) * v[bj];
            }
            for (int bi = 0; bi < NL; ++bi)
                vset(res, iN + bi, r[bi]);
        }
    }

    template<class Real2, class V1, class V2>
    void tmulTranspose(V1& res, const V2& vec) const
    {
        ((Matrix*)this)->compress();
        res.resize(colSize());
        for (unsigned int xi = 0; xi < rowIndex.size(); ++xi)
        {
            Index iN = rowIndex[xi] * NL;
            Range rowRange(rowBegin[xi], rowBegin[xi+1]);
            defaulttype::Vec<NL,Real2> v;
            for (int bi = 0; bi < NL; ++bi)
                v[bi] = vget(vec, iN + bi);
            for (int xj = rowRange.begin(); xj < rowRange.end(); ++xj)
            {
                Index jN = colsIndex[xj] * NC;
                const Bloc& b = colsValue[xj];
                defaulttype::Vec<NC,Real2> r;
                for (int bj = 0; bj < NC; ++bj)
                    r[bj] = traits::v(b, 0, bj) * v[0];
                for (int bi = 1; bi < NL; ++bi)
                    for (int bj = 0; bj < NC; ++bj)
                        r[bj] += traits::v(b, bi, bj) * v[bi];
                for (int bj = 0; bj < NC; ++bj)
                    vadd(res, jN + bj, r[bj]);
            }
        }
    }

    template<class Real2>
    void mul(FullVector<Real2>& res, const FullVector<Real2>& v) const
    {
        tmul< Real2, FullVector<Real2>, FullVector<Real2> >(res, v);
    }

    template<class Real2>
    void mulTranspose(FullVector<Real2>& res, const FullVector<Real2>& v) const
    {
        tmulTranspose< Real2, FullVector<Real2>, FullVector<Real2> >(res, v);
    }

    template<class Real2>
    void mul(FullVector<Real2>& res, const defaulttype::BaseVector* v) const
    {
        tmul< Real2, FullVector<Real2>, defaulttype::BaseVector >(res, *v);
    }

    template<class Real2>
    void mulTranspose(FullVector<Real2>& res, const defaulttype::BaseVector* v) const
    {
        tmulTranspose< Real2, FullVector<Real2>, defaulttype::BaseVector >(res, *v);
    }

    template<class Real2>
    void mul(defaulttype::BaseVector* res, const FullVector<Real2>& v) const
    {
        tmul< Real2, defaulttype::BaseVector, FullVector<Real2> >(*res, v);
    }

    template<class Real2>
    void mulTranspose(defaulttype::BaseVector*& res, const FullVector<Real2>& v) const
    {
        tmulTranspose< Real2, defaulttype::BaseVector, FullVector<Real2> >(*res, v);
    }

    template<class Real2>
    void mul(defaulttype::BaseVector* res, const defaulttype::BaseVector* v) const
    {
        tmul< Real, defaulttype::BaseVector, defaulttype::BaseVector >(*res, *v);
    }

    void mulTranspose(defaulttype::BaseVector* res, const defaulttype::BaseVector* v) const
    {
        tmul< Real, defaulttype::BaseVector, defaulttype::BaseVector >(*res, *v);
    }

    template<class Real2>
    FullVector<Real2> operator*(const FullVector<Real2>& v) const
    {
        FullVector<Real2> res;
        mul(res,v);
        return res;
    }

    friend std::ostream& operator << (std::ostream& out, const Matrix& v )
    {
        int nx = v.colSize();
        int ny = v.rowSize();
        out << "[";
        for (int y=0; y<ny; ++y)
        {
            out << "\n[";
            for (int x=0; x<nx; ++x)
            {
                out << " " << v.element(y,x);
            }
            out << " ]";
        }
        out << " ]";
        return out;
    }

    static const char* Name()
    {
        static std::string name = std::string("CompressedRowSparseMatrix") + std::string(traits::Name());
        return name.c_str();
    }
};

#ifdef SPARSEMATRIX_CHECK
#undef SPARSEMATRIX_CHECK
#endif
#ifdef SPARSEMATRIX_VERBOSE
#undef SPARSEMATRIX_VERBOSE
#endif

} // namespace linearsolver

} // namespace component

} // namespace sofa

#endif
