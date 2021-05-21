import json
import torch, torch.nn as nn
import numpy as np
import torch.nn.functional as F
from torch.autograd import Variable
import random
import pickle
import sys
import time

INPUT_NEURS = 27


def state_to_tuple(d):
    res = np.zeros(INPUT_NEURS)
    res[0] = d["game_state"]
    if d["game_state"] == 0:
        res[1] = d["energy"]
        res[2] = d["player"]["hp"]
        res[3] = d["player"]["def"]
        res[4] = d["mobs"][0]["type"]
        res[5] = d["mobs"][0]["hp"]
        res[6] = d["mobs"][0]["def"]
        res[7] = d["mobs"][0]["move"]
        res[8] = d["mobs"][0]["effects"][0]
        res[9] = d["mobs"][0]["effects"][1]
        res[10] = d["mobs"][0]["effects"][2]
        res[11] = d["mobs"][0]["effects"][3]
        last = 12
        for i in range(len(d["hand"])):
            res[last + i] = d["hand"][i]
        last += 5
        for i in range(len(d["pool"])):
            res[last + i] = d["pool"][i]
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


network = nn.Sequential()


def generate_session(t_max=1000, epsilon=0, train=False):
    global network
    """play env with approximate q-learning agent and train it at the same time"""
    total_reward = 0
    print(-1)
    total_reward = float(input())
    state = json.loads(input())
    l_state = state_to_tuple(state)
    possible_actions = json.loads(input())

    mob_set = random.randint(0, 1)
    print(mob_set)
    if mob_set == 0:
        network = pickle.load(open("DQLAgent_JawWorm.sav", "rb"))
    elif mob_set == 1:
        network = pickle.load(open("DQLAgent_Cultist.sav", "rb"))
    total_reward += float(input())
    state = json.loads(input())
    l_state = state_to_tuple(state)
    possible_actions = json.loads(input())

    for t in range(t_max):
        a = get_action(l_state, len(possible_actions), epsilon=epsilon)
        print(a)
        reward = float(input())
        next_state = json.loads(input())
        l_next_state = state_to_tuple(next_state)
        next_possible_actions = json.loads(input())

        done = (l_next_state[0] == 1) or (l_next_state[0] == 2)

        total_reward += reward
        state = next_state
        l_state = l_next_state
        possible_actions = next_possible_actions
        if done:
            break
        else:
            print("Player HP: ", next_state["player"]["hp"], " | ", "Mob HP: ", next_state["mobs"][0]["hp"], file=sys.stderr)
        time.sleep(1)

    if state["game_state"] == 1:
        print("Lose :(")
    else:
        print("WIN :) | Player HP: ", state["player"]["hp"], file=sys.stderr)
    return total_reward


generate_session()
print(-2)