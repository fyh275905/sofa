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
#include <SofaBaseCollision/CubeModel.h>

#include <sofa/core/visual/VisualParams.h>
#include <sofa/simulation/Simulation.h>
#include <sofa/core/ObjectFactory.h>
#include <algorithm>
#include <cmath>

namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

int CubeCollisionModelClass = core::RegisterObject("Collision model representing a cube")
        .add< CubeCollisionModel >()
        .addAlias("Cube")
        .addAlias("CubeModel")
        ;

CubeCollisionModel::CubeCollisionModel()
{
    enum_type = AABB_TYPE;
}

void CubeCollisionModel::resize(std::size_t size)
{
    std::size_t size0 = this->size;
    if (size == size0) return;
    // reset parent
    CollisionModel* parent = getPrevious();
    while(parent != nullptr)
    {
        parent->resize(0);
        parent = parent->getPrevious();
    }
    this->core::CollisionModel::resize(size);
    this->elems.resize(size);
    this->parentOf.resize(size);
    // set additional indices
    for (std::size_t i=size0; i<size; ++i)
    {
        this->elems[i].children.first=core::CollisionElementIterator(getNext(), i);
        this->elems[i].children.second=core::CollisionElementIterator(getNext(), i+1);
        this->parentOf[i] = i;
    }
}

void CubeCollisionModel::setParentOf(index_type childIndex, const Vector3& min, const Vector3& max)
{
    index_type i = parentOf[childIndex];
    elems[i].minBBox = min;
    elems[i].maxBBox = max;
    elems[i].coneAngle = 2*M_PI;
}

void CubeModel::setParentOf(int childIndex, const Vector3& min, const Vector3& max, const Vector3& normal, const SReal angle)
{
    int i = parentOf[childIndex];
    elems[i].minBBox = min;
    elems[i].maxBBox = max;

    elems[i].coneAxis = normal;
    elems[i].coneAngle = angle;
}

void CubeCollisionModel::setLeafCube(index_type cubeIndex, index_type childIndex)
{
    parentOf[childIndex] = cubeIndex;
    this->elems[cubeIndex].children.first=core::CollisionElementIterator(getNext(), childIndex);
    this->elems[cubeIndex].children.second=core::CollisionElementIterator(getNext(), childIndex+1);
    //elems[cubeIndex].minBBox = min;
    //elems[cubeIndex].maxBBox = max;
}

void CubeCollisionModel::setLeafCube(index_type cubeIndex, std::pair<core::CollisionElementIterator,core::CollisionElementIterator> children, const Vector3& min, const Vector3& max)
{
    elems[cubeIndex].minBBox = min;
    elems[cubeIndex].maxBBox = max;
    elems[cubeIndex].children = children;
}

index_type CubeCollisionModel::addCube(Cube subcellsBegin, Cube subcellsEnd)
{
    index_type index = size;

    this->core::CollisionModel::resize(index + 1);
    elems.resize(index + 1);
    
    elems[index].subcells.first = subcellsBegin;
    elems[index].subcells.second = subcellsEnd;
    elems[index].children.first = core::CollisionElementIterator();
    elems[index].children.second = core::CollisionElementIterator();
    updateCube(index);
    return index;
}

void CubeCollisionModel::updateCube(index_type index)
{
    const std::pair<Cube,Cube>& subcells = elems[index].subcells;
    if (subcells.first != subcells.second)
    {
        Cube c = subcells.first;
        Vector3 minBBox = c.minVect();
        Vector3 maxBBox = c.maxVect();

        elems[index].coneAxis = c.getConeAxis();
        elems[index].coneAngle = c.getConeAngle();

        int subCellsNb = 1;

        ++c;
        while(c != subcells.second)
        {
            subCellsNb++;
            const Vector3& cmin = c.minVect();
            const Vector3& cmax = c.maxVect();

            SReal alpha = std::max<SReal>(elems[index].coneAngle, c.getConeAngle());
            if(alpha <= M_PI/2)
            {
                SReal beta = acos(c.getConeAxis() *  elems[index].coneAxis);
                elems[index].coneAxis = (c.getConeAxis() + elems[index].coneAxis).normalized();
                elems[index].coneAngle = beta/2 + alpha;
            }
            else
                elems[index].coneAngle = 2*M_PI;
            
            for (int j=0; j<3; j++)
            {
                if (cmax[j] > maxBBox[j]) maxBBox[j] = cmax[j];
                if (cmin[j] < minBBox[j]) minBBox[j] = cmin[j];
            }
            ++c;
        }
        elems[index].minBBox = minBBox;
        elems[index].maxBBox = maxBBox;
    }
}

void CubeCollisionModel::updateCubes()
{
    for (index_type i=0; i<size; i++)
        updateCube(i);
}

void CubeCollisionModel::draw(const core::visual::VisualParams* vparams)
{
    if (!isActive() || !((getNext()==nullptr)?vparams->displayFlags().getShowCollisionModels():vparams->displayFlags().getShowBoundingCollisionModels())) return;

    int level=0;
    CollisionModel* m = getPrevious();
    float color = 1.0f;
    while (m!=nullptr)
    {
        m = m->getPrevious();
        ++level;
        color *= 0.8f;
    }
    Vec<4,float> c;
    if (isSimulated())
        c=Vec<4,float>(1.0f, 1.0f, 1.0f, color);
    else
        c=Vec<4,float>(1.0f, 1.0f, 1.0f, color);


    std::vector< Vector3 > points;
    std::vector< Vec<4,float> > colours;
    for (index_type i=0; i<size; i++)
    {
        if(elems[i].coneAngle < M_PI/2)
            c[1] = c[2] = 0.5f;
        else
            c[1] = c[2] = 1.0f;

        for(int j=0; j<12; j++)
            colours.push_back(c);

        const Vector3& vmin = elems[i].minBBox;
        const Vector3& vmax = elems[i].maxBBox;

        points.push_back(Vector3(vmin[0], vmin[1], vmin[2]));
        points.push_back(Vector3(vmin[0], vmin[1], vmax[2]));
        points.push_back(Vector3(vmin[0], vmax[1], vmin[2]));
        points.push_back(Vector3(vmin[0], vmax[1], vmax[2]));
        points.push_back(Vector3(vmax[0], vmin[1], vmin[2]));
        points.push_back(Vector3(vmax[0], vmin[1], vmax[2]));
        points.push_back(Vector3(vmax[0], vmax[1], vmin[2]));
        points.push_back(Vector3(vmax[0], vmax[1], vmax[2]));

        points.push_back(Vector3(vmin[0], vmin[1], vmin[2]));
        points.push_back(Vector3(vmin[0], vmax[1], vmin[2]));
        points.push_back(Vector3(vmin[0], vmin[1], vmax[2]));
        points.push_back(Vector3(vmin[0], vmax[1], vmax[2]));
        points.push_back(Vector3(vmax[0], vmin[1], vmin[2]));
        points.push_back(Vector3(vmax[0], vmax[1], vmin[2]));
        points.push_back(Vector3(vmax[0], vmin[1], vmax[2]));
        points.push_back(Vector3(vmax[0], vmax[1], vmax[2]));

        points.push_back(Vector3(vmin[0], vmin[1], vmin[2]));
        points.push_back(Vector3(vmax[0], vmin[1], vmin[2]));
        points.push_back(Vector3(vmin[0], vmax[1], vmin[2]));
        points.push_back(Vector3(vmax[0], vmax[1], vmin[2]));
        points.push_back(Vector3(vmin[0], vmin[1], vmax[2]));
        points.push_back(Vector3(vmax[0], vmin[1], vmax[2]));
        points.push_back(Vector3(vmin[0], vmax[1], vmax[2]));
        points.push_back(Vector3(vmax[0], vmax[1], vmax[2]));
    }

    vparams->drawTool()->drawLines(points, 1, Vec<4,float>(c));


    if (getPrevious()!=nullptr)
        getPrevious()->draw(vparams);
}

std::pair<core::CollisionElementIterator,core::CollisionElementIterator> CubeCollisionModel::getInternalChildren(index_type index) const
{
    return elems[index].subcells;
}

std::pair<core::CollisionElementIterator,core::CollisionElementIterator> CubeCollisionModel::getExternalChildren(index_type index) const
{
    return elems[index].children;
    /*
        core::CollisionElementIterator i1 = elems[index].leaf;
        if (!i1.valid())
        {
            return std::make_pair(core::CollisionElementIterator(),core::CollisionElementIterator());
        }
        else
        {
            core::CollisionElementIterator i2 = i1; ++i2;
            return std::make_pair(i1,i2);
        }
    */
}

bool CubeCollisionModel::isLeaf(index_type index ) const
{
    return elems[index].children.first.valid();
}

void CubeCollisionModel::computeBoundingTree(int maxDepth)
{
//    if(maxDepth <= 0)
//        return;

    dmsg_info() << ">CubeCollisionModel::computeBoundingTree(" << maxDepth << ")";
    std::list<CubeCollisionModel*> levels;
    levels.push_front(createPrevious<CubeCollisionModel>());
    for (int i=0; i<maxDepth; i++)
        levels.push_front(levels.front()->createPrevious<CubeCollisionModel>());
    CubeCollisionModel* root = levels.front();
    //if (isStatic() && root->getPrevious() == nullptr && !root->empty()) return; // No need to recompute BBox if immobile

    if (root->empty() || root->getPrevious() != nullptr)
    {
        // Tree must be reconstructed
        dmsg_info() << "Building Tree with depth " << maxDepth << " from " << size << " elements.";
        // First remove extra levels
        while(root->getPrevious()!=nullptr)
        {
            core::CollisionModel::SPtr m = root->getPrevious();
            root->setPrevious(m->getPrevious());
            if (m->getMaster()) m->getMaster()->removeSlave(m);
            //delete m;
            m.reset();
        }

        // Then clear all existing levels
        {
            for (std::list<CubeCollisionModel*>::iterator it = levels.begin(); it != levels.end(); ++it)
                (*it)->resize(0);
        }

        // Then build root cell
        dmsg_info() << "CubeCollisionModel: add root cube";
        root->addCube(Cube(this,0),Cube(this,size));
        // Construct tree by splitting cells along their biggest dimension
        std::list<CubeCollisionModel*>::iterator it = levels.begin();
        CubeCollisionModel* level = *it;
        ++it;
        int lvl = 0;
        while(it != levels.end())
        {
            dmsg_info() << "CubeCollisionModel: split level " << lvl;
            CubeCollisionModel* clevel = *it;
            clevel->elems.reserve(level->size*2);
            for(Cube cell = Cube(level->begin()); level->end() != cell; ++cell)
            {
                const std::pair<Cube,Cube>& subcells = cell.subcells();
                index_type ncells = subcells.second.getIndex() - subcells.first.getIndex();
                dmsg_info() << "CubeCollisionModel: level " << lvl << " cell " << cell.getIndex() << ": current subcells " << subcells.first.getIndex() << " - " << subcells.second.getIndex();
                if (ncells > 4)
                {
                    // Only split cells with more than 4 childs
                    // Find the biggest dimension
                    int splitAxis;
                    Vector3 l = cell.maxVect()-cell.minVect();
                    index_type middle = subcells.first.getIndex()+(ncells+1)/2;
                    if(l[0]>l[1])
                        if (l[0]>l[2])
                            splitAxis = 0;
                        else
                            splitAxis = 2;
                    else if (l[1]>l[2])
                        splitAxis = 1;
                    else
                        splitAxis = 2;

                    // Separate cells on each side of the median cell
                    CubeSortPredicate sortpred(splitAxis);
                    std::sort(elems.begin() + subcells.first.getIndex(), elems.begin() + subcells.second.getIndex(), sortpred);

                    // Create the two new subcells
                    Cube cmiddle(this, middle);
                    index_type c1 = clevel->addCube(subcells.first, cmiddle);
                    index_type c2 = clevel->addCube(cmiddle, subcells.second);
                    dmsg_info() << "L" << lvl << " cell " << cell.getIndex() << " split along " << (splitAxis == 0 ? 'X' : splitAxis == 1 ? 'Y' : 'Z') << " in cell " << c1 << " size " << middle - subcells.first.getIndex() << " and cell " << c2 << " size " << subcells.second.getIndex() - middle << ".";
                    //level->elems[cell.getIndex()].subcells = std::make_pair(Cube(clevel,c1),Cube(clevel,c2+1));
                    level->elems[cell.getIndex()].subcells.first = Cube(clevel,c1);
                    level->elems[cell.getIndex()].subcells.second = Cube(clevel,c2+1);
                }
            }
            ++it;
            level = clevel;
            ++lvl;
        }
        if (!parentOf.empty())
        {
            // Finally update parentOf to reflect new cell order
            for (std::size_t i=0; i<size; i++)
                parentOf[elems[i].children.first.getIndex()] = i;
        }
    }
    else
    {
        // Simply update the existing tree, starting from the bottom
        int lvl = 0;
        for (std::list<CubeCollisionModel*>::reverse_iterator it = levels.rbegin(); it != levels.rend(); ++it)
        {
            dmsg_info() << "CubeCollisionModel: update level " << lvl;
            (*it)->updateCubes();
            ++lvl;
        }
    }
    dmsg_info() << "<CubeCollisionModel::computeBoundingTree(" << maxDepth << ")";
}

} // namespace collision

} // namespace component

} // namespace sofa

