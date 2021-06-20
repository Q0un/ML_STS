import json
import torch, torch.nn as nn
import numpy as np
import torch.nn.functional as F
from torch.autograd import Variable
import random
import pickle
import sys
import time
import os

PATH = os.path.dirname(__file__)
INPUT_NEURS = 26


def state_to_tuple(d):
    res = np.zeros(INPUT_NEURS)
    if d["game_state"]["room_phase"] == "COMBAT":
        res[0] = 0
    else:
        res[0] = 1
    if res[0] == 0:
        res[1] = d["game_state"]["combat_state"]["player"]["energy"]
        res[2] = d["game_state"]["combat_state"]["player"]["current_hp"]
        res[3] = d["game_state"]["combat_state"]["player"]["block"]
        res[4] = d["game_state"]["combat_state"]["monsters"][0]["current_hp"]
        res[5] = d["game_state"]["combat_state"]["monsters"][0]["block"]
        res[6] = d["game_state"]["combat_state"]["monsters"][0]["move_id"]
        for i in d["game_state"]["combat_state"]["monsters"][0]["powers"]:
            if i["id"] == "Vulnerable":
                res[7] = i["amount"]
            elif i["id"] == "Strength":
                res[9] = i["amount"]
            elif i["id"] == "Ritual":
                res[10] = i["amount"]
        last = 11
        for i in range(len(d["game_state"]["combat_state"]["hand"])):
            if d["game_state"]["combat_state"]["hand"][i]["id"] == "Strike_R":
                res[last + i] = 0
            elif d["game_state"]["combat_state"]["hand"][i]["id"] == "Defend_R":
                res[last + i] = 1
            elif d["game_state"]["combat_state"]["hand"][i]["id"] == "Bash":
                res[last + i] = 2
        last += 5
        for i in range(len(d["game_state"]["combat_state"]["draw_pile"])):
            if d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Strike_R":
                res[last + i] = 0
            elif d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Defend_R":
                res[last + i] = 1
            elif d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Bash":
                res[last + i] = 2
    return res


def get_action(state, n_actions, epsilon=0):
    state = Variable(torch.FloatTensor(state[None]))
    q_values = network(state).data.cpu().numpy()

    possible_actions = range(n_actions)
    action = None

    if random.random() < epsilon:
        action = random.choice(possible_actions)
    else:
        action = max(possible_actions, key=lambda x: q_values[0][x])

    return action


def get_possible_actions(state):
    acts = list()
    acts.append("end")
    it = 1
    for i in state["game_state"]["combat_state"]["hand"]:
        if i["cost"] <= state["game_state"]["combat_state"]["player"]["energy"]:
            if i["type"] == "ATTACK":
                acts.append("play " + str(it) + " 0")
            else:
                acts.append("play " + str(it))
        it += 1
    return acts


network = nn.Sequential()


def generate_session(state0, t_max=1000, epsilon=0):
    global network
    state = state0
    l_state = state_to_tuple(state)
    possible_actions = get_possible_actions(state)
    room_name = ""
    if state["game_state"]["combat_state"]["monsters"][0]["name"] == "Cultist":
        room_name = "Cultist"
    elif state["game_state"]["combat_state"]["monsters"][0]["name"] == "Jaw Worm":
        room_name = "JawWorm"
    elif state["game_state"]["combat_state"]["monsters"][0]["name"] == "Louse":
        room_name = "2Louse"
    elif "Slime" in state["game_state"]["combat_state"]["monsters"][0]["name"]:
        room_name = "SmallSlimes"
    network = pickle.load(open(PATH + "/DQLAgent_" + room_name + ".sav", "rb"))

    for t in range(t_max):
        time.sleep(1)
        a = get_action(l_state, len(possible_actions), epsilon=epsilon)
        print(possible_actions[a])
        next_state = json.loads(input())
        l_next_state = state_to_tuple(next_state)
        next_possible_actions = get_possible_actions(next_state)

        done = (l_next_state[0] == 1) or (l_next_state[0] == 2)

        state = next_state
        l_state = l_next_state
        possible_actions = next_possible_actions
        if done:
            break


print("ready")
while True:
    state = input()
    print("start Ironclad")
    state = input()
    print("choose 0")
    state = input()
    print("choose 1")
    state = input()
    print("choose 0")
    state = input()
    print("choose 0")
    state = json.loads(input())
    if state["game_state"]["combat_state"]["monsters"][0]["name"] != "Cultist" and \
            state["game_state"]["combat_state"]["monsters"][0]["name"] != "Jaw Worm" and \
            state["game_state"]["combat_state"]["monsters"][0]["name"] != "Louse":
        time.sleep(1)
        print("click Left 1910 10")
        state = input()
        print("click Left 1536 240")
        state = input()
        print("click Left 890 685")
        state = input()
        print("click Left 890 685")
        state = input()
        print("click Left 900 950")
        state = input()
        print("click Left 900 950")
        state = input()
    else:
        generate_session(state)