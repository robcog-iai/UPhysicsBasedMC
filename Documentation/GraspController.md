# Using the GraspController

The GraspController is an actor component that can be added to a skeletal mesh. It enables the user to move the parent meshes bones only using physics. The goal is to grasp objects with the skeletal mesh of a hand, only using physics simulation during the whole process. Of course any mesh can be used and not only hands. 

## Setting up the skeletal mesh

* In order for this to work the skeletal meshes constraints need to have the same rotation they had after importing.

* If this isn't the case you need to create a new physics asset before continuing. 

* For each constraint set all angular limits to free. Also set the angular motors into SLERP mode and tick the SLERP box under **Drives**.

* The last step is supposed to be done automatically by the GraspController, but doesn't work at this point in time. 

## Creating a grasping animation

* Before we can grasp objects we need to record the movements the hand is supposed to perform later only.

* In order to do this we open the skeletal mesh that is supposed to be used in the editor.

* In the top right corner we select its skeleton asset.

* If this plugin is set up correctly you will see two new buttons at the top.

* One is called **New Grasp Anim** and the other one **Edit Grasp Anim**.

* When creating a new grasp we first click on **New Grasp Anim** and then **Start**.

* It's very important that **Start** is clicked before any changes are made to the bones.

* After that you rotate the bones into the starting position of your animation and press **Add Frame**.

* After adding the starting frame you can rotate the bones again and create whatever amount of frames you want.

* The minimum amount of frames is 2. The last frame will always be the final hand position and all the other frames are inbetween steps. 

* After you have created all your frames you press **Save** and type in a name for your animation.

* Afterwards a new DataAsset will appear in the GraspingAnimations folder inside this plugins content folder. 

* Always save it so it doesn't get deleted when closing the engine. 

## Editing a grasping animation

* When editing a grasping animation you also need to press **Start**, before any changes have been made to the mesh. 

* Afterwards load the animation you want to edit, using **Load Grasp Anim**. Type in the animation name and frame number you want to edit. 

* The mesh will now assume the position of the loaded frame. You can rotate the bones and press **Update Frame** to overwrite it. 

* **Goto Next Frame** and **Goto Previous Frame** let you skip through the frames of the currently loaded animation.

* After editing an animation you need to remember to save the edited DataAsset.

* The engine won't detect that the DataAsset has any unsaved changes, but if you close the engine without saving the asset, all changes will be lost. 

## Setting up the GraspController

* Drag the skeletal mesh into your world

* Click **Add Component** and select the **MC Grasp Controller**.

* In the controller select the correct hand type and make sure, that the text in the three action fields fits the input bindings you set up in your project settings. 

* **Grasp Action** is an axis mapping and should be set to your VR controllers trigger button, if you are using one. 

* The other two are action mappings and should be set to your VR controllers face buttons, if you are using one. 

* You can also change the **Spring**, **Damping** and **Force Limit** values, to influence how fast and strong the mesh moves. 

* Note that this component doesn't make the mesh follow your VR controllers. There are other components in this plugin to do that. 

## Additional tricks

* If your hand is not strong enough to hold an object, give it more weight and decrease the weight of any object you are interacting with. This is the most effective method I have found so far. 

* You can also try to change its physics material to give it more friction. To set the physics material you need to open the meshes normal material in the editor. 

* Two examples can be found in the **PhysicsMaterials** folder. 