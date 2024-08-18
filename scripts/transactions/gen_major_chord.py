import sys
import arte

ID = 542
NAME = "gen_major_chord"
IN = ["pitch"]
OUT = "chord"

def OnCall(n, callArgs : list) :
    print(f"gen_major_chord:: OnCall [{ID}]")
    for i in range(len(callArgs)) :
        print(f"gen_major_chord:: ({IN[i]}) = {callArgs[i]}")
    base_pitch = callArgs[0][1] # argument 0 i jego wartość, [0][0] - argument 0 i jego TYP
    samples : list = []
    for i in range(n) :
        scale = arte.RunTransaction(ID, "gen_major_scale", 6, [("pitch", base_pitch + i)])
        [sT, sV] = scale # ST typ skali, SV wartości skali
        samples.append([sV[0], sV[2], sV[5]]) # punkty na skali
    print(f"gen_major_chord:: samples {samples}")
    print("\n")
    return samples


def main(args):
    print(f"registering [{ID}] {NAME} transaction ")
    arte.RegisterTransaction(ID, NAME, IN, OUT)

if __name__ == '__main__': main(sys.argv)