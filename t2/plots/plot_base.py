#!/usr/bin/env python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os, sys

RESULTS = os.path.join(os.path.dirname(__file__), '..', 'results')
df = pd.read_csv(os.path.join(RESULTS, 'base_results.csv'))

fig, axes = plt.subplots(2, 2, figsize=(12, 10))
configs = ['A', 'B', 'C', 'D']
titles = [
    'A: Insercion aleatoria, busqueda uniforme',
    'B: Insercion aleatoria, busqueda sesgada',
    'C: Insercion ordenada, busqueda uniforme',
    'D: Insercion ordenada, busqueda sesgada',
]

for ax, config, title in zip(axes.flat, configs, titles):
    sub = df[df['config'] == config]
    ax.plot(sub['N'], sub['avl_search_ms'], 'o-', label='AVL (search)')
    ax.plot(sub['N'], sub['splay_search_ms'], 's-', label='Splay (search)')
    ax.set_xscale('log', base=2)
    ax.set_yscale('log')
    ax.set_title(title)
    ax.set_xlabel('N')
    ax.set_ylabel('Tiempo de busqueda (ms)')
    ax.legend()
    ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig(os.path.join(RESULTS, 'base_scenarios.png'), dpi=150)
plt.close()

try:
    pdf = pd.read_csv(os.path.join(RESULTS, 'base_per_search.csv'))
    Nmax = pdf['N'].max()
    subset = pdf[pdf['N'] == Nmax]

    fig2, axes2 = plt.subplots(2, 2, figsize=(12, 10))

    for ax, config, title in zip(axes2.flat, configs, titles):
        sub = subset[subset['config'] == config]
        window = 1000
        avl_ma = sub['avl_time_ns'].rolling(window=window, min_periods=1).mean()
        splay_ma = sub['splay_time_ns'].rolling(window=window, min_periods=1).mean()

        ax.plot(sub['step'], avl_ma, linewidth=0.5, alpha=0.8, label='AVL (MA)')
        ax.plot(sub['step'], splay_ma, linewidth=0.5, alpha=0.8, label='Splay (MA)')
        ax.set_xlabel('Step')
        ax.set_ylabel('Tiempo (ns, media movil w=' + str(window) + ')')
        ax.set_title(f'{title}  (N={int(Nmax)})')
        ax.legend()
        ax.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(os.path.join(RESULTS, 'base_per_search.png'), dpi=150)
    plt.close()
    print(f'base_per_search.png generated')
except FileNotFoundError:
    print('base_per_search.csv not found, skipping per-search graph')

print('base_scenarios.png generated')
