import subprocess
import os
import pathlib
import random

nb_games = 10000
main_strat = "StrategyDummy.dll"
opposing_strat = "Strategy.dll"

# Running the program many times
path = str(pathlib.Path(__file__).parent.absolute()) + "\\bin\\x64\\Release\\";

for i in range(nb_games):
    # Creating the list of parameters based on number of players
    nb_players = random.randint(4, 8)
    array = [main_strat] + [opposing_strat,]*(nb_players - 1)
    random.shuffle(array)
    array = list(zip(["-s",]*nb_players, array))

    line = [path+"DiceWars.exe", "-r", "Referee.dll", "-m", "GenMap.dll"]
    for a in array:
        line += [a[0], a[1]]

    list_files = subprocess.run(line)

# Computing winning rate
with open("output.txt", "r") as f:
    data = f.readlines()

data = [int(a[0]) for a in data if a[0] in ['1', '0']]
last_run = [data[-nb_games + i] for i in range(nb_games)]

print("winrate = {} %".format(len([a for a in last_run if a == 1]) / nb_games * 100))
print(last_run)

os.system("pause")
