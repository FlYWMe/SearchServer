FROM nvidia/cuda:8.0-cudnn6-devel-ubuntu16.04

RUN apt-get update && \
    apt-get install -y cmake git software-properties-common && \
    mkdir -p /opt/searcher && \
    cd /opt/searcher && mkdir source && cd source && \
    git clone http://USER:PASSWORD@gitlab.oceanai.com.cn/verifier/searcher.git && \
    cd searcher && \
    apt-get install -y libopenblas-dev libboost-dev && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j"$(nproc)"  && \
    cd /opt/searcher && \
    cp /opt/searcher/source/searcher/build/bin/queryServer . && \
    apt-get -y remove cmake git software-properties-common && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    cd /opt/searcher && rm -rf ./source

EXPOSE 2333

CMD cd /opt/searcher && ./queryServer 
