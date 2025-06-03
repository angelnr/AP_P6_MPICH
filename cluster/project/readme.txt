
Strassens_algorithm Con 1 nodo (ACTUALIZADO)

Strassen distribuido terminó en 10.585825 s

Strassen distribuido terminó en 12.023352 s

Strassen distribuido terminó en 11.531978 s

Strassen distribuido terminó en 11.596553 s

Strassen distribuido terminó en 10.116624 s

Tiempo medio ≈ 11.170866 segundos.


Strassens_algorithm con 4 nodos


Fatal error in internal_Reduce: Message truncated, error stack:
internal_Reduce(150)........................: MPI_Reduce(sendbuf=0x71d87b87ded0, recvbuf=0, count=65536, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) failed
MPIR_Reduce_impl(4171)......................: 
MPIR_Reduce_allcomm_auto(4111)..............: 
MPIR_Reduce_intra_reduce_scatter_gather(214): 
MPIDI_CH3U_Receive_data_found(120)..........: Message from rank 3 and tag 11 truncated; 524288 bytes received but buffer size is 131072





Strassens_algorithm con 4 nodos pero con 2 procesos cada uno

atal error in internal_Reduce: Message truncated, error stack:
internal_Reduce(150)........................: MPI_Reduce(sendbuf=0x71d87b87ded0, recvbuf=0, count=65536, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) failed
MPIR_Reduce_impl(4171)......................: 
MPIR_Reduce_allcomm_auto(4111)..............: 
MPIR_Reduce_intra_reduce_scatter_gather(214): 
MPIDI_CH3U_Receive_data_found(120)..........: Message from rank 3 and tag 11 truncated; 524288 bytes received but buffer size is 131072





Strassens_algorithm con 8 nodos

/project $ mpirun -np 8 ./strassens_algorithm
Warning: Permanently added '172.18.0.15' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.17' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.14' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.16' (ED25519) to the list of known hosts.
Fatal error in internal_Reduce: Other MPI error, error stack:
internal_Reduce(150)........................: MPI_Reduce(sendbuf=0x733ac7e7dcf0, recvbuf=0, count=65536, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD) failed
MPIR_Reduce_impl(4171)......................: 
MPIR_Reduce_allcomm_auto(4111)..............: 
MPIR_Reduce_intra_reduce_scatter_gather(389): Failure during collective




Strassens_algorithm con 16 nodos

Warning: Permanently added '172.18.0.4' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.8' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.9' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.6' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.7' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.5' (ED25519) to the list of known hosts.
Warning: Permanently added '172.18.0.3' (ED25519) to the list of known hosts.
^C[mpiexec@9e20371c2192] Sending Ctrl-C to processes as requested
[mpiexec@9e20371c2192] Press Ctrl-C again to force abort







________________________________________________________________________________________________________________________________________________________________


Algoritmo de fox con 1 nodo (ACTUALIZADO)


Fox completado en 11.167703 segundos

Fox completado en 11.605527 segundos

Fox completado en 12.887646 segundos

Fox completado en 10.477805 segundos

Fox completado en 11.882009 segundos

Tiempo medio ≈ 11.604138 segundos. 




* **Tiempo más alto:** 12.887646 segundos
* **Tiempo más bajo:** 10.477805 segundos
* **Variación (diferencia entre máximo y mínimo):** 12.887646 - 10.477805 = **2.409841 segundos**






Algoritmo de fox con 4 nodos (ACTUALIZADO)

Fox completado en 1.797718 segundos

Fox completado en 2.094557 segundos

Fox completado en 1.790961 segundos

Fox completado en 2.154105 segundos

Fox completado en 1.793127 segundos
Tiempo medio ≈ 1.926094 segundos.



* **Tiempo más alto:** 2.154105 segundos
* **Tiempo más bajo:** 1. panel\_split\[1] **1.790961** segundos
* **Variación (diferencia entre máximo y mínimo):** 2.154105 - 1.790961 = **0.363144 segundos**








Algoritmo de fox con 4 nodos con 2 procesos: (ACTUALIZADO)

Fox completado en 2.107730 segundos

Fox completado en 2.103069 segundos

Fox completado en 1.910982 segundos

Fox completado en 2.094320 segundos

Fox completado en 1.996795 segundos

tiempo medio 2.042579 segundos.




* **Tiempo más alto:** 2.107730 segundos
* **Tiempo más bajo:** 1.910982 segundos
* **Variación (diferencia entre máximo y mínimo):** 2.107730 - 1.910982 = **0.196748 segundos**






Algoritmo de fox con 16 nodos (ACTUALIZADO)

Fox completado en 1.251741 segundos

Fox completado en 1.201413 segundos

Fox completado en 1.148158 segundos

Fox completado en 1.165931 segundos

Fox completado en 1.178105 segundos

Tiempo medio ≈ 1.189070 segundos.




* **Tiempo más alto:** 1.251741 segundos
* **Tiempo más bajo:** 1.148158 segundos
* **Variación (diferencia entre máximo y mínimo):** 1.251741 - 1.148158 = **0.103583 segundos**






___________________________________________________________________________________________________________________________________________________________________________________________________________


Algoritmo de Cannon con 1 nodo:(ACTUALIZADO)

Hazme el tiempo medio de los siguientes valores 

Cannon completado en 11.598458 segundos

Cannon completado en 11.789626 segundos

Cannon completado en 11.508195 segundos

Cannon completado en 10.806841 segundos
Cannon completado en 8.009808 segundos

Tiempo medio: 10.740777

Tiempo más alto: 11.789626 segundos
Tiempo más bajo: 8.009808 segundos
Variación: 11.789626 - 8.009808 = 3.779818 segundos





Algoritmo de Cannon con 4 nodos (ACTUALIZADO)
Cannon completado en 2.203014 segundos

Cannon completado en 2.234247 segundos

Cannon completado en 1.881399 segundos

Cannon completado en 2.059283 segundos

Cannon completado en 2.193044 segundos

Tiempo medio ≈ 2.114000 segundos

Tiempo más alto: 2.234247 segundos
Tiempo más bajo: 1.881399 segundos
Variación: 2.234247 - 1.881399 = 0.352848 segundos



Algoritmo de Cannon con 4 nodos y 2 procesos: (ACTUALIZADO)

Cannon completado en 2.192979 segundos

Cannon completado en 2.094047 segundos

Cannon completado en 1.992857 segundos

Cannon completado en 1.992775 segundos

Cannon completado en 2.191123 segundos

Tiempo medio ≈ 2.092756 segundos.


Tiempo más alto: 2.192979 segundos
Tiempo más bajo: 1.992775 segundos
Variación: 2.192979 - 1.992775 = 0.200204 segundos




Algoritmo de Cannon con 16 nodos (ACTUALIZADO)

Cannon completado en 1.177641 segundos

Cannon completado en 1.000372 segundos

Cannon completado en 0.945468 segundos

Cannon completado en 1.109145 segundos

Cannon completado en 1.046103 segundos

Cannon completado en 1.180969 segundos



Tiempo medio ≈ 1.076616 segundos.

Tiempo más alto: 1.180969 segundos
Tiempo más bajo: 0.945468 segundos
Variación: 1.180969 - 0.945468 = 0.235501 segundos





___________________________________________________________________________________________________________________________________________________________________________________________________________


 
Block row-wise con 1 nodo (ACTUALIZADO)

Matrix multiplication 1024x1024 completed in 6.701251 seconds across 1 process(es).

Matrix multiplication 1024x1024 completed in 7.176517 seconds across 1 process(es).

Matrix multiplication 1024x1024 completed in 7.269252 seconds across 1 process(es).

Matrix multiplication 1024x1024 completed in 7.391804 seconds across 1 process(es).

Matrix multiplication 1024x1024 completed in 8.581758 seconds across 1 process(es).

Tiempo medio ≈ 7.424116 segundos.

Tiempo más alto: 8.581758 segundos
Tiempo más bajo: 6.701251 segundos
Variación: 8.581758 - 6.701251 = 1.880507 segundos







Block row-wise con 4 nodos:(ACTUALIZADO)

Matrix multiplication 1024x1024 completed in 1.594632 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 1.612389 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 1.675503 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 1.453765 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 1.507477 seconds across 4 process(es).


Tiempo medio ≈ 1.568753 segundos.

Tiempo más alto: 1.675503 segundos
Tiempo más bajo: 1.453765 segundos
Variación: 1.675503 - 1.453765 = 0.221738 segundos







Block row-wise con 4 nodos y 2 procesos (ACTUALIZADO)


Matrix multiplication 1024x1024 completed in 1.480423 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 2.000700 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 1.650093 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 1.713068 seconds across 4 process(es).

Matrix multiplication 1024x1024 completed in 1.570283 seconds across 4 process(es).

Tiempo medio ≈ 1.682913 segundos.

Tiempo más alto: 2.000700 segundos
Tiempo más bajo: 1.480423 segundos
Variación: 2.000700 - 1.480423 = 0.520277 segundos



Block row-wise con 8 procesos: (ACTUALIZADO)

Matrix multiplication 1024x1024 completed in 0.890586 seconds across 8 process(es).

Matrix multiplication 1024x1024 completed in 1.073898 seconds across 8 process(es).

Matrix multiplication 1024x1024 completed in 1.083501 seconds across 8 process(es).

Matrix multiplication 1024x1024 completed in 1.074283 seconds across 8 process(es).

Matrix multiplication 1024x1024 completed in 0.899564 seconds across 8 process(es).

Tiempo medio ≈ 1.004366 segundos.

Tiempo más alto: 1.083501 segundos
Tiempo más bajo: 0.890586 segundos
Variación: 1.083501 - 0.890586 = 0.192915 segundos




Block row-wise con 16 procesos (ACTUALIZADO)


Matrix multiplication 1024x1024 completed in 0.705126 seconds across 16 process(es).

Matrix multiplication 1024x1024 completed in 0.697776 seconds across 16 process(es).

Matrix multiplication 1024x1024 completed in 0.692191 seconds across 16 process(es).

Matrix multiplication 1024x1024 completed in 0.682067 seconds across 16 process(es).

Matrix multiplication 1024x1024 completed in 0.683470 seconds across 16 process(es).


Tiempo medio ≈ 0.692126 segundos.


Tiempo más alto: 0.705126 segundos
Tiempo más bajo: 0.682067 segundos
Variación: 0.705126 - 0.682067 = 0.023059 segundos





¡| Nodos / Procesos    | Fox       | Cannon    | Block row-wise |
| ------------------- | --------- | --------- | -------------- |
| 1 nodo              | 11.604138 | 10.740777 | 7.424116       |
| 4 nodos             | 1.926094  | 2.114000  | 1.568753       |
| 4 nodos, 2 procesos | 2.042579  | 2.092756  | 1.682913       |
| 8 procesos          | -         | -         | 1.004366       |
| 16 nodos / procesos | 1.189070  | 1.076616  | 0.692126       |











