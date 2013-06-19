BRPProstateNav
==============

3D Slicer module for MRI-guided robotic prostate intervention.

Protocol
--------

The following table shows message exchange diagram for the communication between 3D Slicer (and other navigation software) and the robot in each workhpase. 


<table border="1" cellpadding="5" cellspacing="0" align="center">

<tr>
<td style="background:#e0e0e0;"> <i>3D Slicer (operator)</i>
</td><td style="background:#e0e0e0;"> <i>Message</i>
</td><td style="background:#e0e0e0;"> <i>Robot Controller</i>
</td><td style="background:#e0e0e0;"> <i>Radiologist</i>
</td><td style="background:#e0e0e0;"> <i>Note</i>
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;"> Start-up
</td></tr>
<tr>
<td align="left"> The operator presses "Start-up" button
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Send command to robot
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, START_UP) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, START_UP) &lt;&lt;
</td><td align="left"> Echo back an acknowledgement command was received, but not yet completed
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the START_UP message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Start up and initialize the hardware. Run the robot homing procedure if necessary (skip if already successfully completed). Move robot to home (loading) configuration.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(START_UP, OK:??:??) &lt;&lt; <br /> DEVICE_NAME=START_UP <br /><i>C=1(OK), SubCode=??, ErrorName=??</i>
</td><td align="left"> Confirm when robot is initialized <br /> TODO: Refine error/status codes
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Display the result of start up process.
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;"> Planning
</td></tr>
<tr>
<td align="left"> The operator opens the planning panel
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, PLANNING) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left">  XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, PLANNING) &lt;&lt;
</td><td align="left"> Echo back an acknowledgement command was received
</td><td align="left">
</td><td align="left">  XXXX is the same unique query ID as the PLANNING message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Do nothing except keep track of current state, robot is awaiting next workphase.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Show that the robot is in PLANNING phase.
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;"> Calibration
</td></tr>
<tr>
<td align="left"> The operator opens the calibration panel
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, CALIBRATION) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, CALIBRATION) &lt;&lt;
</td><td align="left"> Echo back an acknowledgement command was received
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the CALIBRATION message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Do nothing except keep track of current state, robot is awaiting calibration transform
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Show that the robot is in CALIBRATION phase.
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Nav Software (3D Slicer or RadVision) calculates calibration matrix
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; TRANSFORM(CLB_XXXX, 4x4 calibration matrix in RAS coordinates) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left">XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; TRANSFORM(ACK_XXXX, 4x4 calibration matrix in RAS coordinates) &lt;&lt;
</td><td align="left"> Echo back an acknowledgement transform was received
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the CLB message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Update calibration transform, set flag that registration has been set externally, reply with confirmation
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(CALIBRATION, OK:??:??) &lt;&lt; <br /> DEVICE_NAME=CALIBRATION <br /><i>C=1(OK), SubCode=??, ErrorName=??</i>
</td><td align="left"> Confirm that calibration was received and robot is ready for next workphase (targeting)
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Show that calibration successfully sent to robot.
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;"> Targeting
</td></tr>
<tr>
<td align="left"> The operator enters "Targeting" mode
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, TARGETING) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left">XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, TARGETING) &lt;&lt;
</td><td align="left"> Acknowledge receiving targeting command
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the TARGETING message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Confirm if robot is ready for targeting, check if calibration was received
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(TARGETING, OK:??:??) &lt;&lt; <br /> DEVICE_NAME=TARGETING <br /><i>C=1(OK), SubCode=??, ErrorName=??</i>
</td><td align="left"> Confirm robot has entered targeting mode
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(TARGETING, DNR:??:??) &lt;&lt; <br /> DEVICE_NAME=TARGETING <br /><i>C=13(DeviceNotReady), SubCode=??, ErrorName=??</i>
</td><td align="left"> <b>ERROR CASE:</b> If not able to enter targeting mode (i.e. calibration not received, reply with Code:13 (Device Not Ready)
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> The operator select a target, Nav software creates a 4x4 matrix for desired 6-DOF robot pose to reach the target
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; TRANSFORM(TGT_XXXXX, 4x4 target matrix in RAS coordinates) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; TRANSFORM(ACK_XXXXX, 4x4 target matrix) &lt;&lt;
</td><td align="left"> Acknowledge receipt of target transformation by echoing back
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the TARGETING message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Calculate if target pose is reachable based on the kinematics, reply with status and set target
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(TARGET, OK:??:??) &lt;&lt; <br /> DEVICE_NAME=TARGETING <br /><i>C=1(OK), SubCode=??, ErrorName=??</i>
</td><td align="left"> Reply with OK if target was accepted
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; TRANSFORM(TARGET, 4x4 target matrix) &lt;&lt;
</td><td align="left"> Send actual target pose in robot controller if one was set (corresponds to when status comes back OK)
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(TARGET, DNR:??:??) &lt;&lt; <br /> DEVICE_NAME=TARGET <br /><i>C=13(DeviceNotReady), SubCode=??, ErrorName=??</i>
</td><td align="left"> <b>ERROR CASE:</b> If not in targeting mode, reply with Code:13 (Device Not Ready)
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(TARGET, CE:??:??) &lt;&lt; <br /> DEVICE_NAME=TARGET <br /><i>C=10(ConfigurationError), SubCode=??, ErrorName=??</i>
</td><td align="left"> <b>ERROR CASE:</b> If not a valid target (i.e. out of workspace), reply with Code:10 (ConfigurationError)
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Display the reachable target position set in robot controller.
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> The operator confirms the target position set in the controller, and press "MOVE"
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, MOVE_TO_TARGET) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, MOVE_TO_TARGET) &lt;&lt;
</td><td align="left"> Echo back an acknowledgement command was received (not yet completed)
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the MOVE_TO_TARGET message.
</td></tr>
<tr>
<td align="left"> Alert the clinician to hold footpedal to align the robot
</td><td align="left">
</td><td align="left">
</td><td align="left"> Clinician engages interlock (footpedal in scanner room) to enable robot motion. Robot will only move when interlock is engaged following a move command.
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> The robot moves to the target and streams its pose during motion
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; TRANSFORM(CURRENT_POSITION, 4x4 current robot pose matrix in RAS coordinates) &lt;&lt;
</td><td align="left"> Stream current robot pose in RAS coords as moving. Can also be requested (see below).
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Display the current robot position as it moves toward the target.
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Display that the robot is at the target. Send confirmation.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(MOVE_TO_TARGET, OK:??:??) &lt;&lt; <br /> DEVICE_NAME=DONE_MOVING <br /><i>C=1(OK), SubCode=??, ErrorName=??</i>
</td><td align="left"> Reply with OK when robot reaches target
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; TRANSFORM(CURRENT_POSITION, 4x4 current robot pose matrix in RAS coordinates) &lt;&lt;
</td><td align="left"> Push out final robot pose in RAS coords as moving. (same format as previous stream - ensures last one is at final position)
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Display the current final robot position at the target.
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;"> Needle Insertion (Manual)
</td></tr>
<tr>
<td align="left"> Ask to lock the robot
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left"> The operator presses "Lock" button
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING (CMD_XXXX, MANUAL) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, MANUAL) &lt;&lt;
</td><td align="left"> Echo back an acknowledgement command was received (not yet completed)
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the MANUAL message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Cut motor power to prevent motion of the robot base. This also eliminates causes of MR interference for insertion under live imaging.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(MANUAL, OK:??:??) &lt;&lt; <br /> DEVICE_NAME=MANUAL <br /><i>C=1(OK), SubCode=??, ErrorName=??</i>
</td><td align="left"> Reply with OK when robot is in a safe, locked state
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left"> Insert a needle, optionally under live MR imaging. Perform intervention with the needle (biopsy or seed placement).
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left"> Retract the needle
</td><td align="left">
</td></tr>
<tr>
<td align="left"> Ask to unlock the robot and confirm needle is retracted
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left"> The operator presses "Unlock"
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, TARGETING) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, TARGETING) &lt;&lt;
</td><td align="left"> Acknowledge receiving targeting command
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the TARGETING message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Return robot to home (loading) position.&nbsp;?? THIS MAKE SENSE HERE, SHOULD IT BE A SEPARATE 'HOME' COMMAND
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Return to targeting mode, check if ready for targeting.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;"> Additional Commands
</td></tr>
<tr>
<td align="left"> The operator presses "Stop" button
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, STOP) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left">  XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, STOP) &lt;&lt;
</td><td align="left"> Acknowledge receiving targeting command
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the STOP message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> The robot stops all motion. Stays in current state/workphase.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(STOP, 4:??:??) &lt;&lt; <br /> DEVICE_NAME=STOP<br /><i>C=1(OK), SubCode=??, ErrorName=??</i>
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;">
</td></tr>
<tr>
<td align="left"> The operator presses "Emergency" button
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; STRING(CMD_XXXX, EMERGENCY) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXX is a unique query ID (string of any ASCII letters up to 16 bytes)
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STRING(ACK_XXXX, EMERGENCY) &lt;&lt;
</td><td align="left"> Acknowledge receiving targeting command
</td><td align="left">
</td><td align="left"> XXXX is the same unique query ID as the STOP message.
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> The robot stops all motion and disables/locks motors. Switches to Emergency state/workphase.&nbsp;?? IS THIS THE DESIRED ACTION
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(EMERGENCY, 4:??:??) &lt;&lt; <br /> DEVICE_NAME=DONE_MOVING <br /><i>C=4(Emergency), SubCode=??, ErrorName=??</i>
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;">
</td></tr>
<tr>
<td align="left"> Request current robot pose (or target or calibration transforms)
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; GET_TRANSFORM(CURRENT_POSITION) &gt;&gt;
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> The robot transmits current pose ("CURRENT_POSITION") through IGTLink upon request. This also works for requesting "TARGET_POSITION" and "CALIBRATION" transforms stored in robot controller.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; TRANSFORM(CURRENT_POSITION, 4x4 current robot pose matrix in RAS coordinates) &lt;&lt;
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;">
</td></tr>
<tr>
<td align="left"> Request the robot status/workphase
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &gt;&gt; GET_STATUS(CURRENT_STATUS) &gt;&gt;&nbsp;?? CONFIRM COMMAND STRUCTURE FOR STATUS REQUEST
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Sends current state/workphase.&nbsp;?? SHOULD IT SEND OTHER INFO TOO
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(XXXXX, YYYYY) &lt;&lt;
</td><td align="left">
</td><td align="left">
</td><td align="left"> XXXXX is workphase (e.g. TARGETING), YYYYY is status.
</td></tr>
<tr>
<td colspan="5" align="center" style="background:#f0f0f0;">
</td></tr>
<tr>
<td align="left">
</td><td align="left">
</td><td align="left"> Robot controller sends errors or notifications through IGTLink. Transmitted asynchronously with error text in message body. To be used with limit events, hardware failures, invalid commands, etc.
</td><td align="left">
</td><td align="left">
</td></tr>
<tr>
<td align="left">
</td><td align="left"> &lt;&lt; STATUS(ERROR, Error Text) &lt;&lt;
</td><td align="left">
</td><td align="left">
</td><td align="left">
</td></tr></table>


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









