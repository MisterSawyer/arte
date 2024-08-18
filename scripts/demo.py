import sys
import arte

ID = 1091
NAME = "demo"
IN = []
OUT = "chord"

def OnCall(n, callArgs = []) :
    print(f"OnCall [{ID}] {NAME}")
    for i in range(len(callArgs)) :
        print(f"demo:: ({IN[i]}) = {callArgs[i]}")
    #random_samples = arte.RunTransaction(ID, "gen_rand", n, [("val", 420)])
    #[random_samplesT, random_samplesV] = random_samples
    major_chord = arte.RunTransaction(ID, "gen_major_chord", 2, [("pitch", 0)])
    print(f"major_chord {major_chord}")
    return major_chord

def main(args):
    print(f"registering [{ID}] {NAME} transaction ")
    arte.RegisterTransaction(ID, NAME, IN, OUT)

if __name__ == '__main__': main(sys.argv)

