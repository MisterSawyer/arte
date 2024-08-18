import sys
import arte

import random

ID = 69
NAME = "gen_rand"
IN = ["val"]
OUT = "val"

def OnCall(n, callArgs) :
    print(f"gen_rand:: OnCall [{ID}] {NAME}")
    for i in range(len(callArgs)) :
        print(f"gen_rand:: ({IN[i]}) = {callArgs[i]}")
    [ARG0T, ARG0V] = callArgs[0]
    random.seed(ARG0V)
    samples = [random.random() for i in range(n)]
    print(f"gen_rand:: samples {samples}")
    print("\n")

    return samples


def main(args):
    print(f"registering [{ID}] {NAME} transaction ")
    arte.RegisterTransaction(ID, NAME, IN, OUT)

if __name__ == '__main__': main(sys.argv)

