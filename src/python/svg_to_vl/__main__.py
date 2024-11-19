import os
import argparse
from . import readsvg

def main():
    parser=argparse.ArgumentParser(prog='svg-to-vl',
                    description='converting cells drawn in svg files to VirtualLeaf xml start files. The svg file should be specified without extension, the resulting xml file will be stored next to the svg file.',
                    epilog='For more information see our recent publication.')
    parser.add_argument("-i","--svg-file")
    parser.add_argument("-t","--template-file")
    parser.add_argument("-s","--scale-factor")
    parser.add_argument("-c","--color-map")
    
    #ffffff,1,2,3,4:
    args=parser.parse_args()
    if args.svg_file is None or args.template_file is None:
        parser.print_help()
    else:
        print ("svg-file = ", args.svg_file, end='\n')
        print ("template-file = ", args.template_file, end='\n')
        print ("scale = ", args.scale_factor, end='\n')
        print ("colormap = ", args.color_map, end='\n')
        readsvg.convertSVG(args.svg_file, args.template_file, args.scale_factor, args.color_map)

if __name__ == "__main__":
    main()
    