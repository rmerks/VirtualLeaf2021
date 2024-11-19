#Enhanced cell wall mechanics in VirtualLeaf enable realistic simulations of plant tissue dynamics

Computational modelling has become increasingly important in advancing our understanding of biological systems, necessitating the development of new computational approaches and software. VirtualLeaf, in particular, is a modelling framework for plant tissues that accounts for the biophysical mechanics of plant cell interactions. The plant cell wall plays a pivotal role in plant development and survival, with younger cells generally having thinner, more flexible (primary) walls than older cells. Signalling processes in growth and pathogen infection also affect cell wall stability.  This article presents an updated version of VirtualLeaf with improved cell wall mechanics and morphing behaviour. These are crucial for ultimately understanding plant tissue dynamics and essential signalling processes during growth, tissue formation and pathogen defence. The updated version of VirtualLeaf enables detailed modelling of variations in cell wall stability to the level of individual cell wall elements. These improvements lay the groundwork for using VirtualLeaf to address new research questions, including the structural implications of pathogen infection and growth.


The script facilitates the customisation of multiple parameters, including the designation 
and directory of the template file, which outlines the overarching structure and general 
parameters for the XML file alongside the desired filename for the resulting XML document. 
Additionally, users can specify a scaling factor to appropriately map x and y coordinates 
to the cellular scale and a colour map for encoding cell types within the SVG file. 
Furthermore, specific details for each cell type are provided, delineated by colons and 
their hex colour code, encompassing elements such as cell type and intracellular chemical 
concentrations.

>python readsvg.py -i "path to the SVG file (without .svg)" -t "path to the XML template file (with file extension)" -s "numerical scaling factor between image and simulation template" -c “color code”

The colour code has the form of "RGB colour code, cell type, intracellular species concentrations: RGB colour code, cell type, intracellular species: ... : ... :" until all colour codes are described. 
Default colour code is "ffffff,1,2.251808,0.481961:0000f8,2,2.251808,0.481961:009000,3,2.251808,0.481961:ff0000,3,2.251808,0.481961"