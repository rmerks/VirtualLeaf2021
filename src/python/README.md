# svg-to-vl

`svg-to-vl` is a standalone command-line Python package (available on PyPI) that converts a hand-traced SVG tissue outline into a VirtualLeaf XML simulation template ("leaf" file). The SVG is typically created by tracing a microscopy image in a vector-drawing tool such as Inkscape, using different colours for the paths to encode different cell types. The tool maps the traced path coordinates onto nodes and cells in the resulting XML template. `svg-to-vl` streamlines building realistic tissue templates, complementing the previous workflow of hand-editing XML files.


The script facilitates the customisation of multiple parameters, including the designation 
and directory of the template file, which outlines the overarching structure and general 
parameters for the XML file alongside the desired filename for the resulting XML document. 
Additionally, users can specify a scaling factor to appropriately map x and y coordinates 
to the cellular scale and a colour map for encoding cell types within the SVG file. 
Furthermore, specific details for each cell type are provided, delineated by colons and 
their hex colour code, encompassing elements such as cell type and intracellular chemical 
concentrations.

## Prerequisites

- Python >=3.10
- pip
- Inkscape (for tracing the SVG)

Install `svg-to-vl` and its dependencies from PyPI:
```console
pip install svg-to-vl
```

## Usage

```console
svg-to-vl -i "path to the SVG file (without .svg)" -t "path to the XML template file (with file extension)" -s "numerical scaling factor between image and simulation template" -c "color code"
```

The colour code has the form of "RGB colour code, cell type, intracellular species concentrations: RGB colour code, cell type, intracellular species: ... : ... :" until all colour codes are described. 
Default colour code is "ffffff,1,2.251808,0.481961:0000f8,2,2.251808,0.481961:009000,3,2.251808,0.481961:ff0000,3,2.251808,0.481961"