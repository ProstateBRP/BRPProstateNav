BRPProstateNav
==============

3D Slicer module for MRI-guided robotic prostate intervention.

Protocol
--------

The following page shows message exchange diagram for the communication between 3D Slicer (and other navigation software) and the robot in each workhpase. 

http://wiki.na-mic.org/Wiki/index.php/ProstateBRP_OpenIGTLink_Communication_June_2013


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
    >>> scene.AddNode(sm)
    >>> sm.SetAndObserveConnectorNodeID(cnode.GetID())
    >>> snode = scene.GetNodeByID('vtkMRMLAnnotationTextNode1')

If you wat to send 'Test' command, call the following line:

    >>> snode.SetTextLabel('Test')



### Plans

Currently, the node is developed as part of BRPProstateNav () and LightWeightRobotIGT (https://github.com/SNRLab/LightWeightRobotIGT ) projects separately. The SessionManagerNode will become a parent class for all application-specific session management classes and integrated into OpenIGTLinkIF module. 









