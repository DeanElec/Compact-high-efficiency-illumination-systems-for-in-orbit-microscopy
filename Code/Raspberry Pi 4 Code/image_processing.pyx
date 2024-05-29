import os
import rawpy
import imageio
from PIL import Image
from libc.stdio cimport printf

#define the external c function 
cdef extern from "image_processing.h":
    void process_image(char* filename, dict user_options)

#python wrapper function to process the image
def process_image(filename, dict user_options):
    print(f"Processing image: {filename}")
    try:
        #convert string values in user_options to appropriate types
        converted_options = convert_options(user_options)
        #process the image with converted options
        _process_image_c(filename.encode('utf-8'), converted_options)
    except Exception as e:
        print(f"Exception occurred: {e}")

cdef void _process_image_c(char* filename, dict user_options):
    #convert filename back to python string for use with os and rawpy
    path = filename.decode('utf-8')
    with rawpy.imread(path) as raw:
        #use the converted user options for rawpys postprocess function
        rgb = raw.postprocess(**user_options)
        base_filename = os.path.splitext(path)[0]
        tiff_path = f"{base_filename}.tiff"
        
        #save the processed image as TIFF
        imageio.imsave(tiff_path, rgb)

        #convert TIFF to PNG using PIL for compatibility with full bit depth
        png_path = f"{base_filename}.png"
        with Image.open(tiff_path) as img_tiff:
            img_tiff.save(png_path, format='PNG')
        print(f"Saved PNG image at: {png_path}")

#function to convert option values from strings to their correct types
def convert_options(dict user_options):
    converted_options = {}
    for key, value in user_options.items():
        if key == "output_bps":
            converted_options[key] = int(value)
        elif key in ["half_size", "no_auto_bright", "no_auto_scale", "use_camera_wb"]:
            converted_options[key] = value == "True"
        elif key.startswith("demosaic_algorithm"):
            converted_options[key] = getattr(rawpy.DemosaicAlgorithm, value.split(".")[-1], None)
        elif key == "output_color":
            converted_options[key] = getattr(rawpy.ColorSpace, value.split(".")[-1], None)
        elif key == "bright":
            converted_options[key] = float(value)
        elif key == "user_wb":
            converted_options[key] = tuple(map(float, value.split(',')))
        elif key == "gamma":
            converted_options[key] = tuple(map(float, value.split(',')))
        else:
            converted_options[key] = value
    return converted_options
