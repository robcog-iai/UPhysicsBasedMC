# Setting up Ikinema Orion

This is a short guide on how to setup Ikinema Orion for the purpose of this project and what to keep in mind. For more information check out the [official documentation for Orion](https://ikinema.com/index.php?mod=documentation&show=317).

## Installing Orion software

* First you log into the Ikinema homepage. After logging in you will see a list of their software at the top. Click on **Ikinema Orion** and download the installer. 

* The installer will ask you what you want to install. Macke sure **Orion Streamer**, which is the software you bought a license key for, is checked. 

* Also unfold **IKINEMA VRPN Unreal plug-in** and select the UE versions you want to download plugins for. 

* You can uncheck **IKINEMA VRPN Unity plug-in** and **IKINEMA 3D Assets**. The last contains a skeletal mesh that is already included in this plugin. 
If you need the .fbx or a female version you can also keep it checked. 

* After finishing the installation you will have a folder with everything you need at your chosen location. The **UnrealStream** folder contains the plugin that you will have to put into your project. 

* You should also be able to start the **Orion** sofware using the start menu or a desktop shortcut. 

## Vive trackers

* For our setup we need a HTC Vive HMD, two Vive controlers and 3 or 5 Vive trackers. You can find out how to setup the HMD and controllers using their user manual or the internet, if you haven't done that yet. 

* The Vive trackers can be a little bit trickier sometimes. Make sure they are charged and once you have done that make sure to connect a wireless dongle for each tracker so they can communicate with the PC.

* Turn on the tracker by pressing the button in the middle. Then select **pair new controller** in SteamVR. If you get an error message telling you you can't add any additional controllers, you probably forgot to connect the dongle for the tracker. 

* Once SteamVR is searching for a controller press the button in the middle of the tracker for a couple seconds until it's LED starts blinking blue. It will turn green once it is connected.

* We have encountered a bug where one of the trackers gets mistaken for a vive controller in UE. The controller it was mistaken for stopped tracking completely and the trackers position was used instead.

* Disconnecting the trackers, restarting SteamVR and reconnecting them worked eventually. Also make sure that in Orion the correct marker setup is selected. (see **Calibrating Orion**)

## Calibrating Orion

![](./Images/OrionUI.png "Orion UI")

## Streaming into UE4

![](./Images/OrionStreamNode.png "Orion Stream")