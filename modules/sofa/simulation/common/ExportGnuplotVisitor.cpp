#include <sofa/simulation/common/ExportGnuplotVisitor.h>
#include <sofa/simulation/common/Node.h>
#include <sofa/simulation/tree/Simulation.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace simulation
{

simulation::Visitor::Result InitGnuplotVisitor::processNodeTopDown(simulation::Node* node)
{
    if (node->mechanicalState != NULL )
    {
        node->mechanicalState->initGnuplot(tree::getSimulation()->gnuplotDirectory.getValue());
    }
    if (node->mass != NULL )
    {
        node->mass->initGnuplot(tree::getSimulation()->gnuplotDirectory.getValue());
    }
    return RESULT_CONTINUE;
}

ExportGnuplotVisitor::ExportGnuplotVisitor( double time )
    : m_time(time)
{}

simulation::Visitor::Result ExportGnuplotVisitor::processNodeTopDown(simulation::Node* node)
{
    if (node->mechanicalState != NULL )
    {
        node->mechanicalState->exportGnuplot(m_time);
    }
    if (node->mass!= NULL )
    {
        node->mass->exportGnuplot(m_time);
    }
    return RESULT_CONTINUE;
}


} // namespace simulation

} // namespace sofa

