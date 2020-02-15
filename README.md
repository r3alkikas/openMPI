# openMPI
openMPI samples to run in Rocks Cluster 7.0 Manzanita

* COMPILE 
          - mpic++ prog.cpp -o prog 
          or
          - mpic++ prog.c -o prog
          
- RUN
      - mpirun -machinefile machinefile -np 10 -v prog
      or
      - mpiexec -machinefile machinefile -np 10 -v prog
