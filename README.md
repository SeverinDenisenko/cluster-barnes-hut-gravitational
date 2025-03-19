# Distributed gravitational n-body solver

Solver for gravitational n-body problem using parallel distributed Barnes-Hut algorithm

# Build

For Ubuntu install packages:

```
sudo apt install cmake g++ mpich binutils-dev openssh-server net-tools
```

```
git clone --recurse-submodules https://github.com/SeverinDenisenko/cluster-barnes-hut-gravitational.git
cd cluster-barnes-hut-gravitational
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 8
cd ..
```

If you have any problem with project configuration, delete all other MPI implemetations:

```
sudo apt remove libopenmpi-dev openmpi-bin
```

# Configure cluster

On all nodes:

```
sudo adduser mpiuser
sudo usermod -aG sudo mpiuser
```

On master node:

```
su mpiuser
ssh-keygen
ssh-copy-id mpiuser@<slave-node-ip-1>
ssh-copy-id mpiuser@<slave-node-ip-2>
```

# Run cluster

```
mpiexec --oversubscribe -n <number-of-cores-on-nodes> -host <master-node>,<slave-node-ip-1>,<slave-node-ip-2> ./build/bin/cluster-application
```

# Benchmark results

For 100k points (dt=0.01, t=2pi, theta=1):
- Standalone application: 553.12s user 0.33s system 99% cpu 9:17.43 total
- Cluster application (8 threads on one instance): 1551.35s user 5.28s system 705% cpu 3:40.79 total

For 10M points (dt=0.01, t=2pi, theta=0.5) one iteration taking ~1:30.00
