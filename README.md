# HashStatic
implementacion de hash-based index: hash static

# ideas iniciales
## archivos
- datafile: contiene los records, free list tipo LIFO
- buckectsfile: contiene las paginas con pares \<key, position> tipo  freelist LIFO

## clase:
- record: record
- pares: \<key, position relativa>   guarda la posicion relativa del record en el datafile.
- bucket: contiene N pares, count y un next. ademas de funciones de inserciones y busqueda.

# PASOS

- creacion del buckectfile: con las M buckets iniciales vacias. 

# consideraciones
en los pares del bucketfile los pares manejaran posicion relativa de los records.
pero para los nexts manejare posicion absolutas en el bucketfile.

