#!/usr/bin/env python
# 2017 Bartholomaeus Dedersen
# Generates a stream of randomness from last 1000 random numbers.
# With a small statistical check.

import urllib2
import json
import matplotlib.pyplot as plt
import numpy as np

def main():

  numRandomness = '1000'

  url = "https://api.thingspeak.com/channels/305931/fields/4.json?results=" + numRandomness

  httpConnection = urllib2.urlopen(url)

  response = httpConnection.read()

  channel = json.loads(response)
  randomness = [element['field4'] for element in channel['feeds']]
  longRandomness = ''

  for randomValue in randomness:
    longRandomness = longRandomness + "{0:b}".format(int(randomValue))

  httpConnection.close()

  print longRandomness
  print 'Amount of 0s:' + str(longRandomness.count('0'))
  print 'Amount of 1s:' + str(longRandomness.count('1'))

if __name__ == '__main__':
    main()
