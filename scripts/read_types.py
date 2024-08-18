import arte

import yaml
import sys


def main(args):
    with open('typedef.yaml', 'r') as file:
        typedef = yaml.safe_load_all(file)
        for type in typedef :
            print(type)
            arte.RegisterType(type['id'], type['name'])

if __name__ == '__main__':
        main(sys.argv)