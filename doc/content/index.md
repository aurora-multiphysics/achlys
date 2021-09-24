
# Achlys

Achlys aims to model macroscopic tritium transport processes through fusion materials using the 
[MOOSE Finite Element Framework](http://mooseframework.org).

Benchmarking of Achlys is currently being carried out against published computational and experimental results.

## Documentation

Module documentation for Achlys can be found [here](module/index.md).
This provides:

- [Introduction and theory](module/introduction.md)
- [examples](module/examples/index.md)
- [source documentation](source/index.md)

## License

[Achlys](https://github.com/aurora-multiphysics/achlys) is licensed under LGPL v2.1, see [here](https://github.com/aurora-multiphysics/achlys/blob/maaster/LICENSE).

## Installation

If using an existing moose installation and conda environment no additional dependencies are required and the project can simply be pulled and built.

```
git clone https://github.com/aurora-multiphysics/achlys.git
cd achlys
make -j4
```

Alternatively Docker can be used. Note, however, that the base moose-ubuntu image will be about 12GB in size.

```
git clone https://github.com/aurora-multiphysics/achlys.git
cd achlys
docker build -t achlys-ubuntu --build-arg compile_cores=4 --build-arg build_git_sha=master docker/achlys-ubuntu/
docker run -it achlys-ubuntu /bin/bash
```

## Quick Start

```
./achlys-opt -i problems/thermal_desorption/ogorodnikova/tds_multiapp/desorp_multi.i
```
