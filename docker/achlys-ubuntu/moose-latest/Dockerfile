
# To make use of multiple cores during the compile stages of the docker build
# docker build -t achlys-ubuntu --build-arg compile_cores=8 .

# To compile a particular git sha use
# docker build -t achlys-ubuntu --build-arg build_git_sha=${GITHUB_SHA} .

#FROM helenbrooks/moose-ubuntu
FROM idaholab/moose:latest
ARG compile_cores=""
ARG build_git_sha="master"

# Should we generate a coverage report
ARG test_coverage=false
ARG coveralls_token=""
ARG coveralls_branch=""

RUN apt-get update && \
  apt-get -y install pip && \
  pip3 install sympy  \
   pandas \ 
   matplotlib \
   pyyaml \ 
   mock

RUN if "$test_coverage" ; then \
       apt-get install -y python3-pip && \
       pip install cpp-coveralls && \
       pip install coveralls; \
    fi


WORKDIR /opt
RUN  git clone https://github.com/aurora-multiphysics/achlys.git
WORKDIR  /opt/achlys
RUN  git checkout "$build_git_sha"
#RUN  make -j"$compile_cores"

RUN if "$test_coverage" ; then \
       export CXXFLAGS="--coverage $CXXFLAGS"; \
       export CFLAGS="-fprofile-arcs -ftest-coverage"; \
       echo "Generating test coverage"; \
       make -j"$compile_cores" && \
      ./run_tests; \
    else \
       make -j"$compile_cores" &&\ 
       ./run_tests; \
    fi

# Generate test coverage report using coveralls and upload
WORKDIR  /opt/achlys
RUN if "$test_coverage" ; then \
       export COVERALLS_REPO_TOKEN="$coveralls_token" && \
       export CI_BRANCH="$coveralls_branch" && \
       cpp-coveralls -i /opt/achlys/src/ -i /opt/achlys/include/ --exclude-pattern "/usr/*" --dump cpp_cov.json && \
       coveralls --merge=cpp_cov.json; \
       cat cpp_cov.json; \
    fi
