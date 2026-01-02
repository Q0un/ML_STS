import json
import torch
import torch.nn as nn
import torch.nn.functional as F
import numpy as np
from torch.autograd import Variable
import random
import pickle
import sys
import time
import os

PATH = os.path.dirname(__file__)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# Размеры входов для разных мобов
INPUT_SIZES = {
    "JawWorm": 22,
    "Cultist": 22,
    "FuzzyLouseNormal": 31,
    "FuzzyLouseDefensive": 31,
}

OUTPUT_SIZES = {
    "JawWorm": 6,
    "Cultist": 6,
    "FuzzyLouseNormal": 11,
    "FuzzyLouseDefensive": 11,
}

# ============================================================================
# Архитектуры сетей (должны совпадать с train.ipynb)
# ============================================================================

class BaselineMLP(nn.Module):
    def __init__(self, input_size, output_size):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(input_size, 128),
            nn.ReLU(),
            nn.Linear(128, 256),
            nn.ReLU(),
            nn.Linear(256, 128),
            nn.ReLU(),
            nn.Linear(128, output_size)
        )
    
    def forward(self, x):
        return self.net(x)


class NoisyLinear(nn.Module):
    def __init__(self, in_features, out_features, sigma_init=0.5):
        super().__init__()
        self.in_features = in_features
        self.out_features = out_features
        
        self.weight_mu = nn.Parameter(torch.empty(out_features, in_features))
        self.weight_sigma = nn.Parameter(torch.empty(out_features, in_features))
        self.register_buffer('weight_epsilon', torch.empty(out_features, in_features))
        
        self.bias_mu = nn.Parameter(torch.empty(out_features))
        self.bias_sigma = nn.Parameter(torch.empty(out_features))
        self.register_buffer('bias_epsilon', torch.empty(out_features))
        
        self.sigma_init = sigma_init
        self.reset_parameters()
        self.reset_noise()
    
    def reset_parameters(self):
        mu_range = 1 / np.sqrt(self.in_features)
        self.weight_mu.data.uniform_(-mu_range, mu_range)
        self.weight_sigma.data.fill_(self.sigma_init / np.sqrt(self.in_features))
        self.bias_mu.data.uniform_(-mu_range, mu_range)
        self.bias_sigma.data.fill_(self.sigma_init / np.sqrt(self.out_features))
    
    def reset_noise(self):
        epsilon_in = self._scale_noise(self.in_features)
        epsilon_out = self._scale_noise(self.out_features)
        self.weight_epsilon.copy_(epsilon_out.outer(epsilon_in))
        self.bias_epsilon.copy_(epsilon_out)
    
    def _scale_noise(self, size):
        x = torch.randn(size)
        return x.sign().mul_(x.abs().sqrt_())
    
    def forward(self, x):
        if self.training:
            weight = self.weight_mu + self.weight_sigma * self.weight_epsilon
            bias = self.bias_mu + self.bias_sigma * self.bias_epsilon
        else:
            weight = self.weight_mu
            bias = self.bias_mu
        return F.linear(x, weight, bias)


class NoisyDQN(nn.Module):
    def __init__(self, input_size, output_size):
        super().__init__()
        self.fc1 = nn.Linear(input_size, 128)
        self.fc2 = nn.Linear(128, 256)
        self.noisy1 = NoisyLinear(256, 128)
        self.noisy2 = NoisyLinear(128, output_size)
    
    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.relu(self.noisy1(x))
        return self.noisy2(x)
    
    def reset_noise(self):
        self.noisy1.reset_noise()
        self.noisy2.reset_noise()


# ============================================================================
# Конфигурация моделей
# ============================================================================

MODEL_CONFIG = {
    "JawWorm": {
        "path": os.path.join(PATH, "../saved_models/NoisyDQN_JawWorm_v2.pt"),
        "architecture": NoisyDQN,
        "input_size": 22,
        "output_size": 6,
    },
    "Cultist": {
        "path": os.path.join(PATH, "../saved_models/NoisyDQN_Cultist_v2.pt"),
        "architecture": NoisyDQN,
        "input_size": 22,
        "output_size": 6,
    },
    "FuzzyLouseNormal": {
        "path": os.path.join(PATH, "../saved_models/NoisyDQN_Louses_v2.pt"),
        "architecture": NoisyDQN,
        "input_size": 31,
        "output_size": 11,
    },
    "FuzzyLouseDefensive": {
        "path": os.path.join(PATH, "../saved_models/NoisyDQN_Louses_v2.pt"),
        "architecture": NoisyDQN,
        "input_size": 31,
        "output_size": 11,
    },
}


def state_to_tuple(d, input_size):
    """Конвертирует состояние игры в вектор признаков"""
    res = np.zeros(input_size)
    
    combat = d["game_state"]["combat_state"]
    
    res[0] = combat["player"]["energy"] / 3.0
    res[1] = combat["player"]["current_hp"] / 88.0
    res[2] = combat["player"]["block"] / 10.0
    
    last = 3
    
    # Карты в руке
    for i, card in enumerate(combat["hand"]):
        if card["id"] == "Strike_R":
            res[last + i] = 0 / 2.0
        elif card["id"] == "Defend_R":
            res[last + i] = 1 / 2.0
        elif card["id"] == "Bash":
            res[last + i] = 2 / 2.0
    last += len(combat["hand"])
    
    # Колода
    for i, card in enumerate(combat["draw_pile"]):
        if card["id"] == "Strike_R":
            res[last + i] = 0 / 2.0
        elif card["id"] == "Defend_R":
            res[last + i] = 1 / 2.0
        elif card["id"] == "Bash":
            res[last + i] = 2 / 2.0
    last += len(combat["draw_pile"])
    
    # Мобы
    for monster in combat["monsters"]:
        # Тип моба
        mob_type = 0
        if monster["id"] == "JawWorm":
            mob_type = 0
        elif monster["id"] == "Cultist":
            mob_type = 1
        elif monster["id"] == "FuzzyLouseNormal":
            mob_type = 2
        elif monster["id"] == "FuzzyLouseDefensive":
            mob_type = 3
        res[last] = mob_type / 5.0
        
        res[last + 1] = monster["current_hp"] / 60.0
        res[last + 2] = monster["block"] / 10.0
        res[last + 3] = monster.get("move_id", 0) / 5.0
        
        # Эффекты
        for power in monster.get("powers", []):
            if power["id"] == "Vulnerable":
                res[last + 4] = power["amount"] / 5.0
            elif power["id"] == "Weak":
                res[last + 5] = power["amount"] / 5.0
            elif power["id"] == "Strength":
                res[last + 6] = power["amount"] / 5.0
            elif power["id"] == "Ritual":
                res[last + 7] = power["amount"] / 5.0
            elif power["id"] == "CurlUp":
                res[last + 8] = power["amount"] / 10.0
        
        last += 9
    
    return res


def load_model(mob_name):
    """Загружает модель для указанного моба"""
    config = MODEL_CONFIG.get(mob_name)
    if config is None:
        print(f"Unknown mob: {mob_name}", file=sys.stderr)
        return None
    
    # Создаём сеть нужной архитектуры
    network = config["architecture"](
        input_size=config["input_size"],
        output_size=config["output_size"]
    ).to(device)
    
    # Загружаем веса
    try:
        checkpoint = torch.load(config["path"], map_location=device)
        if "policy_net" in checkpoint:
            network.load_state_dict(checkpoint["policy_net"])
        else:
            network.load_state_dict(checkpoint)
        network.eval()  # Переводим в режим inference
        print(f"Loaded model for {mob_name} from {config['path']}", file=sys.stderr)
    except Exception as e:
        print(f"Error loading model: {e}", file=sys.stderr)
        return None
    
    return network


def get_action(network, state, n_actions):
    """Выбор действия"""
    with torch.no_grad():
        state_tensor = torch.FloatTensor(state).unsqueeze(0).to(device)
        q_values = network(state_tensor).cpu().numpy()[0]
        # Выбираем лучшее из доступных действий
        return max(range(n_actions), key=lambda x: q_values[x])


def get_possible_actions(state):
    """Возвращает список возможных действий"""
    acts = ["end"]
    combat = state["game_state"]["combat_state"]
    
    for i, card in enumerate(combat["hand"], 1):
        if card["cost"] <= combat["player"]["energy"]:
            if card["type"] == "ATTACK":
                for j in range(len(combat["monsters"])):
                    acts.append(f"play {i} {j}")
            else:
                acts.append(f"play {i}")
    
    return acts


# Глобальные переменные
network = None
current_input_size = 22


def generate_session(state0, t_max=1000):
    global network, current_input_size
    
    state = state0
    mob_name = state["game_state"]["combat_state"]["monsters"][0]["id"]
    
    # Загружаем модель для этого моба
    network = load_model(mob_name)
    if network is None:
        print("Failed to load model, using random actions", file=sys.stderr)
        return
    
    current_input_size = MODEL_CONFIG[mob_name]["input_size"]
    
    l_state = state_to_tuple(state, current_input_size)
    possible_actions = get_possible_actions(state)

    for t in range(t_max):
        time.sleep(1)
        a = get_action(network, l_state, len(possible_actions))
        print(possible_actions[a])
        
        next_state = json.loads(input())
        l_next_state = state_to_tuple(next_state, current_input_size)
        next_possible_actions = get_possible_actions(next_state)

        # Проверяем конец боя
        combat = next_state.get("game_state", {}).get("combat_state")
        done = combat is None or combat.get("turn", 0) < 0

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
    
    mob_name = state["game_state"]["combat_state"]["monsters"][0]["id"]
    if mob_name in MODEL_CONFIG:
        generate_session(state)
