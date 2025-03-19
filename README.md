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

Look into config.yaml and set solver parameters:

```
solver:
  t: 6.28
  dt: 0.01
  theta: 1.0
generator:
  count: 100000
```

```
mpiexec --oversubscribe -n <number-of-cores-on-all-nodes-plus-one> -host <master-node>,<slave-node-ip-1>,<slave-node-ip-2> ./build/bin/cluster-application
```

Here we oversubscribe by one thread because master thread does almost no work.

You also can run single-threaded executable (standalone-application):

```
./build/bin/standalone-application
```

# Benchmark results

For 100k points (dt=0.01, t=2pi, theta=1):
- Standalone application: 8:02.13
- Cluster application (1 main thread + 8 slave threads): 2:39.65

For 10M points (dt=0.01, t=2pi, theta=0.5) one iteration taking ~1:30.00
