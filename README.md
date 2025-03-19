# Distributed gravitational n-body solver

Solver for gravitational n-body problem using parallel distributed Barnes-Hut algorithm

# Benchmark results

For 100k points, dt=0.01, t=2pi, theta=1:
- Standalone application: 553.12s user 0.33s system 99% cpu 9:17.43 total
- Cluster application (8 threads on one instance): 1551.35s user 5.28s system 705% cpu 3:40.79 total
