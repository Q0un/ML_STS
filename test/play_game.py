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
    if res[0] == 0:
        res[0] = d["game_state"]["combat_state"]["player"]["energy"] / 3.0
        res[1] = d["game_state"]["combat_state"]["player"]["current_hp"] / 80.0
        res[2] = d["game_state"]["combat_state"]["player"]["block"] / 10.0
        last = 3
        for i in range(len(d["game_state"]["combat_state"]["hand"])):
            if d["game_state"]["combat_state"]["hand"][i]["id"] == "Strike_R":
                res[last + i] = 0 / 2.0
            elif d["game_state"]["combat_state"]["hand"][i]["id"] == "Defend_R":
                res[last + i] = 1 / 2.0
            elif d["game_state"]["combat_state"]["hand"][i]["id"] == "Bash":
                res[last + i] = 2 / 2.0
        last += len(d["game_state"]["combat_state"]["hand"])
        for i in range(len(d["game_state"]["combat_state"]["draw_pile"])):
            if d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Strike_R":
                res[last + i] = 0 / 2.0
            elif d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Defend_R":
                res[last + i] = 1 / 2.0
            elif d["game_state"]["combat_state"]["draw_pile"][i]["id"] == "Bash":
                res[last + i] = 2 / 2.0
        last += len(d["game_state"]["combat_state"]["draw_pile"])
        for i in range(len(d["game_state"]["combat_state"]["monsters"])):
            if d["game_state"]["combat_state"]["monsters"][i]["id"] == "JawWorm":
                res[last] = 0 / 5.0
            elif d["game_state"]["combat_state"]["monsters"][i]["id"] == "Cultist":
                res[last] = 1 / 5.0
            elif d["game_state"]["combat_state"]["monsters"][i]["id"] == "FuzzyLouseNormal":
                res[last] = 2 / 5.0
            elif d["game_state"]["combat_state"]["monsters"][i]["id"] == "FuzzyLouseDefensive":
                res[last] = 3 / 5.0
            res[last] = d["game_state"]["combat_state"]["monsters"][i]["current_hp"] / 60.0
            res[last + 1] = d["game_state"]["combat_state"]["monsters"][i]["block"] / 10.0
            res[last + 2] = d["game_state"]["combat_state"]["monsters"][i]["move_id"] / 5.0
            for j in d["game_state"]["combat_state"]["monsters"][i]["powers"]:
                if j["id"] == "Vulnerable":
                    res[last + 3] = j["amount"] / 5.0
                elif j["id"] == "Weak":
                    res[last + 4] = j["amount"] / 5.0
                elif j["id"] == "Strength":
                    res[last + 5] = j["amount"] / 5.0
                elif j["id"] == "Ritual":
                    res[last + 6] = j["amount"] / 5.0
                elif j["id"] == "CurlUp":
                    res[last + 7] = j["amount"] / 10.0
            last += 8
        
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

model_by_mob = {
    "Cultist": PATH + "../saved_models/v2_NoisyDQN_Cultist.pt",
    "JawWorm": PATH + "../saved_models/v2_NoisyDQN_JawWorm.pt",
    "FuzzyLouseNormal": PATH + "../saved_models/v2_NoisyDQN_Louses.pt",
    "FuzzyLouseDefensive": PATH + "../saved_models/v2_NoisyDQN_Louses.pt",
}

def generate_session(state0, t_max=1000, epsilon=0):
    global network
    state = state0
    l_state = state_to_tuple(state)
    possible_actions = get_possible_actions(state)
    mob_name = state["game_state"]["combat_state"]["monsters"][0]["id"]
    network = pickle.load(open(model_by_mob[mob_name], "rb"))

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
    if state["game_state"]["combat_state"]["monsters"][0]["id"] in model_by_mob.keys():
        generate_session(state)