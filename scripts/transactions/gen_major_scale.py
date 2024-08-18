import sys
import arte

ID = 987
NAME = "gen_major_scale"
IN = ["pitch"]
OUT = "major_scale"

def OnCall(n, callArgs) :
    print(f"gen_major_scale:: OnCall [{ID}]")
    for i in range(len(callArgs)) :
        print(f"gen_major_scale:: ({IN[i]}) = {callArgs[i]}")
    [ARG0T, ARG0V] = callArgs[0]
    pitch_samples = arte.RunTransaction(ID, "gen_pitch", 2*n, [callArgs[0]])
    samples = [pitch_samples[1][i] for i in range(len(pitch_samples[1])) if i % 2 == 0]
    print(f"gen_major_scale:: samples {samples}")
    print("\n")
    return samples


def main(args):
    print(f"registering [{ID}] {NAME} transaction ")
    arte.RegisterTransaction(ID, NAME, IN, OUT)

if __name__ == '__main__': main(sys.argv)