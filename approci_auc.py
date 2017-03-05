#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2017 dl <dl@mpi36.news.usw2.contents.cmcm.com>
#
# Distributed under terms of the MIT license.

"""

"""

import sys

class ClickInfo(object):
  def __init__(self):
    self.click = 0
    self.non_click = 0
  def to_string(self):
    return "click[%d],nonclick[%d]" % (self.click, self.non_click)

clickinfos = [ClickInfo() for x in range(1000)]

for lines in sys.stdin:
  label, score = lines.strip().split()
  score = float(score)
  if label == "1":
    clickinfos[int(score * 1000)].click += 1
  else:
    clickinfos[int(score * 1000)].non_click += 1

non_click = 0
click = 1
auc_sum = 0.0

print ' '.join(map(lambda x: x.t))

for clickinfo in clickinfos:
  auc_sum += clickinfo.click * non_click + 0.5 * clickinfo.non_click * clickinfo.click
  non_click += clickinfo.non_click
  click += clickinfo.click

print "auc=", auc_sum / non_click / click
