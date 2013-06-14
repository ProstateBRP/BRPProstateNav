BRPProstateNav
==============

3D Slicer module for MRI-guided robotic prostate intervention.


Session Manager Node
--------------------

### Overview

BRPProstateNav manages the communication between 3D Slicer and external robot software through a IGTLSessionManagerNode node. The roles of the IGTLSessionManagerNode are as follows:

- Observes an IGTL connector node for the communication.
- Create MRML nodes that will hold incoming and outgoing message data and register them to the IGTL connector.
- Call message handler when an OpenIGTLink message has been received.

The IGTLSessionManagerNode is wrapped by Python, so that a user can call it's API from 3D Slicer's Python Interactor.

### Example

Currently, you can push a command message from the Python Interactor. After opening "BRPProstateNav" module, open the Python Interactor and type in the following lines:

    >>> from vtkSlicerBRPProstateNavModuleMRMLPython import *
    >>> scene = slicer.mrmlScene
    >>> cnode = scene.GetNodeByID('vtkMRMLIGTLConnectorNode1')
    >>> sm = vtkMRMLIGTLSessionManagerNode()
    >>> scene.AddNode(sm.GetID())
    >>> scene.AddNode(sm)
    >>> sm.SetAndObserveConnectorNodeID(cnode.GetID())
    >>> snode = scene.GetNodeByID('vtkMRMLAnnotationTextNode1')
    >>> snode = scene.GetNodeByID('vtkMRMLAnnotationTextNode1')
    >>> snode
    >>> snode.SetTextLabel('Test')
    >>> snode.SetTextLabel('COMMAND')










