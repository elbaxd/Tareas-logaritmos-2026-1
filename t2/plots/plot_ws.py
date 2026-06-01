#!/usr/bin/env python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os, sys

RESULTS = os.path.join(os.path.dirname(__file__), '..', 'results')
csv_path = os.path.join(RESULTS, 'ws_results.csv')
if not os.path.exists(csv_path) or os.path.getsize(csv_path) == 0:
    print('ws_results.csv missing or empty, skipping.')
    sys.exit(0)
df = pd.read_csv(csv_path)

N = 33554432
fig, ax = plt.subplots(figsize=(8, 5))

ax.plot(df['W'], df['avl_search_ms'], 'o-', label='AVL')
ax.plot(df['W'], df['splay_search_ms'], 's-', label='Splay')

c1 = np.mean(df['avl_search_ms']) / np.log2(N)
c2 = df['splay_search_ms'].iloc[-1] / np.log2(df['W'].iloc[-1] + 1)

W_range = np.logspace(np.log10(df['W'].min()), np.log10(df['W'].max()), 100)
ax.plot(W_range, [c1 * np.log2(N)] * len(W_range), '--', alpha=0.5,
        label=f'AVL teorico: {c1:.2e} * log2(N)')
ax.plot(W_range, c2 * np.log2(W_range + 1), '--', alpha=0.5,
        label=f'Splay teorico: {c2:.2e} * log2(W)')

ax.set_xscale('log')
ax.set_xlabel('W (tamano del working set)')
ax.set_ylabel('Tiempo (ms)')
ax.set_title('Working Set Theorem')
ax.legend()
ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig(os.path.join(RESULTS, 'working_set.png'), dpi=150)
plt.close()
print('working_set.png generated')
