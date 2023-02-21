
# To make use of multiple cores during the compile stages of the docker build
# docker build -t achlys-ubuntu --build-arg compile_cores=8 .

# To compile a particular git sha use
# docker build -t achlys-ubuntu --build-arg build_git_sha=${GITHUB_SHA} .

FROM idaholab/moose:latest
ARG compile_cores=1
ARG build_git_sha="master"


RUN apt-get update && \
  apt-get -y install pip && \
  pip3 install sympy  \
   pandas \ 
   matplotlib \
   pyyaml \ 
   mock


WORKDIR /opt
RUN  git clone https://github.com/aurora-multiphysics/achlys.git
WORKDIR  /opt/achlys
RUN  git checkout "$build_git_sha"
RUN  make -j"$compile_cores"
RUN  ./run_tests

