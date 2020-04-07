How to build Fullplay Media Library.
------------------------------------

Open phase1.dsw in Visual Studio 6.0 (or Visual Studio .Net)

In Visual Studio, you need to specify the include and library file directory.  
In VS 6.0, select Tools/Options and then the Directories tab.  
Make sure that the FML/Code directory is the first one for the 
libraries and that FML and FML/Code are the first two for the
include files.

Select the Build menu and select 'Build'.

The new fpml.exe will be placed in the installFiles directory.  
These files are used by Install Shield to make the FML release.

The version number on the FML screen can be changed in the file version.h.  
This is in the "VersionInfo" folder within the VS workspace.

*****

If you want to build, you can now do it from the command line as well (just open up a dos prompt):  

cd c:\fml\code
nmake phase1_gui.mak


Using InstallShield to make a release
-------------------------------------

Use InstallShield Express 3.5 Service Pack 4

Open the file fml.ism

1) Product version
Click on 'General Information', then click on the 'Product Code' field.  Click the 
'generate guid' button that appears in the lower pane to make a new guid.
Don't change the 'Upgrade Code' field.
Change the 'Product version' field to something appropriate.

2) Upgrade path
Click on the 'Upgrade Paths' on the left pane.  Then right click the 'Upgrade Paths' 
in the middle pane and select 'New upgrade path'.  Browse for the previous fml build's .msi file

2.5) Specifiy Application Data --> Files.  Make sure the files here match the local path to 
the files you're building.

2.6) Customize the Setup Appearance --> Dialogs --> Licence Agreement.  Change the license.rtf 
path to match your local path.

2.7) Define Setup Requirements --> Custom Actions.  Change path to match your local WMFDist.exe.

3) Building releases
Click on the 'Build Your Release' on the left pane.  
Right click on the middle pane item and select 'build' to make the build.  
You should build CD_ROM and SingleImage versions.

4) Distributing releases
You can either browse the folders and find the 'disk 1' folders, 
or click on 'Distribute Your Release' on the left pane and then follow the instructions.


