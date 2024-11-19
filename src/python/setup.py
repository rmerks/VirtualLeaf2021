import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="svg-to-vl",
    version="2.0.2",
    author="Richard van Nieuwenhoven, Ruth Großeholz, Roeland R.M.H. Merks",
    author_email="richard.nieuwenhoven@tuwien.ac.at, ruth.grosseholz@maastrichtuniversity.nl, merksrmh@math.leidenuniv.nl",
    description="converting cells drawn in svg files to VirtualLeaf xml start files. The svg file should be specified without extension, the resulting xml file will be stored next to the svg file.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/rmerks/VirtualLeaf2021/tree/v2.0.0/src/python",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.1',
    install_requires=[
        'shapely>=1.8.0', 
        'sympy>=1.9'
    ],
    keywords='virtualleaf svg conversion',
    project_urls={
        'Homepage': 'https://github.com/rmerks/VirtualLeaf2021',
    },
    entry_points={
        'console_scripts': [
            'svg_to_vl=svg_to_vl.__main__:main',
            'svg-to-vl=svg_to_vl.__main__:main',
        ],
    },
)
