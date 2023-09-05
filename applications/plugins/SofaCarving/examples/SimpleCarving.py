import Sofa
import Sofa.Simulation

def main():
    root = Sofa.Core.Node("root")
    createScene(root)
    Sofa.Simulation.init(root)

    Sofa.Gui.GUIManager.Init("myscene", "qglviewer")
    Sofa.Gui.GUIManager.createGUI(root, __file__)
    Sofa.Gui.GUIManager.SetDimension(1080, 1080)
    Sofa.Gui.GUIManager.MainLoop(root)
    Sofa.Gui.GUIManager.closeGUI()

def createScene(root):
    
    root.gravity=[0, 0, 0]
    root.dt=0.05
    root.showBoundingTree = 0

    # Load required plugins
    plugins = root.addChild('Plugins')
    plugins.addObject('RequiredPlugin', name="Mechanical", pluginName="Sofa.Component.AnimationLoop Sofa.Component.LinearSolver.Direct Sofa.Component.ODESolver.Backward Sofa.Component.SolidMechanics.Spring Sofa.Component.StateContainer") 
    plugins.addObject('RequiredPlugin', name="Collision", pluginName="Sofa.Component.Collision.Detection.Algorithm Sofa.Component.Collision.Detection.Intersection Sofa.Component.Collision.Geometry Sofa.Component.Collision.Response.Contact") 
    plugins.addObject('RequiredPlugin', name="Constraint", pluginName="Sofa.Component.Constraint.Lagrangian.Correction Sofa.Component.Constraint.Lagrangian.Solver Sofa.Component.Constraint.Projective") 
    plugins.addObject('RequiredPlugin', name="Topology", pluginName="Sofa.Component.Topology.Container.Constant Sofa.Component.Topology.Container.Dynamic Sofa.Component.Topology.Container.Grid") 
    plugins.addObject('RequiredPlugin', name="Visual", pluginName="Sofa.Component.Visual Sofa.GL.Component.Rendering3D") 

    root.addObject('VisualStyle',displayFlags="showVisualModels")

    # Add main scene pipeline components
    root.addObject('DefaultVisualManagerLoop')
    root.addObject('DefaultAnimationLoop')
    root.addObject('CollisionPipeline', depth=6, verbose=False, draw=False)
    root.addObject('BruteForceBroadPhase')
    root.addObject('BVHNarrowPhase')    
    root.addObject('LocalMinDistance', name="localmindistance", alarmDistance=0.1, contactDistance=0.01, angleCone=0.8, coneFactor=0.8)
    root.addObject('DefaultContactManager', response="FrictionContactConstraint")
    root.addObject('CarvingManager',active=True, carvingDistance=-0.01)
  
     # Add Volume mechanical object to be carved
    TT = root.addChild('TetraVolume')
    
    TT.addObject('EulerImplicitSolver',name="cg_odesolver", printLog=False, rayleighStiffness=0.1, rayleighMass=0.1)
    TT.addObject('CGLinearSolver', name="linear solver", iterations=25, tolerance=1.0e-9, threshold=1.0e-9)
    TT.addObject('MeshGmshLoader', name="loader", filename="mesh/liver.msh")
    
    TT.addObject('MechanicalObject',template="Vec3d", name="Volume", src="@loader")
    
    TT.addObject('TetrahedronSetTopologyContainer', name="topo", src="@loader")
    TT.addObject('TetrahedronSetTopologyModifier', name="topoMod")
    TT.addObject('TetrahedronSetGeometryAlgorithms', template="Vec3d", name="GeomAlgo")
    
    TT.addObject('DiagonalMass', massDensity=0.5)
    TT.addObject('FixedConstraint', indices=[1, 3, 50])
    TT.addObject('TetrahedralCorotationalFEMForceField', name="CFEM", youngModulus=160, poissonRatio=0.3, method="large")

    # Add corresponding surface topology
    T = TT.addChild('TriangleSurface')    
    T.addObject('TriangleSetTopologyContainer', name="Container")
    T.addObject('TriangleSetTopologyModifier', name="Modifier")
    T.addObject('TriangleSetGeometryAlgorithms', template="Vec3d", name="GeomAlgo")
    T.addObject('Tetra2TriangleTopologicalMapping', input="@../topo", output="@Container")
    
    T.addObject('TriangleCollisionModel', tags="CarvingSurface")

    Visu = T.addChild('VisualModel')
    Visu.addObject('OglModel', name="Visual", material="Default Diffuse 1 0 1 0 1 Ambient 0 1 1 1 1 Specular 1 1 1 0 1 Emissive 0 1 1 0 1 Shininess 1 100")
    Visu.addObject('IdentityMapping', input="@Volume", output="@Visual")


    # Add instrument object
    Instrument = root.addChild('Instrument')
    Instrument.addObject('EulerImplicitSolver', name="cg_odesolver")
    Instrument.addObject('CGLinearSolver', name="linear solver", iterations=25, tolerance=1.0e-9, threshold=1.0e-9)
    Instrument.addObject('MechanicalObject', template="Rigid3d", name="instrumentState", rotation=[90, 45, 0], translation=[0, 0, 1])
    Instrument.addObject('UniformMass', template="Rigid3d", name="mass", totalMass=5.0)
    
    visuIns = Instrument.addChild('VisualModel')
    visuIns.addObject('MeshOBJLoader', name="meshLoader_0", filename="mesh/dental_instrument_light.obj", translation=[-0.412256, -0.067639, 3.35], rotation=[180, 0, 150], handleSeams=True)
    visuIns.addObject('OglModel',template="Vec3d", name="InstrumentVisualModel", src="@meshLoader_0", material="Default Diffuse 1 1 0.2 0.2 1 Ambient 1 0.2 0.04 0.04 1 Specular 0 1 0.2 0.2 1 Emissive 0 1 0.2 0.2 1 Shininess 0 45")
    visuIns.addObject('RigidMapping', name="mecha mapping", input="@instrumentState", output="@InstrumentVisualModel")

    colIns = Instrument.addChild('CollisionModel')
    colIns.addObject('MechanicalObject', template="Vec3d", name="Particle", position=[-0.2, -0.2, -0.2])
    colIns.addObject('SphereCollisionModel', name="ParticleModel", radius=0.2, tags="CarvingTool")
    colIns.addObject('RigidMapping', name="mecha mapping", input="@instrumentState", output="@Particle")

    return root


#Function used only if this script is called from a python environment
if __name__ == '__main__':
    main()
