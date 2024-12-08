# Compact-high-efficiency-illumination-systems-for-in--orbit-microscopy
Presents all data associated with the development of my thesis for a 3rd year final year project at the University of Nottingham

***
## Project Evaluation
The project overall was a success as the system was able to take both bright field and dark field images with high enough resolution and resolving power to accurate image Diatoms, C.elegans and Buccal Epithelial Cells.

Due to cost restraint a set of cheap lenses were used, but the datasheet that came with these lenses had all incorrect parameters such as FOV or MOD. For this reason significant time was spent characterising the lenses so the data could be relied upon.
***

## Future Work / Improvements
If this project was to be taken forward, the current PCB's would work but really require a PCB assembly house to manufacture and populate the board as it is too densely packed with too small component to do by hand and was a main struggle in the project.
That said an improved design for the illumination board has already been created and tested based on addressable SK6805 LEDs and will be uploaded to this github repo in the future once the driver board for it has been finalised and tested.
The new design will be able to be controlled via a plug-in for the open source microscopy software micro-manager, it will provide a GUI that will allow the user to control the illumination system from the host PC running micro-manager, as micro-manager
was wrote in java it is inherently cross platform and thus the plug-in should work for any OS.

Also as potential future upgrades would be to add a form of auto focus as although the microscope worked and gathered highly detailed images it was hard to get the specimens in focus.
***


