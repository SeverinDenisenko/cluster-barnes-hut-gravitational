# Distributed gravitational n-body solver

Solver for gravitational n-body problem using parallel distributed Barnes-Hut algorithm

# Build

For Ubuntu install packages:

```
sudo apt install cmake g++ binutils-dev openssh-server net-tools
```

Select one of the MPI implementations:

```
sudo apt install libopenmpi-dev openmpi-bin
sudo apt install mpich
```

For openmpi on Fedora run:

```
sudo dnf install openmpi openmpi-devel
source /etc/profile.d/modules.sh
module load mpi/openmpi-x86_64
```

Then build:


```
git clone --recurse-submodules https://github.com/SeverinDenisenko/cluster-barnes-hut-gravitational.git
cd cluster-barnes-hut-gravitational
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 8
cd ..
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

Look into config.yaml and set solver parameters.

```
mpiexec --oversubscribe -n <number-of-cores-on-all-nodes-plus-two> -host <master-node>,<slave-node-ip-1>,<slave-node-ip-2> ./build/bin/cluster-application
```

For 8-threaded CPU:

```
mpirun --oversubscribe -n 10 ./build/bin/cluster-application
```

Here we oversubscribe by two threads because master thread and frontend does almost no work.

# Benchmark results

For 100k points (dt=0.01, t=2pi, theta=1):
- Cluster application (1 main thread + 8 slave threads): 2:39.65

For 10M points (dt=0.01, t=2pi, theta=0.5) one iteration taking ~1:30.00

Todo
- Optimize cluster communication
- Optimize tree reduction
