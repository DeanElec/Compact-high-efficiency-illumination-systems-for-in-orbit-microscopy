import os
import psutil
import rawpy
import imageio
from PIL import Image

# Function to convert bytes to GB
def bytes_to_gb(bytes):
    return bytes / 1024 / 1024 / 1024

# Get initial memory usage
process = psutil.Process(os.getpid())
initial_memory = process.memory_info().rss

path = '/home/deanm/Documents/Raspberry_Pi_Stuff/Image_04-02-2024_16:36:29.dng'

# Step 1: Process raw image and save as TIFF
with rawpy.imread(path) as raw:
    rgb = raw.postprocess(output_bps=16, gamma=(1,1), output_color = rawpy.ColorSpace.raw, no_auto_bright = True, bright = 2.5)
tiff_path = 'linear.tiff'
imageio.imsave(tiff_path, rgb)

# Step 2: Convert TIFF to PNG
with Image.open(tiff_path) as img_tiff:
    # Save the image in PNG format
    png_path = 'output_image.png'
    img_tiff.save(png_path, format='PNG')

# Step 3: Print completion message
print(f"Conversion complete. Images saved as {tiff_path} and {png_path}")

# Get final memory usage and calculate difference
final_memory = process.memory_info().rss
memory_difference = bytes_to_gb(final_memory - initial_memory)

print(f"Memory used by this script: {memory_difference} GB")
