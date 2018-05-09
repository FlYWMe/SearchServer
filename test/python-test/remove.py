#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import requests
import base64
import json
import time
import random
import numpy as np


def main():
    ip = '127.0.0.1'
    port = '2333'
    api = '/delete'
    length = len(sys.argv)
    if length == 2:
        ip = sys.argv[1]
    elif length == 3:
        ip = sys.argv[1]
        port = sys.argv[2]
    elif length > 3:
        print 'Usage: python remove.py [ip] [port]'
        return
    ntotal = 100
    ids = list(range(0, ntotal + 1))
    address = 'http://' + ip + ':' + port
    url = address + api
    headers = {'Content-Type': 'application/json'}

    data = {'ids': ids}

    start = time.clock()
    r = requests.post(url, data=json.dumps(data))
    elapsed = (time.clock() - start)
    result = r.json()
    print result
    print 'Time used:', elapsed * 1000
    return


if __name__ == '__main__':
    main()
