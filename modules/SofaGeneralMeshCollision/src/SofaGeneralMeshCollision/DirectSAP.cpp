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
#include <SofaGeneralMeshCollision/DirectSAP.h>
#include <sofa/core/visual/VisualParams.h>
#include <SofaBaseCollision/CapsuleModel.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/collision/Intersection.h>
#include <sofa/helper/AdvancedTimer.h>

#include <deque>
#include <unordered_map>


namespace sofa::component::collision
{

inline void DSAPBox::update(int axis, double alarmDist)
{
    min->value = (cube.minVect())[axis] - alarmDist;
    max->value = (cube.maxVect())[axis] + alarmDist;
}

double DSAPBox::squaredDistance(const DSAPBox & other) const
{
    double dist2 = 0;

    for (int axis = 0; axis < 3; ++axis)
    {
        dist2 += squaredDistance(other, axis);
    }

    return dist2;
}

inline double DSAPBox::squaredDistance(const DSAPBox & other, int axis) const
{
    const defaulttype::Vector3 & min0 = this->cube.minVect();
    const defaulttype::Vector3 & max0 = this->cube.maxVect();
    const defaulttype::Vector3 & min1 = other.cube.minVect();
    const defaulttype::Vector3 & max1 = other.cube.maxVect();

    if(min0[axis] > max1[axis])
    {
        return std::pow(min0[axis] - max1[axis], 2);
    }

    if(min1[axis] > max0[axis])
    {
        return std::pow(min1[axis] - max0[axis], 2);
    }

    return 0;
}

DirectSAP::DirectSAP()
    : d_draw(initData(&d_draw, false, "draw", "enable/disable display of results"))
    , d_showOnlyInvestigatedBoxes(initData(&d_showOnlyInvestigatedBoxes, true, "showOnlyInvestigatedBoxes", "Show only boxes which will be sent to narrow phase"))
    , d_nbPairs(initData(&d_nbPairs, 0, "nbPairs", "number of pairs of elements sent to narrow phase"))
    , d_box(initData(&d_box, "box", "if not empty, objects that do not intersect this bounding-box will be ignored"))
    , m_currentAxis(0)
    , m_alarmDist(0)
    , m_alarmDist_d2(0)
    , m_sq_alarmDist(0)
{
    d_nbPairs.setReadOnly(true);
}

void DirectSAP::init()
{
    reinit();
}


void DirectSAP::reinit()
{
    if (d_box.getValue()[0][0] >= d_box.getValue()[1][0])
    {
        m_boxModel.reset();
    }
    else
    {
        if (!m_boxModel) m_boxModel = sofa::core::objectmodel::New<CubeCollisionModel>();
        m_boxModel->resize(1);
        m_boxModel->setParentOf(0, d_box.getValue()[0], d_box.getValue()[1]);
    }
}

void DirectSAP::reset()
{
    m_endPointContainer.clear();
    m_boxes.clear();
    m_isBoxInvestigated.clear();
    m_sortedEndPoints.clear();
    m_collisionModels.clear();
}

inline bool DirectSAP::added(core::CollisionModel *cm) const
{
    assert(cm != nullptr);
    return m_collisionModels.count(cm->getLast()) >= 1;
}

inline void DirectSAP::add(core::CollisionModel *cm)
{
    assert(cm != nullptr);
    m_collisionModels.insert(cm->getLast());
    m_newCollisionModels.push_back(cm->getLast());
}

void DirectSAP::endBroadPhase()
{
    BroadPhaseDetection::endBroadPhase();

    if (m_newCollisionModels.empty())
        return;

    createBoxesFromCollisionModels();
    m_newCollisionModels.clear(); //createBoxesFromCollisionModels will be called again iff new collision models are added
}

void DirectSAP::createBoxesFromCollisionModels()
{
    sofa::helper::vector<CubeCollisionModel*> cube_models;
    cube_models.reserve(m_newCollisionModels.size());

    int totalNbElements = 0;
    for (auto* newCM : m_newCollisionModels)
    {
        if (newCM != nullptr)
        {
            totalNbElements += newCM->getSize();
            cube_models.push_back(dynamic_cast<CubeCollisionModel*>(newCM->getPrevious()));
        }
    }

    m_boxes.reserve(m_boxes.size() + totalNbElements);

    int cur_boxID = static_cast<int>(m_boxes.size());

    for (auto* cm : cube_models)
    {
        if (cm != nullptr)
        {
            for (Size j = 0; j < cm->getSize(); ++j)
            {
                m_endPointContainer.emplace_back();
                EndPoint* min = &m_endPointContainer.back();

                m_endPointContainer.emplace_back();
                EndPoint* max = &m_endPointContainer.back();

                min->setBoxID(cur_boxID);
                max->setBoxID(cur_boxID);
                max->setMax();

                m_sortedEndPoints.push_back(min);
                m_sortedEndPoints.push_back(max);

                m_boxes.emplace_back(Cube(cm, j), min, max);
                ++cur_boxID;
            }
        }
    }

    m_isBoxInvestigated.resize(m_boxes.size(), false);
    m_boxData.resize(m_boxes.size());
}

void DirectSAP::addCollisionModel(core::CollisionModel *cm)
{
    assert(cm != nullptr);
    if(!added(cm))
        add(cm);
}

int DirectSAP::greatestVarianceAxis() const
{
    defaulttype::Vector3 variance;//variances for each axis
    defaulttype::Vector3 mean;//means for each axis

    //computing the mean value of end points on each axis
    for (const auto& dsapBox : m_boxes)
    {
        mean += dsapBox.cube.minVect();
        mean += dsapBox.cube.maxVect();
    }

    const auto nbBoxes = m_boxes.size();
    if (nbBoxes > 0)
    {
        mean[0] /= 2. * static_cast<double>(nbBoxes);
        mean[1] /= 2. * static_cast<double>(nbBoxes);
        mean[2] /= 2. * static_cast<double>(nbBoxes);
    }

    //computing the variance of end points on each axis
    for (const auto& dsapBox : m_boxes)
    {
        const defaulttype::Vector3 & min = dsapBox.cube.minVect();
        const defaulttype::Vector3 & max = dsapBox.cube.maxVect();

        for (unsigned int j = 0 ; j < 3; ++j)
        {
            variance[j] += std::pow(min[j] - mean[j], 2);
            variance[j] += std::pow(max[j] - mean[j], 2);
        }
    }

    if(variance[0] >= variance[1] && variance[0] >= variance[2])
        return 0;
    if(variance[1] >= variance[2])
        return 1;
    return 2;
}

void DirectSAP::update()
{
    m_currentAxis = greatestVarianceAxis();
    for (auto& dsapBox : m_boxes)
    {
        dsapBox.update(m_currentAxis, m_alarmDist_d2);
    }

    //used only for drawing
    m_isBoxInvestigated.resize(m_boxes.size(), false);
    std::fill(m_isBoxInvestigated.begin(), m_isBoxInvestigated.end(), false);
}

bool DirectSAP::isSquaredDistanceLessThan(const DSAPBox &a, const DSAPBox &b, double threshold)
{
    double dist2 = 0.;

    for (int axis = 0; axis < 3; ++axis)
    {
        dist2 += a.squaredDistance(b, axis);
        if (dist2 > threshold)
        {
            return false;
        }
    }

    return true;
}

void DirectSAP::cacheData()
{
    sofa::helper::ScopedAdvancedTimer scopeTimer("Direct SAP cache");
    unsigned int i{ 0 };
    for (const auto& box : m_boxes)
    {
        auto* collisionModel = box.cube.getCollisionModel()->getLast();

        auto& data = m_boxData[i++];
        data.collisionModel = collisionModel;
        data.context = collisionModel->getContext();
        data.doesBoxSelfCollide = collisionModel->getSelfCollision();
        data.isBoxSimulated = collisionModel->isSimulated();
        data.collisionElementIterator = box.cube.getExternalChildren().first;
    }
}

void DirectSAP::sortEndPoints()
{
    sofa::helper::ScopedAdvancedTimer scopeTimer("Direct SAP sort");
    std::sort(m_sortedEndPoints.begin(), m_sortedEndPoints.end(), CompPEndPoint());
}

void DirectSAP::beginNarrowPhase()
{
    assert(intersectionMethod != nullptr);

    core::collision::NarrowPhaseDetection::beginNarrowPhase();

    m_alarmDist = getIntersectionMethod()->getAlarmDistance();
    m_sq_alarmDist = m_alarmDist * m_alarmDist;
    m_alarmDist_d2 = m_alarmDist/2.0;

    update();
    cacheData();
    sortEndPoints();
    narrowCollisionDetectionFromSortedEndPoints();
}

void DirectSAP::narrowCollisionDetectionFromSortedEndPoints()
{
    sofa::helper::ScopedAdvancedTimer scopeTimer("Direct SAP intersection");
    int nbInvestigatedPairs{ 0 };

    std::list<int> activeBoxes;//active boxes are the one that we encoutered only their min (end point), so if there are two boxes b0 and b1,
                                 //if we encounter b1_min as b0_min < b1_min, on the current axis, the two boxes intersect :  b0_min--------------------b0_max
                                 //                                                                                                      b1_min---------------------b1_max
                                 //once we encouter b0_max, b0 will not intersect with nothing (trivial), so we delete it from active_boxes.
                                 //so the rule is : -every time we encounter a box min end point, we check if it is overlapping with other active_boxes and add the owner (a box) of this end point to
                                 //                  the active boxes.
                                 //                 -every time we encounter a max end point of a box, we are sure that we encountered min end point of a box because _end_points is sorted,
                                 //                  so, we delete the owner box, of this max end point from the active boxes

    // Iterators to activeBoxes are stored in a map for a fast access from a box id
    std::unordered_map<int, decltype(activeBoxes)::const_iterator> activeBoxesIt;

    for (auto* endPoint : m_sortedEndPoints)
    {
        assert(endPoint != nullptr);
        if (endPoint->max())
        {
            const auto foundIt = activeBoxesIt.find(endPoint->boxID()); // complexity: Constant on average, worst case linear in the size of the container
            if (foundIt != activeBoxesIt.end())
            {
                //erase the box with id endPoint->boxID() from the list of active boxes
                //the iterator is found from a map
                //with std::list, erasing an element does not invalidate the other iterators
                activeBoxes.erase(foundIt->second);// complexity: Constant
            }
        }
        else //we encounter a min possible intersection between it and active_boxes
        {
            const int boxId0 = endPoint->boxID();
            const DSAPBox& box0 = m_boxes[boxId0];
            const BoxData& data0 = m_boxData[boxId0];

            core::CollisionModel *cm0 = data0.collisionModel;
            auto collisionElement0 = data0.collisionElementIterator;

            for (int boxId1 : activeBoxes)
            {
                const BoxData& data1 = m_boxData[boxId1];

                if (!isPairFiltered(data0, data1, box0, boxId1))
                {
                    core::CollisionModel *cm1 = data1.collisionModel;

                    bool swapModels = false;
                    core::collision::ElementIntersector* finalintersector = intersectionMethod->findIntersector(cm0, cm1, swapModels);//find the method for the finnest CollisionModels

                    if (!swapModels && cm0->getClass() == cm1->getClass() && cm0 > cm1)//we do that to have only pair (p1,p2) without having (p2,p1)
                        swapModels = true;

                    if (finalintersector != nullptr)
                    {
                        auto collisionElement1 = data1.collisionElementIterator;

                        auto swappableCm0 = cm0;
                        auto swappableCollisionElement0 = collisionElement0;

                        if (swapModels)
                        {
                            std::swap(swappableCm0, cm1);
                            std::swap(swappableCollisionElement0, collisionElement1);
                        }

                        narrowCollisionDetectionForPair(finalintersector, swappableCm0, cm1, swappableCollisionElement0, collisionElement1);

                        //used only for drawing
                        m_isBoxInvestigated[boxId0] = true;
                        m_isBoxInvestigated[boxId1] = true;

                        ++nbInvestigatedPairs;
                    }
                }

            }
            activeBoxes.push_back(boxId0);// complexity: Constant
            auto last = activeBoxes.end();
            --last;//iterator the last element of the list
            activeBoxesIt.insert({boxId0, std::move(last)});// complexity: Average case: O(1), worst case O(size())
        }
    }

    d_nbPairs.setValue(nbInvestigatedPairs);
    sofa::helper::AdvancedTimer::valSet("Direct SAP pairs", nbInvestigatedPairs);
}

bool DirectSAP::isPairFiltered(const BoxData& data0, const BoxData& data1,
                               const DSAPBox& box0, int boxId1) const
{
    if (data0.isBoxSimulated || data1.isBoxSimulated) //is any of the object simulated?
    {
        // do the models belong to the same object? Can both object collide?
        if ((data0.context != data1.context) || data0.doesBoxSelfCollide)
        {
            if (isSquaredDistanceLessThan(box0, m_boxes[boxId1], m_sq_alarmDist))
            {
                return false;
            }
        }
    }
    return true;
}

void DirectSAP::narrowCollisionDetectionForPair(core::collision::ElementIntersector* intersector,
                                                core::CollisionModel *collisionModel0,
                                                core::CollisionModel *collisionModel1,
                                                core::CollisionElementIterator collisionModelIterator0,
                                                core::CollisionElementIterator collisionModelIterator1)
{
    sofa::core::collision::DetectionOutputVector*& outputs = this->getDetectionOutputs(collisionModel0, collisionModel1);
    intersector->beginIntersect(collisionModel0, collisionModel1, outputs);//creates outputs if null
    intersector->intersect(collisionModelIterator0, collisionModelIterator1, outputs);
}

void DirectSAP::draw(const core::visual::VisualParams* vparams)
{
    if (!d_draw.getValue())
        return;

    vparams->drawTool()->saveLastState();
    vparams->drawTool()->disableLighting();

    std::vector<sofa::helper::types::RGBAColor> colors;

    vparams->drawTool()->setPolygonMode(0, true);
    std::vector<sofa::defaulttype::Vector3> vertices;

    unsigned int boxId{ 0 };
    for (const auto& dsapBox : m_boxes)
    {
        const bool isBoxInvestigated = m_isBoxInvestigated[boxId++];
        if (d_showOnlyInvestigatedBoxes.getValue() && !isBoxInvestigated) continue;

        const auto& minCorner = dsapBox.cube.minVect();
        const auto& maxCorner = dsapBox.cube.maxVect();

        vertices.emplace_back(minCorner[0], minCorner[1], minCorner[2]);
        vertices.emplace_back(maxCorner[0], minCorner[1], minCorner[2]);

        vertices.emplace_back(minCorner[0], minCorner[1], minCorner[2]);
        vertices.emplace_back(minCorner[0], maxCorner[1], minCorner[2]);

        vertices.emplace_back(minCorner[0], minCorner[1], minCorner[2]);
        vertices.emplace_back(minCorner[0], minCorner[1], maxCorner[2]);

        vertices.emplace_back(minCorner[0], minCorner[1], maxCorner[2]);
        vertices.emplace_back(maxCorner[0], minCorner[1], maxCorner[2]);

        vertices.emplace_back(minCorner[0], maxCorner[1], minCorner[2]);
        vertices.emplace_back(maxCorner[0], maxCorner[1], minCorner[2]);

        vertices.emplace_back(maxCorner[0], minCorner[1], minCorner[2]);
        vertices.emplace_back(maxCorner[0], maxCorner[1], minCorner[2]);

        vertices.emplace_back(minCorner[0], maxCorner[1], minCorner[2]);
        vertices.emplace_back(minCorner[0], maxCorner[1], maxCorner[2]);

        vertices.emplace_back(maxCorner[0], maxCorner[1], minCorner[2]);
        vertices.emplace_back(maxCorner[0], maxCorner[1], maxCorner[2]);

        vertices.emplace_back(maxCorner[0], minCorner[1], minCorner[2]);
        vertices.emplace_back(maxCorner[0], minCorner[1], maxCorner[2]);

        vertices.emplace_back(minCorner[0], maxCorner[1], maxCorner[2]);
        vertices.emplace_back(maxCorner[0], maxCorner[1], maxCorner[2]);

        vertices.emplace_back(maxCorner[0], minCorner[1], maxCorner[2]);
        vertices.emplace_back(maxCorner[0], maxCorner[1], maxCorner[2]);

        vertices.emplace_back(minCorner[0], minCorner[1], maxCorner[2]);
        vertices.emplace_back(minCorner[0], maxCorner[1], maxCorner[2]);

        if (isBoxInvestigated)
        {
            for (unsigned int i = 0; i < 12; ++i)
            {
                colors.emplace_back(1.0, 0.0, 0.0, 1.0);
            }
        }
        else
        {
            for (unsigned int i = 0; i < 12; ++i)
            {
                colors.emplace_back(0.0, 0.0, 1.0, 1.0);
            }
        }
    }

    vparams->drawTool()->drawLines(vertices, 3, colors);
    vparams->drawTool()->restoreLastState();
}

inline void DSAPBox::show()const
{
    msg_info("DSAPBox") <<"MIN "<<cube.minVect()<< msgendl
                        <<"MAX "<<cube.maxVect() ;
}

using namespace sofa::defaulttype;
using namespace collision;

int DirectSAPClass = core::RegisterObject("Collision detection using sweep and prune")
        .add< DirectSAP >()
        ;

} // namespace sofa::component::collision

