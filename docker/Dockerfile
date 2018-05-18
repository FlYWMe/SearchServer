FROM nvidia/cuda:8.0-cudnn6-devel-ubuntu16.04

RUN apt-get update && \
    apt-get install -y cmake git software-properties-common && \
    mkdir -p /opt/searcher && \
    cd /opt/searcher && mkdir source && cd source && \
    git clone http://FlYWMe:fly950418111@github.com/FlYWMe/SearchServer.git && \
    cd SearchServer && \
    bash scripts/git-lfs.deb.sh && \
    apt-get update && \
    apt-get install -y git-lfs && \
    git lfs install --skip-smudge && \
    git lfs pull && \
    bash scripts/install_deps.sh && \    
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j"$(nproc)"  && \
    cd /opt/searcher && \
    cp /opt/searcher/source/SearchServer/build/bin/searcher . && \
    apt-get -y remove cmake git git-lfs software-properties-common && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    cd /opt/searcher && rm -rf ./source

EXPOSE 2333

CMD cd /opt/searcher && ./bin/searcher 