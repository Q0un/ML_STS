import json
import torch, torch.nn as nn
import numpy as np
import torch.nn.functional as F
from torch.autograd import Variable
import random
import pickle
import sys
import matplotlib.pyplot as plt

INPUT_NEURS = 26
OUTPUT_NEURS = 16


def state_to_tuple(d):
    res = np.zeros(INPUT_NEURS)
    res[0] = d["game_state"]
    if d["game_state"] == 0:
        res[1] = d["energy"]
        res[2] = d["player"]["hp"]
        res[3] = d["player"]["def"]
        res[4] = d["mobs"][0]["hp"]
        res[5] = d["mobs"][0]["def"]
        res[6] = d["mobs"][0]["move"]
        res[7] = d["mobs"][0]["effects"][0]
        res[8] = d["mobs"][0]["effects"][1]
        res[8] = d["mobs"][0]["effects"][2]
        res[10] = d["mobs"][0]["effects"][3]
        last = 11
        for i in range(len(d["hand"])):
            res[last + i] = d["hand"][i]
        last += 5
        for i in range(len(d["pool"])):
            res[last + i] = d["pool"][i]
    return res


network = nn.Sequential(
    nn.Linear(INPUT_NEURS, 128),
    nn.ReLU(),
    nn.Linear(128, 256),
    nn.ReLU(),
    nn.Linear(256, 128),
    nn.ReLU(),
    nn.Linear(128, OUTPUT_NEURS)
)


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


def to_one_hot(y, n_dims=None):
    y_tensor = y.data if isinstance(y, Variable) else y
    y_tensor = y_tensor.type(torch.LongTensor).view(-1, 1)
    n_dims = n_dims if n_dims is not None else int(torch.max(y_tensor)) + 1
    y_one_hot = torch.zeros(y_tensor.size()[0], n_dims).scatter_(1, y_tensor, 1)
    return Variable(y_one_hot) if isinstance(y, Variable) else y_one_hot

def where(cond, x_1, x_2):
    return (cond * x_1) + ((1-cond) * x_2)


def compute_td_loss(states, actions, rewards, next_states, is_done, gamma = 0.9, check_shapes = False):
    states = Variable(torch.FloatTensor(states))    # shape: [batch_size, state_size]
    actions = Variable(torch.IntTensor(actions))    # shape: [batch_size]
    rewards = Variable(torch.FloatTensor(rewards))  # shape: [batch_size]
    next_states = Variable(torch.FloatTensor(next_states)) # shape: [batch_size, state_size]
    is_done = Variable(torch.FloatTensor(is_done))  # shape: [batch_size]

    #get q-values for all actions in current states (use network)
    predicted_qvalues = network(states)

    #select q-values for chosen actions
    predicted_qvalues_for_actions = torch.sum(predicted_qvalues * to_one_hot(actions, OUTPUT_NEURS), dim=1)

    # compute q-values for all actions in next states (use network)
    predicted_next_qvalues = network(next_states)

    # compute V*(next_states) using predicted next q-values
    next_state_values = torch.max(predicted_next_qvalues, dim = 1)[0]
    assert isinstance(next_state_values.data, torch.FloatTensor)

    # compute "target q-values" for loss - it's what's inside square parentheses in the above formula.
    target_qvalues_for_actions = rewards + gamma * next_state_values

    # at the last state we shall use simplified formula: Q(s,a) = r(s,a) since s' doesn't exist
    target_qvalues_for_actions = where(is_done, rewards, target_qvalues_for_actions)

    #mean squared error loss to minimize
    loss = F.mse_loss(predicted_qvalues_for_actions, target_qvalues_for_actions.detach())

    if check_shapes and False:
        assert predicted_next_qvalues.data.dim() == 2, "make sure you predicted q-values for all actions in next state"
        assert next_state_values.data.dim() == 1, "make sure you computed V(s') as maximum over just the actions axis and not all axes"
        assert target_qvalues_for_actions.data.dim() == 1, "there's something wrong with target q-values, they must be a vector"

    return loss


opt = torch.optim.Adam(network.parameters(), lr=1e-4)   #create an optim
epsilon = 0.2 # set default epsilon


def generate_session(t_max=1000, epsilon=0, train=False):
    total_reward = 0
    print(-1)
    total_reward = float(input())
    state = json.loads(input())
    l_state = state_to_tuple(state)
    possible_actions = json.loads(input())

    if sys.argv[1] == "JawWorm":
        print(0)
    elif sys.argv[1] == "Cultist":
        print(1)
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

        if train:
            opt.zero_grad()
            compute_td_loss([l_state], [a], [reward], [l_next_state], [done]).backward()
            opt.step()

        total_reward += reward
        state = next_state
        l_state = l_next_state
        possible_actions = next_possible_actions
        if done: break

    return total_reward

from IPython.display import clear_output

logs = open("rewards.log", "w")

rewards = []

for i in range(300):
    session_rewards = [generate_session(epsilon=epsilon, train=True) for _ in range(100)]#play some sessions (generate_session)
    rewards.append(np.mean(session_rewards))
    print("epoch #{}\tmean reward = {:.3f}\tepsilon = {:.3f}".format(i, np.mean(session_rewards), epsilon), file=logs)
    print("epoch #{}\tmean reward = {:.3f}\tepsilon = {:.3f}".format(i, np.mean(session_rewards), epsilon), file=sys.stderr)
    logs.flush()

    epsilon *= 0.99 #reduce exploration coef over time
    if sys.argv[1] == "JawWorm":
        if epsilon < 0.01:
            epsilon = 0.03
        #if np.mean(session_rewards) >= 10:
        #    break
    elif sys.argv[1] == "Cultist":
        if epsilon < 0.04:
            epsilon = 0.07
        if np.mean(session_rewards) >= 20:
            break

print(-2)

plt.plot(rewards)
plt.title("Средняя награда в битве с " + sys.argv[1])
plt.xlabel("Номер эпохи(по 100 игр)")
plt.ylabel("Награда")
plt.show()

pickle.dump(network, open("newAgent_" + sys.argv[1] + ".sav", "wb"))