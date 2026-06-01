#!/usr/bin/env python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

RESULTS = os.path.join(os.path.dirname(__file__), '..', 'results')
try:
    df = pd.read_csv(os.path.join(RESULTS, 'traversal_results.csv'))
except FileNotFoundError:
    print('traversal_results.csv not found, skipping.')
    sys.exit(0)

N = 33554432
total_ms = df['search_time_ms'].sum()

fig, ax = plt.subplots(figsize=(10, 5))

step_subset = df['step']
time_subset = df['search_time_ms']

ax.plot(step_subset, time_subset, linewidth=0.3, alpha=0.6, label='Por busqueda')

window = max(1, len(df) // 200)
ma = df['search_time_ms'].rolling(window=window, min_periods=1).mean()
ax.plot(df['step'], ma, linewidth=1.0, label=f'Media movil (w={window})')

ax.set_xlabel('Step')
ax.set_ylabel('Tiempo (ms)')
ax.set_title(f'Traversal Conjecture  (total={total_ms:.2f} ms)')
ax.legend()
ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig(os.path.join(RESULTS, 'traversal.png'), dpi=150)
plt.close()

print(f'traversal.png generated (total={total_ms:.2f} ms)')
print(f'O(n) = {N}, ratio = {total_ms / N:.6f} ms/element')
