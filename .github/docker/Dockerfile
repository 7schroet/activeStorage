FROM registry.fedoraproject.org/fedora:42

RUN dnf -y --setopt=tsflags=nodocs install \
      automake          \
      cmake             \
      clang             \
      clang-tools-extra \
      cppcheck          \
      gcc               \
      git               \
      lcov              \
      libasan           \
      libubsan          \
      patch             \
      zlib-devel        \
    && dnf clean all

ARG SPACK_COMMIT=0b06bb793e577852ddb5ff0cb55bb2c5aca0a9f3
ARG SPACK_PKG_COMMIT=b9e8cc6d79ff67c59fcf64dca65d7e7358645b8b

RUN git clone --depth 1 --revision=${SPACK_COMMIT} https://github.com/spack/spack.git && \
    . spack/share/spack/setup-env.sh                                                  && \
    mkdir ~/.spack                                                                    && \
    repo_config="repos:\n  builtin:\n    commit: ${SPACK_PKG_COMMIT}\n"               && \
    printf "$repo_config" > ~/.spack/repos.yaml                                       && \
    spack compiler find                                                               && \
    spack external find                                                               && \
    spack install googletest@1.17.0 hdf5@1.14.6+cxx+threadsafe mochi-thallium@0.15.0  && \
    spack gc -y && rm -rf /tmp/* spack/.git
