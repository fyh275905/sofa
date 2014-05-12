import Sofa

import inspect
import os

## @package SofaTest
#  Python helper to perform test in Sofa

## A controller to return a test result directly from a python script.
class Controller(Sofa.PythonScriptController):

    # to ensure member variables are declared and accessible
    node = 0
    root = 0

    ## @internal storing node pointers
    # @warning must be called manually if your own Controller::onLoaded is surcharged
    def onLoaded(self, node):
        self.node = node
        self.root = node.getRoot()
    
    ## Send a success event.
    def sendSuccess(self):
        self.node.sendScriptEvent('success', 0)
        self.root.findData('animate').value = 0

    ## Send a failure event.
    #
    #   @param msg A message to print out
    def sendFailure(self, msg = 'unknown'):
        callerframerecord = inspect.stack()[1]

        frame = callerframerecord[0]
        info = inspect.getframeinfo(frame)

        f = open(info.filename)
        lines = f.readlines()

        # print
        print '{0}:{1}: {2}'.format(os.path.abspath(info.filename),
                                            info.lineno, 'Failure')
        print 'Reason:', msg
        # print #lines[ info.lineno - 1 ]
        self.node.sendScriptEvent('failure', 0)
        self.root.findData('animate').value = 0
        
    ## Send a success event if value is true. Otherwise a failure event is sent.
    #  @param value A value used to set which failure/success event is sent.
    #  @param msg A meesage to print out.
    def should(self, value, msg = 'unknown'):
        if value:
            self.sendSuccess()
        else:
            self.sendFailure( msg )
            

