import json
import numpy as np
from collections import defaultdict
import random
import math
import numpy as np
import gym
import matplotlib.pyplot as plt
from IPython.display import clear_output


class QLearningAgent:
    def __init__(self, alpha, epsilon, discount):
        """
        Q-Learning Agent
        based on https://inst.eecs.berkeley.edu/~cs188/sp19/projects.html
        Instance variables you have access to
          - self.epsilon (exploration prob)
          - self.alpha (learning rate)
          - self.discount (discount rate aka gamma)

        Functions you should use
          - self.get_legal_actions(state) {state, hashable -> list of actions, each is hashable}
            which returns legal actions for a state
          - self.get_qvalue(state,action)
            which returns Q(state,action)
          - self.set_qvalue(state,action,value)
            which sets Q(state,action) := value
        !!!Important!!!
        Note: please avoid using self._qValues directly.
            There's a special self.get_qvalue/set_qvalue for that.
        """

        self._qvalues = defaultdict(lambda: defaultdict(lambda: 0))
        self.alpha = alpha
        self.epsilon = epsilon
        self.discount = discount

    def get_qvalue(self, state, action):
        """ Returns Q(state,action) """
        return self._qvalues[state][action]

    def set_qvalue(self, state, action, value):
        """ Sets the Qvalue for [state,action] to the given value """
        self._qvalues[state][action] = value

    def get_value(self, state, possible_actions):
        """
        Compute your agent's estimate of V(s) using current q-values
        V(s) = max_over_action Q(state,action) over possible actions.
        Note: please take into account that q-values can be negative.
        """

        # If there are no legal actions, return 0.0
        if len(possible_actions) == 0:
            return 0.0

        act = max(possible_actions, key=lambda x: self.get_qvalue(state, x))
        value = self.get_qvalue(state, act)

        return value

    def update(self, state, action, reward, next_state, next_possible_actions):
        """
        You should do your Q-Value update here:
           Q(s,a) := (1 - alpha) * Q(s,a) + alpha * (r + gamma * V(s'))
        """

        # agent parameters
        gamma = self.discount
        learning_rate = self.alpha

        qval = (1 - self.alpha) * self.get_qvalue(state, action) + self.alpha * (reward + gamma * self.get_value(next_state, next_possible_actions))

        self.set_qvalue(state, action, qval)

    def get_best_action(self, state, possible_actions):
        """
        Compute the best action to take in a state (using current q-values).
        """

        # If there are no legal actions, return None
        if len(possible_actions) == 0:
            return None

        best_action = max(possible_actions, key=lambda x: self.get_qvalue(state, x))

        return best_action

    def get_action(self, state, possible_actions):
        """
        Compute the action to take in the current state, including exploration.
        With probability self.epsilon, we should take a random action.
            otherwise - the best policy action (self.get_best_action).

        Note: To pick randomly from a list, use random.choice(list).
              To pick True or False with a given probablity, generate uniform number in [0, 1]
              and compare it with your probability
        """

        # Pick Action
        action = None

        # If there are no legal actions, return None
        if len(possible_actions) == 0:
            return None

        # agent parameters:
        epsilon = self.epsilon

        if random.random() < epsilon:
            action = random.choice(possible_actions)
        else:
            action = self.get_best_action(state, possible_actions)

        return action


agent = QLearningAgent(alpha=0.5, epsilon=0.25, discount=0.9)


def state_to_tuple(d):
    res = []
    if isinstance(d, dict):
        for i in d.values():
            res.append(state_to_tuple(i))
    elif isinstance(d, list):
        for i in d:
            res.append(state_to_tuple(i))
    else:
        res = [d]
    return tuple(res)


def play_and_train(agent, t_max=10**4):
    """
    This function should
    - run a full game, actions given by agent's e-greedy policy
    - train agent using agent.update(...) whenever it is possible
    - return total reward
    """
    print(-1)
    total_reward = float(input())
    state = json.loads(input())
    l_state = state_to_tuple(state)
    possible_actions = json.loads(input())

    for t in range(t_max):
        # get agent to pick action given state s.
        action = agent.get_action(l_state, range(len(possible_actions)))
        print(action)
        reward = float(input())
        next_state = json.loads(input())
        l_next_state = state_to_tuple(next_state)
        next_possible_actions = json.loads(input())

        # train (update) agent for state s
        agent.update(l_state, action, reward, l_next_state, range(len(next_possible_actions)))

        state = next_state
        l_state = l_next_state
        possible_actions = next_possible_actions
        total_reward += reward
        if state["game_state"] == 1 or state["game_state"] == 2:
            break

    return total_reward


from IPython.display import clear_output

logs = open("rewards.log", "w")
rewards = []
for i in range(1000):
    rewards.append(play_and_train(agent))
    agent.epsilon *= 0.99

    if i % 100 == 0:
        print(np.mean(rewards[-10:]), file=logs)

print(-2)