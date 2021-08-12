
# To make use of multiple cores during the compile stages of the docker build
# docker build -t achlys-ubuntu --build-arg compile_cores=8 .

# To compile a particular git sha use
# docker build -t achlys-ubuntu --build-arg build_git_sha=${GITHUB_SHA} .

FROM helenbrooks/moose-ubuntu
ARG compile_cores=1
ARG build_git_sha="master"

RUN apt-get update && \
  apt-get -y install pip && \
  pip3 install sympy && \
  pip3 install pandas

RUN cd /home && \
    git clone https://github.com/aurora-multiphysics/achlys.git && \
    cd achlys && \
    git checkout "$build_git_sha" && \
    make -j"$compile_cores" && \
    ./run_tests
