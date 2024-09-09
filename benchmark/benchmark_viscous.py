import sys
import os
import Sofa

dirname = '/home/alexandre/workspace/sofa/src/sofa/benchmark'
def createScene(rootNode, dt=0.01, m=1, alpha=1, g=1):

    # rootNode
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Collision.Detection.Algorithm')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Collision.Detection.Intersection')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Collision.Geometry')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Collision.Response.Contact')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.IO.Mesh')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.LinearSolver.Iterative')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Mapping.Linear')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Mass')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.ODESolver.Backward')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.SolidMechanics.Spring')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.StateContainer')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Topology.Container.Constant')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Topology.Container.Grid')
    rootNode.addObject('RequiredPlugin', name='Sofa.GL.Component.Rendering3D')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.AnimationLoop')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Constraint.Lagrangian.Correction')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.LinearSolver.Direct')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Mapping.NonLinear')
    rootNode.addObject('RequiredPlugin', name='Sofa.Component.Topology.Container.Dynamic')
    
    rootNode.addObject('FreeMotionAnimationLoop')
    rootNode.addObject('CollisionPipeline', verbose='0', depth='10', draw='0')
    rootNode.addObject('BruteForceBroadPhase')
    rootNode.addObject('BVHNarrowPhase')
    rootNode.addObject('MinProximityIntersection', name='Proximity', alarmDistance='10', contactDistance='0.02')
    rootNode.addObject('CollisionResponse', name='Response', response='FrictionContactConstraint', responseParams=f'mu=0.,alpha={alpha}')
    rootNode.addObject('GenericConstraintSolver', maxIterations='10', multithreading='true', tolerance='1.0e-3')

    boxTranslation = "-20 -0.9 0"
    boxPath = os.path.join(dirname, 'cube.obj')
    rootNode.addObject('MeshOBJLoader', name='Loader-box', filename=boxPath, translation=boxTranslation)

    rootNode.dt = dt

    rootNode.gravity = [g, 0, -1]

    # rootNode/Box
    Box = rootNode.addChild('Box')

    Box = Box
    Box.addObject('EulerImplicitSolver', name='EulerImplicitScheme')
    Box.addObject('SparseLDLSolver', name='linearSolver', template='CompressedRowSparseMatrixd', linearSystem='@system')
    Box.addObject('MatrixLinearSystem', template='CompressedRowSparseMatrixd', name='system')

    Box.addObject('MechanicalObject', name='BoxDOF', template='Rigid3d', position=boxTranslation+' 0 0 0 1')
    Box.addObject('UniformMass', totalMass=m)

    # rootNode/Box/Collision
    Collision = Box.addChild('Collision')
    Collision = Collision
    Collision.addObject('PointSetTopologyContainer', name='boxCollision', position=boxTranslation)
    Collision.addObject('PointSetTopologyModifier', name='Modifier')
    Collision.addObject('MechanicalObject', name='CollisionDOF', template='Vec3d')
    Collision.addObject('RigidMapping', name='MappingCollision', input='@../BoxDOF', output='@CollisionDOF', globalToLocalCoords='true')
    Collision.addObject('PointCollisionModel', name='CenterLineCollisionModel', proximity='0.02')

    # rootNode/Box/Visu
    Visu = Box.addChild('Visu')
    Visu = Visu
    Visu.addObject('OglModel', name='VisualModel', color='0.7 0.7 0.7 0.8', position='@../../Loader-box.position', triangles='@../../Loader-box.triangles')
    Visu.addObject('RigidMapping', name='SurfaceMapping', input='@../BoxDOF', output='@VisualModel', globalToLocalCoords='true')
    
    Box.addObject('LinearSolverConstraintCorrection', name='ConstraintCorrection', linearSolver='@linearSolver')

    # rootNode/Floor
    Floor = rootNode.addChild('Floor')
    Floor = Floor
    Floor.addObject('TriangleSetTopologyContainer', name='FloorTopology', position='-20 -15 -2  50000 -15 -2  50000 15 -2  -20 15 -2', triangles='0 2 1  0 3 2')
    Floor.addObject('MechanicalObject', name='FloorDOF', template='Vec3d')
    Floor.addObject('TriangleCollisionModel', name='FloorCM', proximity='0.002', moving='0', simulated='0', color='0.3 0.3 0.3 0.1')
    Floor.addObject('OglModel', name='VisualModel', src='@FloorTopology')

    return 0

def main_gui():
    import SofaRuntime
    import Sofa.Gui

    root = Sofa.Core.Node('root')
    createScene(root)
    Sofa.Simulation.init(root)

    Sofa.Gui.GUIManager.Init('myscene', 'qglviewer')
    Sofa.Gui.GUIManager.createGUI(root, __file__)
    Sofa.Gui.GUIManager.SetDimension(1080, 1080)
    Sofa.Gui.GUIManager.MainLoop(root)
    Sofa.Gui.GUIManager.closeGUI()

def error_as_a_function_of_dt():
    import matplotlib.pyplot as plt
    import numpy as np

    m = 1
    alpha = .5
    g = 1

    dt_arr = np.logspace(-4,-2,10)
    limitSpeeds = np.zeros_like(dt_arr)
    for i, dt in enumerate(dt_arr):
        print('\r', i, '/', len(dt_arr))
        root = Sofa.Core.Node("root")

        createScene(root, dt=dt, m=m, alpha=alpha, g=g)

        Sofa.Simulation.init(root)

        previousVelocity = 1.
        
        while True:
            Sofa.Simulation.animate(root, root.dt.value)
            currentVelocity = root.Box.BoxDOF.velocity.value[0][0]
            if abs((currentVelocity-previousVelocity)/previousVelocity) < 1e-9:
                limitSpeeds[i] = currentVelocity
                break
            previousVelocity = currentVelocity
    
    limitSpeed = m*g/alpha
    plt.figure()
    plt.plot(dt_arr, abs((limitSpeed-limitSpeeds)/limitSpeed), 'Dk')
    plt.loglog()
    plt.xlabel('$\delta t$')
    plt.ylabel('$\\frac{|v_{lim}-v|}{v_{lim}}$', rotation=0)
    plt.gca().invert_xaxis()
    plt.savefig(os.path.join(dirname, 'err_against_dt.png'))
    plt.show()

    return 0

def limit_speed_as_a_function_of_m_and_alpha():
    import matplotlib.pyplot as plt
    import numpy as np

    g = 1
    N = 10
    masses = np.logspace(-1,.8,N)
    alphas = np.logspace(0,.8,N)
    limitSpeeds = np.zeros((N,N))
    for i, m in enumerate(masses):
        print('m = ', m)
        for j, alpha in enumerate(alphas):
            root = Sofa.Core.Node("root")

            createScene(root, dt=1e-2, m=m, alpha=alpha, g=g)

            Sofa.Simulation.init(root)

            previousVelocity = 1.
            
            while True:
                Sofa.Simulation.animate(root, root.dt.value)
                currentVelocity = root.Box.BoxDOF.velocity.value[0][0]
                err =  abs((currentVelocity-previousVelocity)/previousVelocity)
                if err < 1e-5:
                    limitSpeeds[i,j] = currentVelocity
                    break
                previousVelocity = currentVelocity
                print('\r\talpha = ', alpha, f'error = {err:.2e}', end='')
            print()

    plt.figure()
    for limitSpeedsColumn in limitSpeeds.transpose():
        plt.plot(masses, limitSpeedsColumn, 'D-')

    plt.legend(labels=[f'$\\alpha = {alpha:.1f}$' for alpha in alphas])
    plt.xlabel('$m$')
    plt.ylabel('$v_{lim}$')
    plt.savefig(os.path.join(dirname, 'v_lim_against_m_alpha.png'))
    plt.show()

    plt.figure()
    for limitSpeedsRow in limitSpeeds:
        plt.plot(alphas, limitSpeedsRow, 'D-')

    plt.legend(labels=[f'$m = {m:.2f}$' for m in masses])
    plt.xlabel('$\\alpha$')
    plt.ylabel('$v_{lim}$')
    plt.savefig(os.path.join(dirname, 'v_lim_against_alpha_m.png'))
    plt.show()

def limit_speed_as_a_function_of_m_and_g():
    import matplotlib.pyplot as plt
    import numpy as np

    alpha = 1
    N = 10
    masses = np.logspace(-1,.8,N)
    gs = np.logspace(-1,1,N)
    limitSpeeds = np.zeros((N,N))
    for i, m in enumerate(masses):
        print('m = ', m)
        for j, g in enumerate(gs):
            root = Sofa.Core.Node("root")

            createScene(root, dt=1e-2, m=m, alpha=alpha, g=g)

            Sofa.Simulation.init(root)

            previousVelocity = 1.
            
            while True:
                Sofa.Simulation.animate(root, root.dt.value)
                currentVelocity = root.Box.BoxDOF.velocity.value[0][0]
                err =  abs((currentVelocity-previousVelocity)/previousVelocity)
                if err < 1e-5:
                    limitSpeeds[i,j] = currentVelocity
                    break
                previousVelocity = currentVelocity
                print('\r\tg = ', g, f'error = {err:.2e}', end='')
            print()

    plt.figure()
    for limitSpeedsColumn in limitSpeeds.transpose():
        plt.plot(masses, limitSpeedsColumn, 'D-')

    plt.legend(labels=[f'$g = {g:.1f}$' for g in gs])
    plt.xlabel('$m$')
    plt.ylabel('$v_{lim}$')
    plt.savefig(os.path.join(dirname, 'v_lim_against_m_g.png'))
    plt.show()

    plt.figure()
    for limitSpeedsRow in limitSpeeds:
        plt.plot(gs, limitSpeedsRow, 'D-')

    plt.legend(labels=[f'$m = {m:.2f}$' for m in masses])
    plt.xlabel('$g$')
    plt.ylabel('$v_{lim}$')
    plt.savefig(os.path.join(dirname, 'v_lim_against_g_m.png'))
    plt.show()

def limit_speed_accuracy():
    import matplotlib.pyplot as plt
    import numpy as np

    N = 3
    masses = np.logspace(-1,.8,N)
    gs = np.logspace(-1,1,N)
    alphas = np.logspace(0,.8,N)
    limitSpeeds = []
    for m in masses:
        print('m = ', m)
        for alpha in alphas:
            print('\talpha = ', alpha)
            for g in gs:
                root = Sofa.Core.Node("root")

                createScene(root, dt=1e-4, m=m, alpha=alpha, g=g)

                Sofa.Simulation.init(root)

                previousVelocity = 1.
                theoreticalVelocity = m*g/alpha
                while True:
                    Sofa.Simulation.animate(root, root.dt.value)
                    currentVelocity = root.Box.BoxDOF.velocity.value[0][0]
                    err =  abs((currentVelocity-previousVelocity)/previousVelocity)
                    if err < 1e-9:
                        limitSpeeds.append([theoreticalVelocity,currentVelocity])
                        break
                    previousVelocity = currentVelocity
                    print('\r\t\tg = ', g, f'error = {err:.2e}', end='')
                print()
    limitSpeeds = np.array(limitSpeeds)
    plt.plot(limitSpeeds[:,0])
    plt.plot(limitSpeeds[:,1])
    plt.show()
    plt.figure()
    plt.boxplot((limitSpeeds[:,1]-limitSpeeds[:,0])/(1e-12+limitSpeeds[:,0]))
    plt.show()

# Function used only if this script is called from a python environment
if __name__ == '__main__':
    # error_as_a_function_of_dt()
    # limit_speed_as_a_function_of_m_and_alpha()
    # limit_speed_as_a_function_of_m_and_g()
    limit_speed_accuracy()