# Rigid bodies and joints, Compliant-style.
# 
# Authors: maxime.tournier@inria.fr, ... ?
#
# The basic philosophy is that python provides nice data structures to
# define scene component *semantically*. Once this is done, the actual
# scene graph must be generated through the 'insert' methods. Please
# refer to the python examples in Compliant for more information.
#

import Sofa

from subprocess import Popen, PIPE

import Quaternion as quat
import Vec as vec

from Tools import cat as concat


class Frame:
        # a rigid frame, group operations are available.

        # TODO kwargs
        def __init__(self, value = None):
                if value != None:
                        self.translation = value[:3]
                        self.rotation = value[3:]
                else:
                        self.translation = [0, 0, 0]
                        self.rotation = [0, 0, 0, 1]
                
        def insert(self, parent, **args):
                return parent.createObject('MechanicalObject', 
                                           template = 'Rigid',
                                           position = str(self),
                                           **args)
                
        def __str__(self):
                return concat(self.translation) + ' ' + concat(self.rotation) 
               
        def read(self, str):
                num = map(float, str.split())
                self.translation = num[:3]
                self.rotation = num[3:]
                return self

	def __mul__(self, other):
		res = Frame()
		res.translation = vec.sum(self.translation,
					  quat.rotate(self.rotation,
						      other.translation))
		res.rotation = quat.prod( self.rotation,
					  other.rotation)

		return res

	def inv(self):
		res = Frame()
		res.rotation = quat.conj( self.rotation )
		res.translation = vec.minus( quat.rotate(res.rotation,
							 self.translation) )
		return res

        def set(self, **kwargs):
                for k in kwargs:
                        setattr(self, k, kwargs[k])
                        
                return self
					  
        # TODO more: apply( vec3 ), wrench/twist frame change.


class MassInfo:
        pass

# front-end to sofa GenerateRigid tool. density unit is kg/m^3
def generate_rigid(filename, density = 1000.0):
        cmd = Sofa.build_dir() + '/bin/GenerateRigid'
        args = filename
	try:
		output = Popen([cmd, args], stdout=PIPE)
		line = output.stdout.read().split('\n')
	except OSError:
                # try the debug version
                cmd += 'd'
                
                try:
                        output = Popen([cmd, args], stdout=PIPE)
                        line = output.stdout.read().split('\n')
                except OSError:
                        print 'error when calling GenerateRigid, do you have GenerateRigid built in SOFA ?'
                        raise

        start = 2
        
        # print line 
        
        mass = float( line[start].split(' ')[1] )
        volm = float( line[start + 1].split(' ')[1] )
        inrt = map(float, line[start + 2].split(' ')[1:] )
        com = map(float, line[start + 3].split(' ')[1:] )
        
        # TODO extract principal axes basis if needed
        # or at least say that we screwd up
        
        res = MassInfo()

        # by default, GenerateRigid assumes 1000 kg/m^3 already
        res.mass = (density / 1000.0) * mass

        res.inertia = [mass * x for x in inrt]
        res.com = com

        return res



class Body:
        # generic rigid body
        
        def __init__(self, name = "unnamed"):
                self.name = name         # node name
                self.collision = None # collision mesh
                self.visual = None    # visual mesh
                self.dofs = Frame()   # initial dofs
                self.mass = 1         # mass 
                self.inertia = [1, 1, 1] # inertia tensor
                self.color = [1, 1, 1]   # not sure this is used 
                self.offset = None       # rigid offset for com/inertia axes
                self.inertia_forces = False # compute inertia forces flag
                self.group = None
                self.mu = 0           # friction coefficient
                self.scale = [1, 1, 1]

                # TODO more if needed (scale, color)
                
        def mass_from_mesh(self, name, density = 1000.0):
                info = generate_rigid(name, density)

                self.mass = info.mass
                
                # TODO svd inertia tensor, extract rotation quaternion
                
                self.inertia = [info.inertia[0], 
                                info.inertia[3 + 1],
                                info.inertia[6 + 2]]
                
                self.offset = Frame()
                self.offset.translation = info.com
                
                # TODO handle principal axes
                
        def insert(self, node):
                res = node.createChild( self.name )

                dofs = self.dofs.insert(res, name = 'dofs' )
                
                mass_node = res
                
                if self.offset != None:
                        mass_node = res.createChild('mapped_mass')
                        self.offset.insert(mass_node, name = 'dofs')
                        mapping = mass_node.createObject('AssembledRigidRigidMapping',
                                                         template = 'Rigid',
                                                         source = '0 ' + str( self.offset) )
                # mass
                mass = mass_node.createObject('RigidMass', 
                                              template = 'Rigid',
                                              name = 'mass', 
                                              mass = self.mass, 
                                              inertia = concat(self.inertia),
                                              inertia_forces = self.inertia_forces )
                
                # visual model
                if self.visual != None:
                        visual_template = 'ExtVec3f'
                        
                        visual = res.createChild( 'visual' )
                        ogl = visual.createObject('OglModel', 
                                                  template = visual_template, 
                                                  name='mesh', 
                                                  fileMesh = self.visual, 
                                                  color = concat(self.color), 
                                                  scale3d = concat(self.scale))
                        
                        visual_map = visual.createObject('RigidMapping', 
                                                         template = 'Rigid' + ', ' + visual_template, 
                                                         input = '@../')
                # collision model
                if self.collision != None:
                        collision = res.createChild('collision')
                
                        collision.createObject("MeshObjLoader", 
					       name = 'loader', 
					       filename = self.collision,
                                               scale3d = concat(self.scale) )
			
                        collision.createObject('MeshTopology', 
                                               name = 'topology',
                                               triangles = '@loader.triangles')
                        
			collision.createObject('MechanicalObject',
                                               name = 'dofs',
                                               position = '@loader.position')
                        
			model = collision.createObject('TriangleModel', 
                                               name = 'model',
                                               template = 'Vec3d',
                                               contactFriction = self.mu)
                        if self.group != None:
                                model.group = self.group                        
                        
			collision.createObject('RigidMapping',
                                               template = 'Rigid,Vec3d',
                                               input = '@../',
                                               output = '@./')
                return res


class Joint:
        # generic rigid joint

        def __init__(self, name = 'joint'):
                self.dofs = [0] * 6
                self.body = []
                self.offset = []
                self.name = name

                # link constraints compliance
                self.compliance = 0
                
                # TODO if you're looking for damping/stiffness, you
                # now should do it yourself, directly on joint dofs
                
        def append(self, node, offset = None):
                self.body.append(node)
                self.offset.append(offset)
                self.name = self.name + '-' + node.name
        
        # convenience: define joint using absolute frame and vararg nodes
        def absolute(self, frame, *nodes):
                for n in nodes:
                        pos = n.getObject('dofs').position
                        s = concat(pos[0])
                        local = Frame().read( s )
                        self.append(n, local.inv() * frame)
        
        # joint dimension
        def dim(self):
                return sum( self.dofs )

        class Node:
                pass
        
        def insert(self, parent):
                # build input data for multimapping
                input = []
                for b, o in zip(self.body, self.offset):
                        if o is None:
                                input.append( '@' + b.name + '/dofs' )
                        else:
                                joint = b.createChild( self.name + '-offset' )
                                
                                joint.createObject('MechanicalObject', 
                                                   template = 'Rigid', 
                                                   name = 'dofs' )
                                
                                joint.createObject('AssembledRigidRigidMapping', 
                                                   template = "Rigid,Rigid",
                                                   source = '0 ' + str( o ) )
                                
                                input.append( '@' + b.name + '/' + joint.name + '/dofs' )
                             
                if len(input) == 0:
                        print 'warning: empty joint'
                        return None
   
                # now for the joint dofs
                node = parent.createChild(self.name)
                
                dofs = node.createObject('MechanicalObject', 
                                         template = 'Vec6d', 
                                         name = 'dofs', 
                                         position = '0 0 0 0 0 0' )
                
                map = node.createObject('RigidJointMultiMapping',
                                        name = 'mapping', 
                                        template = 'Rigid,Vec6d', 
                                        input = concat(input),
                                        output = '@dofs',
                                        pairs = "0 0")
                
		sub = node.createChild("constraints")

		sub.createObject('MechanicalObject', 
				 template = 'Vec1d', 
				 name = 'dofs')
		
		mask = [ (1 - d) for d in self.dofs ]
		
		map = sub.createObject('MaskMapping', 
				       name = 'mapping',
				       template = 'Vec6d,Vec1d',
				       input = '@../',
				       output = '@dofs',
				       dofs = concat(mask) )
		
                compliance = sub.createObject('UniformCompliance',
					      name = 'compliance',
					      template = 'Vec1d',
					      compliance = self.compliance)

                stab = sub.createObject('Stabilization')
                
		return node


# and now for more specific joints:

class SphericalJoint(Joint):

        def __init__(self, **args):
                Joint.__init__(self)
                self.dofs = [0, 0, 0, 1, 1, 1]
                self.name = 'spherical'
                
                for k in args:
                        setattr(self, k, args[k])

# this one has limits \o/
class RevoluteJoint(Joint):

        # TODO make this 'x', 'y', 'z' instead
        def __init__(self, axis, **args):
                Joint.__init__(self)
                self.dofs[3 + axis] = 1
                self.name = 'revolute'
                self.lower_limit = None
                self.upper_limit = None

                for k in args:
                        setattr(self, k, args[k])


        def insert(self, parent):
                res = Joint.insert(self, parent)

                if self.lower_limit == None and self.upper_limit == None:
                        return res
                
                limit = res.createChild('limit')

                dofs = limit.createObject('MechanicalObject', template = 'Vec1d')
                map = limit.createObject('ProjectionMapping', template = 'Vec6d, Vec1d' )

                limit.createObject('UniformCompliance', template = 'Vec1d', compliance = '0' )
                limit.createObject('UnilateralConstraint');

                # don't stabilize as we need to detect violated
                # constraints first
                # limit.createObject('Stabilization');

                set = []
                position = []
                offset = []

                if self.lower_limit != None:
                        set = set + [0] + self.dofs
                        position.append(0)
                        offset.append(self.lower_limit)

                if self.upper_limit != None:
                        set = set + [0] + vec.minus(self.dofs)
                        position.append(0)
                        offset.append(- self.upper_limit)
                
                map.set = concat(set)
                map.offset = concat(offset)
                dofs.position = concat(position)

                return res


class CylindricalJoint(Joint):

        def __init__(self, axis ):
                Joint.__init__(self)
                self.dofs[0 + axis] = 1
                self.dofs[3 + axis] = 1
                self.name = 'cylindrical'

class PrismaticJoint(Joint):

        def __init__(self, axis):
                Joint.__init__(self)
                self.dofs[0 + axis] = 1
                self.name = 'prismatic'

class PlanarJoint(Joint):

        def __init__(self, normal):
                Joint.__init__(self)
                self.dofs = [ 
                        int( (i != normal) if i < 3 else (i - 3 == normal) )
                        for i in xrange(6)
                ]
                self.name = 'planar'
