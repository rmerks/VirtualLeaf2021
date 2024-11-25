# VirtualLeaf

VirtualLeaf is a cell-based computer modeling framework for simulating plant tissue morphogenesis.

## What is it?

**VirtualLeaf** is a cell-based tissue simulation software written in C++. Simulations proceed in dicreet steps where the system energy is minimized using a Monte-carlo approach. This approach generates realistic cell shapes and emergent tissue dynamics, enabling users to model a variety of developmental proccesses in plants.  
On the most basic level cell shape is defined through a balance of **turgor pressure**, causing expansion, and **cell wall tension**, causing restriction. By considering cell walls as springs that resist the internal pressure a mathematical description of the energy can be obtained. This expression, called the Hamiltonian, can also be extended to include other restrictions that a user may desire (e.g. cell shape).  
VirtualLeaf also comes with capabilities to perform **compartmentalized ODE-based simulations** that can affect the physical characterstics of the cell.  
This makes VirtualLeaf a powerful tool in understanding how plants grow. Try it out! 

## Getting Started

### Dependencies

* Describe any prerequisites, libraries, OS version, etc., needed before installing program.
* ex. Windows 10

### Installing

* How/where to download your program
* Any modifications needed to be made to files/folders

### Executing program

* How to run the program
* Step-by-step bullets
```
code blocks for commands
```

## Help

Any advise for common problems or issues.
```
command to run if program contains helper info
```

## Authors

Contributors names and contact info

ex. Dominique Pizzie  
ex. [@DomPizzie](https://twitter.com/dompizzie)

## Version History

* 0.2
    * Various bug fixes and optimizations
    * See [commit change]() or See [release history]()
* 0.1
    * Initial Release

## License

This project is licensed under the [NAME HERE] License - see the LICENSE.md file for details

## Acknowledgments

Inspiration, code snippets, etc.
* [awesome-readme](https://github.com/matiassingers/awesome-readme)
* [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
* [dbader](https://github.com/dbader/readme-template)
* [zenorocha](https://gist.github.com/zenorocha/4526327)
* [fvcproductions](https://gist.github.com/fvcproductions/1bfc2d4aecb01a834b46)
