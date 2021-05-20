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


def state_to_tuple(d):
    res = np.zeros(36)
    if d["game_state"]["room_phase"] == "COMBAT":
        res[0] = 0
    else:
        res[0] = 1
    if res[0] == 0:
        res[1] = d["game_state"]["combat_state"]["player"]["energy"]
        res[2] = d["game_state"]["combat_state"]["player"]["current_hp"]
        res[3] = d["game_state"]["combat_state"]["player"]["block"]
        res[4] = 0  # type of mob
        res[5] = d["game_state"]["combat_state"]["monsters"][0]["current_hp"]
        res[6] = d["game_state"]["combat_state"]["monsters"][0]["block"]
        res[7] = d["game_state"]["combat_state"]["monsters"][0]["move_id"]
        for i in d["game_state"]["combat_state"]["monsters"][0]["powers"]:
            if i["id"] == "Vulnerable":
                res[8] = i["amount"]
            elif i["id"] == "Strength":
                res[10] = i["amount"]
        for i in range(len(d["game_state"]["combat_state"]["hand"])):
            if d["game_state"]["combat_state"]["hand"][i]["id"] == "Strike_R":
                res[11 + i] = 0
            elif d["game_state"]["combat_state"]["hand"][i]["id"] == "Defend_R":
                res[11 + i] = 1
            elif d["game_state"]["combat_state"]["hand"][i]["id"] == "Bash":
                res[11 + i] = 2
        for i in range(len(d["game_state"]["combat_state"]["draw_pile"])):
            if d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Strike_R":
                res[16 + i] = 0
            elif d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Defend_R":
                res[16 + i] = 1
            elif d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Bash":
                res[16 + i] = 2
        for i in range(len(d["game_state"]["combat_state"]["discard_pile"])):
            if d["game_state"]["combat_state"]["discard_pile"][i]["id"] == "Strike_R":
                res[26 + i] = 0
            elif d["game_state"]["combat_state"]["discard_pile"][i]["id"] == "Defend_R":
                res[26 + i] = 1
            elif d["game_state"]["combat_state"]["discard_pile"][i]["id"] == "Bash":
                res[26 + i] = 2
    return res


def get_action(state, n_actions, epsilon=0):
    """
    sample actions with epsilon-greedy policy
    recap: with p = epsilon pick random action, else pick action with highest Q(s,a)
    """
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


network = pickle.load(open(PATH + "/DQLAgent.sav", "rb"))


def generate_session(state0, t_max=1000, epsilon=0):
    """play env with approximate q-learning agent and train it at the same time"""
    state = state0
    l_state = state_to_tuple(state)
    possible_actions = get_possible_actions(state)

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
    if state["game_state"]["combat_state"]["monsters"][0]["id"] != "JawWorm":
        time.sleep(2)
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