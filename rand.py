#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
#
# Distributed under terms of the MIT license.

"""

"""
import random

def zoneRand():
  return random.randint(0, 1)

def randFloat():
  return random.uniform(0, 1)

for x in range(10000):
  print str(zoneRand()) + "\t" + str(randFloat())
