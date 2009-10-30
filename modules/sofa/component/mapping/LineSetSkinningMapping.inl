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

#ifndef SOFA_COMPONENT_MAPPING_LINESETSKINNINGMAPPING_INL
#define SOFA_COMPONENT_MAPPING_LINESETSKINNINGMAPPING_INL

#include <sofa/component/mapping/LineSetSkinningMapping.h>

namespace sofa
{

namespace component
{

namespace mapping
{

template <class BasicMapping>
Vec<3,double> LineSetSkinningMapping<BasicMapping>::projectToSegment( Vec<3,Real>& first, Vec<3,Real>& last, OutCoord& vertice)
{
    Vec3d segment,v_f,v_l;

    segment = last - first;
    v_f = vertice-first;
    v_l = vertice-last;

    if(v_f*segment>0.0 && -segment*v_l>0.0)
    {
        double prod = v_f*segment;
        return first + (segment * (prod/segment.norm2()));
    }
    else
    {
        if (v_l.norm() > v_f.norm())
            return first;
        else
            return last;
    }
}

template <class BasicMapping>
double LineSetSkinningMapping<BasicMapping>::convolutionSegment(Vec<3,Real>& first, Vec<3,Real>& last, OutCoord& vertice)
{
    int steps = 1000;
    double sum = 0.0;
    Vec<3,Real> dist, line;

    line=last-first;

    // False integration
    for(int i=0; i<=steps; i++)
    {
        dist = ((line * i) / steps) + first - vertice;
        sum += pow(1/dist.norm(),weightCoef.getValue());
    }

    sum *= line.norm()/steps;
    return sum;
}

template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::init()
{
    OutVecCoord& xto = *this->toModel->getX();
    InVecCoord& xfrom = *this->fromModel->getX();
    t = this->fromModel->getContext()->getMeshTopology();
    linesInfluencedByVertice.resize(xto.size());

    verticesInfluencedByLine.resize(t->getNbLines());

    neighborhoodLinesSet.resize(t->getNbLines());
    neighborhood.resize(t->getNbLines());

    for(unsigned int line1Index=0; line1Index< (unsigned) t->getNbLines(); line1Index++)
    {
        const sofa::core::componentmodel::topology::BaseMeshTopology::Line& line1 = t->getLine(line1Index);
        for(unsigned int line2Index=0; line2Index< (unsigned) t->getNbLines(); line2Index++)
        {
            const sofa::core::componentmodel::topology::BaseMeshTopology::Line& line2 = t->getLine(line2Index);
            if ((line1[0] == line2[0]) || (line1[0] == line2[1]) || (line1[1] == line2[0]))
            {
                neighborhoodLinesSet[line1Index].insert(line2Index);
            }
        }
    }

    for(unsigned int line1Index=0; line1Index< (unsigned) t->getNbLines(); line1Index++)
    {
        std::set<int> result;
        std::insert_iterator<std::set<int> > res_ins(result, result.begin());

        neighborhood[line1Index] = neighborhoodLinesSet[line1Index];

        for(unsigned int i=0; i<nvNeighborhood.getValue()-1; i++)
        {
            for (std::set<int>::const_iterator it = neighborhood[line1Index].begin(), itbegin = it, itend = neighborhood[line1Index].end(); it != itend; it++)
            {
                set_union(itbegin, itend, neighborhoodLinesSet[(*it)].begin(), neighborhoodLinesSet[(*it)].end(), res_ins);
            }

            neighborhood[line1Index] = result;
        }
    }

    for(unsigned int verticeIndex=0; verticeIndex<xto.size(); verticeIndex++)
    {
        double	sumWeights = 0.0;
        vector<influencedLineType> lines;
        lines.resize(t->getNbLines());

        for(unsigned int lineIndex=0; lineIndex< (unsigned) t->getNbLines(); lineIndex++)
        {
            const sofa::core::componentmodel::topology::BaseMeshTopology::Line& line = t->getLine(lineIndex);
            double _weight = convolutionSegment(xfrom[line[0]].getCenter(), xfrom[line[1]].getCenter(), xto[verticeIndex]);

            for(unsigned int lineInfluencedIndex=0; lineInfluencedIndex<lines.size(); lineInfluencedIndex++)
            {
                if(lines[lineInfluencedIndex].weight <= _weight)
                {
                    for(unsigned int i=lines.size()-1; i > lineInfluencedIndex; i--)
                    {
                        lines[i].lineIndex = lines[i-1].lineIndex;
                        lines[i].weight = lines[i-1].weight;
                        lines[i].position = lines[i-1].position;
                    }
                    lines[lineInfluencedIndex].lineIndex = lineIndex;
                    lines[lineInfluencedIndex].weight = _weight;
                    lines[lineInfluencedIndex].position = xfrom[line[0]].getOrientation().inverseRotate(xto[verticeIndex] - xfrom[line[0]].getCenter());
                    break;
                }
            }
        }

        unsigned int lineInfluencedIndex = 0;
        int max = lines[lineInfluencedIndex].lineIndex;
        sumWeights += lines[lineInfluencedIndex].weight;
        linesInfluencedByVertice[verticeIndex].push_back(lines[lineInfluencedIndex]);
        influencedVerticeType vertice;
        vertice.verticeIndex = verticeIndex;
        vertice.weight = lines[lineInfluencedIndex].weight;
        vertice.position = lines[lineInfluencedIndex].position;
        verticesInfluencedByLine[lineInfluencedIndex].push_back(vertice);

        lineInfluencedIndex++;

        while (linesInfluencedByVertice[verticeIndex].size() < numberInfluencedLines.getValue() && lineInfluencedIndex < lines.size())
        {
            if (neighborhood[max].count(lines[lineInfluencedIndex].lineIndex) != 0)
            {
                sumWeights += lines[lineInfluencedIndex].weight;
                linesInfluencedByVertice[verticeIndex].push_back(lines[lineInfluencedIndex]);
                vertice.verticeIndex = verticeIndex;
                vertice.weight = lines[lineInfluencedIndex].weight;
                vertice.position = lines[lineInfluencedIndex].position;
                verticesInfluencedByLine[lineInfluencedIndex].push_back(vertice);
            }
            lineInfluencedIndex++;
        }

        for (unsigned int influencedLineIndex=0; influencedLineIndex<linesInfluencedByVertice[verticeIndex].size(); influencedLineIndex++)
            linesInfluencedByVertice[verticeIndex][influencedLineIndex].weight /= sumWeights;
    }
}

template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::reinit()
{
    linesInfluencedByVertice.clear();
    verticesInfluencedByLine.clear();
    neighborhoodLinesSet.clear();
    neighborhood.clear();

    init();
}

template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::draw()
{
    if (!getShow(this)) return;
    glDisable (GL_LIGHTING);
    glLineWidth(1);

    glBegin (GL_LINES);

    OutVecCoord& xto = *this->toModel->getX();
    InVecCoord& xfrom = *this->fromModel->getX();

    for (unsigned int verticeIndex=0; verticeIndex<xto.size(); verticeIndex++)
    {
        //out[verticeIndex] = typename Out::Coord();
        for (unsigned int lineInfluencedIndex=0; lineInfluencedIndex<linesInfluencedByVertice[verticeIndex].size(); lineInfluencedIndex++)
        {

            influencedLineType iline = linesInfluencedByVertice[verticeIndex][lineInfluencedIndex];
            //Vec<3,Real> v = xfrom[t->getLine(iline.lineIndex)[0]].getCenter() + xfrom[t->getLine(iline.lineIndex)[0]].getOrientation().rotate(iline.position);
            const sofa::core::componentmodel::topology::BaseMeshTopology::Line& l = t->getLine(linesInfluencedByVertice[verticeIndex][lineInfluencedIndex].lineIndex);
            Vec<3,Real> v = projectToSegment(xfrom[l[0]].getCenter(), xfrom[l[1]].getCenter(), xto[verticeIndex]);


            glColor3f ((GLfloat) iline.weight, (GLfloat) 0, (GLfloat) (1.0-iline.weight));
            helper::gl::glVertexT(xto[verticeIndex]);
            helper::gl::glVertexT(v);

        }
    }


    /*
    for(unsigned int verticeIndex=0; verticeIndex<xto.size(); verticeIndex++)
    {
    	const sofa::core::componentmodel::topology::BaseMeshTopology::Line& line = t->getLine(linesInfluencedByVertice[verticeIndex][0].lineIndex);
    	Vec<3,Real> v = projectToSegment(xfrom[line[0]].getCenter(), xfrom[line[1]].getCenter(), xto[verticeIndex]);

    	glColor3f (1,0,0);
    	helper::gl::glVertexT(xto[verticeIndex]);
    	helper::gl::glVertexT(v);

    	for(unsigned int i=1; i<linesInfluencedByVertice[verticeIndex].size(); i++)
    	{
    		const sofa::core::componentmodel::topology::BaseMeshTopology::Line& l = t->getLine(linesInfluencedByVertice[verticeIndex][i].lineIndex);
    		Vec<3,Real> v = projectToSegment(xfrom[l[0]].getCenter(), xfrom[l[1]].getCenter(), xto[verticeIndex]);

    		glColor3f (0,0,1);
    		helper::gl::glVertexT(xto[verticeIndex]);
    		helper::gl::glVertexT(v);
    	}
    }
    */
    glEnd();
}

template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::apply( typename Out::VecCoord& out, const typename In::VecCoord& in )
{
    for (unsigned int verticeIndex=0; verticeIndex<out.size(); verticeIndex++)
    {
        out[verticeIndex] = typename Out::Coord();
        for (unsigned int lineInfluencedIndex=0; lineInfluencedIndex<linesInfluencedByVertice[verticeIndex].size(); lineInfluencedIndex++)
        {
            influencedLineType iline = linesInfluencedByVertice[verticeIndex][lineInfluencedIndex];
            out[verticeIndex] += in[t->getLine(iline.lineIndex)[0]].getCenter()*iline.weight;
            out[verticeIndex] += in[t->getLine(iline.lineIndex)[0]].getOrientation().rotate(iline.position*iline.weight);
        }
    }
}

template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in )
{
    InVecCoord& xfrom = *this->fromModel->getX();


    for (unsigned int verticeIndex=0; verticeIndex<out.size(); verticeIndex++)
    {
        out[verticeIndex] = typename Out::Deriv();
        for (unsigned int lineInfluencedIndex=0; lineInfluencedIndex<linesInfluencedByVertice[verticeIndex].size(); lineInfluencedIndex++)
        {
            influencedLineType iline = linesInfluencedByVertice[verticeIndex][lineInfluencedIndex];
            Vec<3,Real> IP = xfrom[t->getLine(iline.lineIndex)[0]].getOrientation().rotate(iline.position);
            out[verticeIndex] += (in[t->getLine(iline.lineIndex)[0]].getVCenter() - IP.cross(in[t->getLine(iline.lineIndex)[0]].getVOrientation())) * iline.weight;
        }
    }
}


template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in )
{

    //std::cerr<<"LineSetSkinningMapping::applyJT called   - out :"<<out<<std::endl;



    InVecCoord& xfrom = *this->fromModel->getX();
    out.clear();
    out.resize(xfrom.size());


    for (unsigned int verticeIndex=0; verticeIndex<in.size(); verticeIndex++)
    {
        typename Out::Deriv f = in[verticeIndex];

        for (unsigned int lineInfluencedIndex=0; lineInfluencedIndex<linesInfluencedByVertice[verticeIndex].size(); lineInfluencedIndex++)
        {
            influencedLineType iline = linesInfluencedByVertice[verticeIndex][lineInfluencedIndex];
            unsigned int I =t->getLine(iline.lineIndex)[0];

            Vec<3,Real> IP = xfrom[I].getOrientation().rotate(iline.position);

            out[I].getVCenter() += f * iline.weight;
            out[I].getVOrientation() += IP.cross(f) *  iline.weight;

        }
    }


    /*
    	for(unsigned int lineIndex=0; lineIndex< (unsigned) t->getNbLines(); lineIndex++)
    	{
    		unsigned int I = t->getLine(lineIndex)[0];
    		for (unsigned int verticeInfluencedIndex=0; verticeInfluencedIndex<verticesInfluencedByLine[lineIndex].size(); verticeInfluencedIndex++)
    		{
    			influencedVerticeType vertice = verticesInfluencedByLine[lineIndex][verticeInfluencedIndex];
    			Vec<3,Real> IP = xfrom[I].getOrientation().rotate(vertice.position);
    			out[I].getVCenter() += in[vertice.verticeIndex] * vertice.weight;
    			out[I].getVOrientation() += IP.cross(in[vertice.verticeIndex]) * vertice.weight;
    		}
    	}
    */



}

template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::applyJT( typename In::VecConst& out, const typename Out::VecConst& in )
{
    //out.clear();
    unsigned int outSize = out.size();
    out.resize(outSize+in.size());

    InVecCoord& xfrom = *this->fromModel->getX();

    //serr<<"applyJT for constraints called : in.size() = "<<in.size()<<sendl;

    for(unsigned int i=0; i<in.size(); i++)
    {
        OutConstraintIterator itOut;
        std::pair< OutConstraintIterator, OutConstraintIterator > iter=in[i].data();

        for (itOut=iter.first; itOut!=iter.second; itOut++)
        {
            unsigned int indexIn = itOut->first;
            OutDeriv data = (OutDeriv) itOut->second;
            int verticeIndex = indexIn;
            const OutDeriv d = data;
            //printf(" normale : %f %f %f",d.x(), d.y(), d.z());
            for(unsigned int lineInfluencedIndex=0; lineInfluencedIndex<linesInfluencedByVertice[verticeIndex].size(); lineInfluencedIndex++)
            {
                influencedLineType iline = linesInfluencedByVertice[verticeIndex][lineInfluencedIndex];
                Vec<3,Real> IP = xfrom[t->getLine(iline.lineIndex)[0]].getOrientation().rotate(iline.position);
                InDeriv direction;
                direction.getVCenter() = d * iline.weight;
                //printf("\n Weighted normale : %f %f %f",direction.getVCenter().x(), direction.getVCenter().y(), direction.getVCenter().z());
                direction.getVOrientation() = IP.cross(d) * iline.weight;
                out[outSize+i].add(t->getLine(iline.lineIndex)[0], direction);
            }
        }
    }



}


template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::propagateX()
{
    if (this->fromModel!=NULL && this->toModel->getX()!=NULL && this->fromModel->getX()!=NULL)
        apply(*this->toModel->getX(), *this->fromModel->getX() );

    if( this->f_printLog.getValue())
    {
        serr<<"propageX processed :"<<sendl;
        serr<<"  - input: "<<*this->fromModel->getX()<<"  output : "<<*this->toModel->getX()<<sendl;
    }

}

template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::propagateXfree()
{
    if (this->fromModel!=NULL && this->toModel->getXfree()!=NULL && this->fromModel->getXfree()!=NULL)
        apply(*this->toModel->getXfree(), *this->fromModel->getXfree());

    if( this->f_printLog.getValue())
    {
        serr<<"propageXfree processed"<<sendl;
        serr<<"  - input: "<<*this->fromModel->getXfree()<<"  output : "<<*this->toModel->getXfree()<<sendl;
    }
}


template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::propagateV()
{
    if (this->fromModel!=NULL && this->toModel->getV()!=NULL && this->fromModel->getV()!=NULL)
        applyJ(*this->toModel->getV(), *this->fromModel->getV());

    if( this->f_printLog.getValue())
    {
        serr<<" propagateV processed"<<sendl;
        serr<<"  - V input: "<<*this->fromModel->getV()<<"   V output : "<<*this->toModel->getV()<<sendl;
    }

}



template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::propagateDx()
{

    if (this->fromModel!=NULL && this->toModel->getDx()!=NULL && this->fromModel->getDx()!=NULL)
        applyJ(*this->toModel->getDx(), *this->fromModel->getDx());

    if( this->f_printLog.getValue())
    {
        serr<<"propagateDx processed"<<sendl;
        serr<<"  - input: "<<*this->fromModel->getDx()<<"  output : "<<*this->toModel->getDx()<<sendl;
    }
}



template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::accumulateForce()
{
    if (this->fromModel!=NULL && this->toModel->getF()!=NULL && this->fromModel->getF()!=NULL)
        applyJT(*this->fromModel->getF(), *this->toModel->getF());

    if( this->f_printLog.getValue())
    {
        serr<<"accumulateForce processed"<<sendl;
        serr<<" input f : "<<*this->toModel->getF();
        serr<<"  - output F: "<<*this->fromModel->getF()<<sendl;
    }
}



template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::accumulateDf()
{
    if (this->fromModel!=NULL && this->toModel->getF()!=NULL && this->fromModel->getF()!=NULL)
        applyJT(*this->fromModel->getF(), *this->toModel->getF());

    if( this->f_printLog.getValue())
    {
        serr<<"accumulateDf processed"<<sendl;
        serr<<" input df : "<<*this->toModel->getF();
        serr<<"  - output: "<<*this->fromModel->getF()<<sendl;
    }
}



template <class BasicMapping>
void LineSetSkinningMapping<BasicMapping>::accumulateConstraint()
{
    if (this->fromModel!=NULL && this->toModel->getC()!=NULL && this->fromModel->getC()!=NULL)
    {
        applyJT(*this->fromModel->getC(), *this->toModel->getC());

        // Accumulate contacts indices through the MechanicalMapping
        std::vector<unsigned int>::iterator it = this->toModel->getConstraintId().begin();
        std::vector<unsigned int>::iterator itEnd = this->toModel->getConstraintId().end();


        while (it != itEnd)
        {
            this->fromModel->setConstraintId(*it);
            it++;
        }
    }


}





} // namespace mapping

} // namespace component

} // namespace sofa

#endif
