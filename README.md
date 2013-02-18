ofProjects
==========

to compile the apps first install openframeworks and then clone the git into the of-folder.


FamilyPortrait   apps/2013_interlab/familyPortrait
===================================================
- compiles on MacOS 10.7x

mySettings.xml:
----------------
- defines all the settings the program starts with

<FILES>
    <CITY> your City
    <TIMEZONE> difference of your timezone to Dresden (Cairo = 1)
    <PICTURE_FOLDER> dropboxfolder to store the images (absolute path)
<IMAGE_PROCESSING>
    <WIDTH> camera x resolution 
    <HEIGTH> camera y resolution
    <THRESHOLD> threshold of difference picture 
    <MAX_SHOWN_PICTURES> maximum photo layers shown
    <IMAGE_MODE> the image filte (COLOR, BLACKWHITE or SEPIA)


Shortcuts:
-----------

'g'     toggle gui
'+' '-' increase/decrease threshold
'n' 'm' increase/decrease maximum shown photo layers
'i'     toggle image mode
'f'     toggle fullscreen


