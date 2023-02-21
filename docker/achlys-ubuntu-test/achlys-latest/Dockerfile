
# To make use of multiple cores during the compile stages of the docker build
# docker build -t achlys-ubuntu --build-arg compile_cores=8 .

# To compile a particular git sha use
# docker build -t achlys-ubuntu --build-arg build_git_sha=${GITHUB_SHA} .

FROM sdixonukaea/achlys-ubuntu:latest
ARG compile_cores=1
ARG build_git_sha="master"

# Should we generate a coverage report
ARG test_coverage=false
ARG coveralls_token=""
ARG coveralls_branch=""

RUN if "$test_coverage" ; then \
       apt-get install -y python3-pip && \
       pip install cpp-coveralls && \
       pip install coveralls; \
    fi

WORKDIR /opt/achlys

RUN if "$test_coverage" ; then \
       export CXXFLAGS="--coverage $CXXFLAGS"; \
       export CFLAGS="-fprofile-arcs -ftest-coverage"; \
       echo "Generating test coverage"; \
       make clobber
    fi; \
    git pull && \
    git checkout "$build_git_sha" && \
    make -j"$compile_cores" && \
    ./run_tests

# Generate test coverage report using coveralls and upload
RUN if "$test_coverage" ; then \
       export COVERALLS_REPO_TOKEN="$coveralls_token" && \
       export CI_BRANCH="$coveralls_branch" && \
       cpp-coveralls -i src/ -i include/ --exclude-pattern "/usr/*" --dump cpp_cov.json && \
       coveralls --merge=cpp_cov.json; \
    fi
