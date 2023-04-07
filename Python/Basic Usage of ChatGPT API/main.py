#!/usr/bin/env python
# -*- coding: utf-8 -*-
'''
    python_v: python3.9.1
    author: 梦想是优秀社畜 from CSDN
    date: 2023/03/07
'''

import os
import openai

openai.api_key = "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"
# os.environ["http_proxy"]="http://172.29.1.26:4780"
# os.environ["https_proxy"]="https://172.29.1.26:4780"
response = openai.Completion.create(
  model="text-davinci-001",
  prompt="Write a tagline for an ice cream shop.\n",
  temperature=0.4,
  max_tokens=64,
  top_p=1,
  frequency_penalty=0,
  presence_penalty=0
)


