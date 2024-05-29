import napari
import sys

if len(sys.argv) < 2:
    print("Usage: python script.py <path_to_image>")
    sys.exit(1)

#get the file path from the command line arguments
file_path = sys.argv[1]

#create a napari viewer
viewer = napari.Viewer()

#define a function to open an image file
def open_image(path):
    viewer.open(path)

#open the image
open_image(file_path)

#run the napari event loop
napari.run()
