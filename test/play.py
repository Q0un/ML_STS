import json
import torch, torch.nn as nn
import numpy as np
import torch.nn.functional as F
from torch.autograd import Variable
import random
import pickle
import sys
import time

INPUT_NEURS = 21


def state_to_tuple(d, input_size):
    """Конвертирует JSON состояние в вектор признаков (с нормализацией)"""
    res = np.zeros(input_size)
    if d["game_state"] == 0:
        # Нормализуем все признаки в диапазон ~0-1
        res[0] = d["energy"] / 3.0           # энергия: 0-3 → 0-1
        res[1] = d["player"]["hp"] / 80.0    # HP игрока: 0-80 → 0-1
        res[2] = d["player"]["def"] / 10.0   # блок: 0-10 → 0-1
        last = 3
        for i in range(len(d["hand"])):
            res[last + i] = d["hand"][i] / 2.0  # card_id: 0-2 → 0-1
        last += len(d["hand"])
        for i in range(len(d["pool"])):
            res[last + i] = d["pool"][i] / 2.0
        last += len(d["pool"])
        for i in range(len(d["mobs"])):
            res[last] = d["mobs"][i]["type"] / 5.0       # тип моба
            res[last + 1] = d["mobs"][i]["hp"] / 60.0     # HP моба: 0-60 → 0-1
            res[last + 2] = d["mobs"][i]["def"] / 10.0    # блок моба
            res[last + 3] = d["mobs"][i]["move"] / 5.0    # move_id
            res[last + 4] = d["mobs"][i]["effects"][0] / 5.0  # Vulnerable
            res[last + 5] = d["mobs"][i]["effects"][1] / 5.0  # Weak
            res[last + 6] = d["mobs"][i]["effects"][3] / 5.0  # Strength
            res[last + 7] = d["mobs"][i]["effects"][4] / 5.0  # Ritual
            res[last + 8] = d["mobs"][i]["effects"][5] / 10.0 # CurlUp
            last += 9

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


network = nn.Sequential()


def generate_session(t_max=1000, epsilon=0, train=False):
    global network
    total_reward = 0
    print(-1)
    total_reward = float(input())
    state = json.loads(input())
    possible_actions = json.loads(input())

    mob_set = random.randint(0, 1)
    mob_name = ""
    if mob_set == 0:
        mob_name = "Jaw Worm"
    elif mob_set == 1:
        mob_name = "Cultist"
    elif mob_set == 2:
        mob_name = "Louses"
    print(mob_name + "!", file=sys.stderr)
    print(mob_set)
    if mob_set == 0:
        network = pickle.load(open("../saved_models/v1_DQLAgent_JawWorm.sav", "rb"))
    elif mob_set == 1:
        network = pickle.load(open("../saved_models/v1_DQLAgent_Cultist.sav", "rb"))
    elif mob_set == 2:
        network = pickle.load(open("../saved_models/v1_DQLAgent_Louses.sav", "rb"))
    input_sizes = {0: 21, 1: 21, 2: 29}
    total_reward += float(input())
    state = json.loads(input())
    l_state = state_to_tuple(state, input_sizes[mob_set])
    possible_actions = json.loads(input())

    for t in range(t_max):
        a = get_action(l_state, len(possible_actions), epsilon=epsilon)
        print(a)
        reward = float(input())
        next_state = json.loads(input())
        l_next_state = state_to_tuple(next_state, input_sizes[mob_set])
        next_possible_actions = json.loads(input())

        done = (l_next_state[0] == 1) or (l_next_state[0] == 2)

        total_reward += reward
        state = next_state
        l_state = l_next_state
        possible_actions = next_possible_actions
        if done:
            break
        else:
            print("Player HP: ", next_state["player"]["hp"], " | ", mob_name + " HP: ", next_state["mobs"][0]["hp"], file=sys.stderr)
        time.sleep(1)

    if state["game_state"] == 1:
        print("Lose :(")
    else:
        print("WIN :) | Player HP: ", state["player"]["hp"], file=sys.stderr)
    return total_reward


generate_session()
print(-2)