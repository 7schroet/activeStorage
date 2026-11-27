FROM registry.fedoraproject.org/fedora:45

RUN dnf -y --setopt=tsflags=nodocs install \
      automake \
      cmake \
      clang \
      clang-tools-extra \
      cppcheck \
      file \
      gcc \
      git \
      lcov \
      libasan \
      libubsan \
      llvm \
      ninja \
      patch \
      python3-devel \
      zlib-devel \
    && dnf clean all

ARG SPACK_TAG=v1.2.2

RUN git clone --depth 1 --revision=${SPACK_TAG} https://github.com/spack/spack.git \
    && . spack/share/spack/setup-env.sh \
    && spack compiler find \
    && spack external find \
    && spack install googletest@1.17.0 \
         hdf5@1.14.6+cxx+threadsafe ^openmpi ^libevent~openssl\
         mochi-thallium@0.15.0 \
    && spack gc -y && rm -rf /tmp/* spack/.git

RUN find -L spack/opt/spack/* -type f -exec readlink -f '{}' \; \
    | xargs file -i \
    | grep 'charset=binary' \
    | grep 'x-executable\|x-archive\|x-sharedlib' \
    | awk -F: '{print $1}' | xargs strip
