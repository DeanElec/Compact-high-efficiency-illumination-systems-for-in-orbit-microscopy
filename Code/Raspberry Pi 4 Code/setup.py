from setuptools import setup
from Cython.Build import cythonize

setup(
    name='Image Processing Module',
    ext_modules=cythonize("image_processing.pyx"),
)

