import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import pybind11

ext_modules = [
    Extension(
        'monero_cpp',
        ['monero_cpp/bindings.cpp'],
        include_dirs=[
            pybind11.get_include(),
            # Include monero-cpp headers; adjust the path as needed.
            os.path.join(os.path.dirname(__file__), 'monero-cpp', 'include')
        ],
        language='c++',
        extra_compile_args=['-std=c++11'],
    ),
]

setup(
    name='monero_cpp',
    version='0.1.0',
    author='Don Collins',
    author_email='doncollins1985@gmail.com',
    description='Python bindings for monero-cpp library using pybind11',
    long_description='A Pythonic interface for monero-cpp enabling wallet management, transactions, and daemon interactions.',
    ext_modules=ext_modules,
    install_requires=[
        'pybind11>=2.6'
    ],
    cmdclass={'build_ext': build_ext},
    classifiers=[
        "Programming Language :: Python :: 3",
        "Programming Language :: C++",
        "Operating System :: OS Independent",
    ],
    zip_safe=False,
)

