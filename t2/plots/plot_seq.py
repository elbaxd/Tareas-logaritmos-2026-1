#!/usr/bin/env python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

RESULTS = os.path.join(os.path.dirname(__file__), '..', 'results')
df = pd.read_csv(os.path.join(RESULTS, 'seq_results.csv'))

N = 33554432
fig, ax = plt.subplots(figsize=(8, 5))

ax.plot(df['m'], df['avl_search_ms'], 'o-', label='AVL')
ax.plot(df['m'], df['splay_search_ms'], 's-', label='Splay')

c1 = df['avl_search_ms'].iloc[-1] / (df['m'].iloc[-1] * np.log2(N))
c2 = df['splay_search_ms'].iloc[-1] / (df['m'].iloc[-1] * np.log2(N / df['m'].iloc[-1] + 1))

m_range = np.linspace(df['m'].min(), df['m'].max(), 100)
ax.plot(m_range, c1 * m_range * np.log2(N), '--', alpha=0.5,
        label=f'AVL teorico: {c1:.2e} * m * log2(N)')
ax.plot(m_range, c2 * m_range * np.log2(N / (m_range + 1)), '--', alpha=0.5,
        label=f'Splay teorico: {c2:.2e} * m * log2(N/m)')

ax.set_xlabel('m (búsquedas)')
ax.set_ylabel('Tiempo (ms)')
ax.set_title('Sequential Access Theorem')
ax.legend()
ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig(os.path.join(RESULTS, 'sequential_access.png'), dpi=150)
plt.close()
print('sequential_access.png generated')
