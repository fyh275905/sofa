/******************************************************************************
 *       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 3      *
 *                (c) 2006-2008 MGH, INRIA, USTL, UJF, CNRS                    *
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation; either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program; if not, write to the Free Software Foundation, Inc., 51  *
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
 *******************************************************************************
 *                            SOFA :: Applications                             *
 *                                                                             *
 * Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
 * H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
 * M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
 *                                                                             *
 * Contact information: contact@sofa-framework.org                             *
 ******************************************************************************/
//
// C++ Implementation: BglGraphManager
//
// Description:
//
//
// Author: Francois Faure in The SOFA team </www.sofa-framework.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef BglGraphManager_h
#define BglGraphManager_h

//#include "BglNode.h"
#include <sofa/simulation/common/Node.h>
//Boost Headers
#include <boost/graph/adjacency_list.hpp>

#include <sofa/core/componentmodel/collision/Pipeline.h>

#include <sofa/helper/vector.h>


namespace sofa
{
namespace simulation
{
namespace bgl
{
class BglNode;
using sofa::helper::vector;
using sofa::simulation::bgl::BglNode;
using sofa::simulation::Node;

class BglGraphManager
{
public:

    /** @name Hierarchical graph
        This graph (hgraph) reflects the sofa mechanical mapping hierarchy.
        The vertices contains sofa simulation nodes, while the edges contain nothing.
        The (mechanical) mappings are not in the edges because in future work we want to allow a single mapping to read input from several parents. Thus, the mappings remain associated with the Nodes as usual.

        A dual graph (rraph) is used for convenience.
        It is the same as the hierarchical graph, but its edges are in the opposite directions.
        (BTW, do we really need it ?)
    */
    /// @{
    // Vertices
    struct bglnode_t
    {
        typedef boost::vertex_property_tag kind;
    };
    typedef boost::property<bglnode_t, Node*> VertexProperty;

    // Graph
    typedef ::boost::adjacency_list < ::boost::vecS, ::boost::vecS, ::boost::bidirectionalS, VertexProperty > Hgraph; // bidirectional = oriented + each node stores its in-edges, allowing to easily detect roots (root = no in-edges)
    typedef Hgraph::vertex_descriptor Hvertex;
    typedef Hgraph::edge_descriptor Hedge;
    typedef std::pair<Hgraph::vertex_iterator,Hgraph::vertex_iterator> Hvpair;

    typedef vector<Hvertex> HvertexVector;
    typedef ::boost::property_map<Hgraph, bglnode_t>::type  H_vertex_node_map; // hvertex->sofa node
    /*    typedef ::boost::property_map<const Hgraph, bglnode_t>::type  H_vertex_node_const_map; // hvertex->sofa node*/
    typedef std::map<Node*, Hvertex> H_node_vertex_map;                    //  sofa node->hvertex


    //-----------------------------------------------------------------------------------
    // reverse graph: hgraph with vertices in the opposite direction.
    typedef boost::property<bglnode_t, Node*> RVertexProperty;
    // Graph
    typedef ::boost::adjacency_list < ::boost::vecS, ::boost::vecS, ::boost::bidirectionalS, RVertexProperty > Rgraph;
    typedef Rgraph::vertex_descriptor Rvertex;
    typedef Rgraph::edge_descriptor Redge;
    typedef std::pair<Rgraph::vertex_iterator,Rgraph::vertex_iterator> Rvpair;
    typedef boost::property_map<Rgraph, bglnode_t>::type  R_vertex_node_map; // rvertex->sofa node
    typedef std::map<Node*, Rvertex> R_node_vertex_map;                    //  sofa node->rvertex


    /** @name interaction graph
        This auxiliary graph represents the interactions between the simulated objects.
        It is created temporarily and used to build the interaction groups.
        Contrary to the hierarchical graph, it is not oriented.
    */
    ///@{
    // same Vertices as Hgraph
    // Edges
    struct interaction_t
    {
        typedef ::boost::edge_property_tag kind;
    };
    typedef boost::property<interaction_t, core::objectmodel::BaseObject*> IEdgeProperty;
    // Graph
    typedef ::boost::adjacency_list < ::boost::vecS, ::boost::vecS, ::boost::undirectedS, VertexProperty, IEdgeProperty > Igraph;
    typedef Igraph::vertex_descriptor Ivertex;
    typedef Igraph::edge_descriptor Iedge;
    typedef std::pair<Igraph::vertex_iterator,Igraph::vertex_iterator> Ivpair;

    typedef ::boost::property_map<Igraph, bglnode_t>::type  I_vertex_node_map;           // ivertex->sofa node
    typedef std::map<Node*, Ivertex>     I_node_vertex_map;                              // sofa node -> ivertex
    typedef ::boost::property_map<Igraph, interaction_t>::type  I_edge_interaction_map;  // iedge->sofa interaction force field
    typedef std::map<core::objectmodel::BaseObject*, Iedge> I_interaction_edge_map;              // sofa interaction force field->iedge
    ///@}

    /** @name interactions
        Interactions can be defined at any level of the hgraph.
        Based on a list of interactions, we build interaction groups : lists of objects and the interactions between them.
        Each object is represented by its root in the hierarchical graph.
    */
    ///@{
    struct InteractionData
    {
        Node *n1;
        Node *n2;
        core::objectmodel::BaseObject* iff;
        InteractionData( Node *r1, Node *r2, core::objectmodel::BaseObject* i ) : n1(r1), n2(r2), iff(i) {}
    };

    struct Interaction
    {
        Hvertex v1;
        Hvertex v2;
        core::objectmodel::BaseObject* iff;
        Interaction( Hvertex r1, Hvertex r2, core::objectmodel::BaseObject* i ) : v1(r1), v2(r2), iff(i) {}
    };
    typedef vector<Interaction> Interactions;
    typedef vector<InteractionData> InteractionsData;
    typedef std::pair< HvertexVector, vector<Interaction> > InteractionGroup; ///< maximum set of nodes which interact together, along with the interactions between them
    typedef vector<InteractionGroup> InteractionGroups;







public:

    BglGraphManager();

    BglNode *getMasterNode() {return masterNode;};
    Hvertex getMasterVertex() {return masterVertex;};

    bool isNodeCreated(const Node *n) {return std::find(nodeToAdd.begin(),nodeToAdd.end(),n) != nodeToAdd.end();}
    bool isNodeDeleted(const Hvertex u) {return std::find(vertexToDelete.begin(),vertexToDelete.end(),u) != vertexToDelete.end();}

    /// Delayed Creation of a graph node and attach a new Node to it, then return the Node
    Node* newNode(const std::string& name="");

    /// Instant Creation of a graph node and attach a new Node to it, then return the Node
    Node* newNodeNow(const std::string& name="");


    /// Insert a node previously created, into the graph
    void insertNewNode(Node *n);

    // Node dynamical access
    /// Add a node to as the child of another
    void addNode(BglNode* parent, BglNode* child);

    /// Delete a graph node and all the edges, and entries in map
    void deleteNode( Node* n);

    /// Add an interaction
    void addInteraction( Node* n1, Node* n2, core::objectmodel::BaseObject* );

    /// Add an interaction
    void addInteractionNow( InteractionData &i );

    void setCollisionPipeline(core::componentmodel::collision::Pipeline* p) {collisionPipeline=p;};

    void addSolver(core::objectmodel::BaseObject* s,Node* n);
    void setSolverOfCollisionGroup(Node* solverNode, Node* solverOfCollisionGroup);

    /// Remove an interaction
    void removeInteraction( core::objectmodel::BaseObject* );


    /// depth search in the whole scene
    void dfs( Visitor& );
    /// depth visit starting from the given vertex
    void dfv( Hvertex, Visitor& );

    Node* getNodeFromHvertex(Hvertex h);
    Node* getNodeFromRvertex(Rvertex r);

    std::set< Node*> &getSolverNode() {return nodeSolvers;}

    void  removeVertex(Hvertex p);
    void  addEdge(Hvertex p, Hvertex c);
    Hedge addHedge(Hvertex parent, Hvertex child);
    Redge addRedge(Rvertex parent, Rvertex child);
    void  removeEdge(Hvertex p, Hvertex c);
    void  removeHedge(Hvertex parent, Hvertex child);
    void  removeRedge(Rvertex parent, Rvertex child);

    /// Delete the hgraph node and rgraph node and all the edges, and entries in map
    void  deleteVertex( Hvertex v);
    /// Delete the hgraph node and all the edges, and entries in map
    void deleteHvertex( Hvertex n);
    /// Delete the rgraph node and all the edges, and entries in map
    void deleteRvertex( Rvertex n);

    void  clearVertex(Hvertex v);


    //Dynamic creation of edges
    void addEdge(Node *p, Node*c);
    void removeEdge( Node* p, Node* c );

    /// Update the graph with all the operation stored in memory: add/delete node, add interactions...
    void update();

    void reset();
    void clear();
    /// @}

    Rvertex convertHvertex2Rvertex(Hvertex v);
    Hvertex convertRvertex2Hvertex(Rvertex v);


    bool isVisualRoot(Node* n);

    bool isCollisionRoot(Node* n);

    bool hasCollisionPipeline() {return collisionPipeline != NULL;}
    /** Compute the interaction graph and the connected components, based on interactions and hroots
     */
    void computeInteractionGraphAndConnectedComponents();

    /** Determine if we have to recompute the interaction graph
     */
    bool needToComputeInteractions();

    /** Compute the Roots of the graphs
     */
    void computeRoots();

    /// Perform the collision detection
    void collisionStep(Node* root, double dt=0.0);
    /// Animate all the nodes depth-first
    void mechanicalStep(Node* root, double dt=0.0);

    void insertHierarchicalGraph();
    void insertVisualGraph();
    void insertCollisionGraph();

    void clearMasterVertex() {clearVertex(masterVertex);}



    Hgraph hgraph;             ///< the directed acyclic graph representing system dependencies (edges correspond to mappings)
    Rgraph rgraph;             ///< The reverse graph

    H_vertex_node_map h_vertex_node_map;           ///< hvertex->sofa node
    R_vertex_node_map r_vertex_node_map;  ///< rvertex->sofa node

    H_node_vertex_map h_node_vertex_map;     ///< sofa node->hvertex
    R_node_vertex_map r_node_vertex_map;     ///< sofa node->rvertex
    bool hasCollisionGroupManager;
protected:


    BglNode* masterNode;
    Hvertex masterVertex;

    core::componentmodel::collision::Pipeline* collisionPipeline;


    HvertexVector hroots;           ///< the roots of the forest
    HvertexVector visualroots;      ///< the roots of the visual graph
    HvertexVector collisionroots;   ///< the roots of the collision graph
    //H_vertex_node_const_map h_vertex_node_const_map;           ///< hvertex->sofa node


    Igraph igraph;                           ///< the interaction graph
    I_vertex_node_map      i_vertex_node_map;
    I_node_vertex_map      i_node_vertex_map;
    I_edge_interaction_map i_edge_interaction_map;           ///< iedge->sofa interaction force field
    I_interaction_edge_map i_interaction_edge_map;                       ///< sofa interaction force field->iedge


    Interactions interactions;            ///< interactions between nodes at at any hierarchical levels
    Interactions previousInteractions;    ///< interactions between nodes at at any hierarchical levels at the previous time step
    InteractionGroups interactionGroups;  ///< all the objects and interactions, in independent groups which can be processed separately

    /// Make the correspondance between a node in the mechanical graph and the solver nodes.
    std::map< Node*, Node*> solver_colisionGroup_map;
    /// Make the correspondance between a node in the mechanical graph and the solver nodes.
    std::set< Node*> nodeSolvers;
    std::set< Node*> nodeGroupSolvers;



    std::set   < Hvertex >                     vertexToDelete;
    std::set   < Node*   >                     externalDelete;
    std::vector< Node*   >                     nodeToAdd;
    std::set< std::pair<Node*,Node*> >         edgeToAdd;
    std::vector< InteractionData >             interactionToAdd;
};
}
}
}
#endif
