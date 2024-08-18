import arte

ID = 7
NAME = "gen_time"
IN = {("ms", "val")}
OUT = "time"

print(f"starting [{ID}] {NAME} transaction")

arte.RegisterTransaction(ID, NAME, IN, OUT)

ins = arte.ReadInput()

arte.PublishResult(ID, ins['ms'])