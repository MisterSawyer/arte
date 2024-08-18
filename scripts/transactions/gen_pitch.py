import sys
import arte

ID = 98
NAME = "gen_pitch"
IN = ["pitch"]
OUT = "pitch"

def OnCall(n, callArgs) :
    print(f"gen_pitch:: OnCall [{ID}]")
    for i in range(len(callArgs)) :
        print(f"gen_major_chord:: ({IN[i]}) = {callArgs[i]}")
    [ARG0T, ARG0V] = callArgs[0]
    samples = [ARG0V + x for x in range(n)]
    print(f"gen_pitch:: samples {samples}")
    print("\n")

    return samples

def main(args):
    print(f"registering [{ID}] {NAME} transaction ")
    arte.RegisterTransaction(ID, NAME, IN, OUT)

if __name__ == '__main__': main(sys.argv)