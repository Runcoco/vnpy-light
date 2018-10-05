# encoding: UTF-8

"""
包含一些开发中常用的函数
"""

import os
import json
import math
from datetime import datetime


#----------------------------------------------------------------------
def safeUnicode(value):
    """检查接口数据潜在的错误，保证转化为的字符串正确"""
    # 检查是数字接近0时会出现的浮点数上限
    if isinstance(value, (int, float)):
        if math.isnan(value) or value > MAX_NUMBER:
            value = 0

    # 检查防止小数点位过多
    if isinstance(value, float):
        value = round(value, ndigits=MAX_DECIMAL)

    return str(value)


#----------------------------------------------------------------------
def todayDate():
    """获取当前本机电脑时间的日期"""
    return datetime.now().replace(hour=0, minute=0, second=0, microsecond=0)


# 图标路径
iconPathDict = {}

path = os.path.abspath(os.path.dirname(__file__))   # 遍历vnpy安装目录
for root, subdirs, files in os.walk(path):
    for fileName in files:
        if '.ico' in fileName:
            iconPathDict[fileName] = os.path.join(root, fileName)

path = os.getcwd()      # 遍历工作目录
for root, subdirs, files in os.walk(path):
    for fileName in files:
        if '.ico' in fileName:
            iconPathDict[fileName] = os.path.join(root, fileName)

#----------------------------------------------------------------------
def loadIconPath(iconName):
    """加载程序图标路径"""
    global iconPathDict
    return iconPathDict.get(iconName, '')



#----------------------------------------------------------------------
def getTempPath(name):
    """获取存放临时文件的路径"""
    tempPath = os.path.join(os.getcwd(), 'temp')
    if not os.path.exists(tempPath):
        os.makedirs(tempPath)

    path = os.path.join(tempPath, name)
    return path


# JSON配置文件路径
jsonPathDict = {}

#----------------------------------------------------------------------
def getJsonPath(name, moduleFile):
    """
    获取JSON配置文件的路径：
    1. 优先从当前工作目录查找JSON文件
    2. 若无法找到则前往模块所在目录查找
    """
    currentFolder = os.getcwd()
    currentJsonPath = os.path.join(currentFolder, name)
    if os.path.isfile(currentJsonPath):
        jsonPathDict[name] = currentJsonPath
        return currentJsonPath

    moduleFolder = os.path.abspath(os.path.dirname(moduleFile))
    moduleJsonPath = os.path.join(moduleFolder, '.', name)
    jsonPathDict[name] = moduleJsonPath
    return moduleJsonPath


# 加载全局配置
#----------------------------------------------------------------------
def loadJsonSetting(settingFileName):
    """加载JSON配置"""
    settingFilePath = getJsonPath(settingFileName, __file__)
    setting = {}
    with open(settingFilePath, 'r', encoding='utf8') as f:
        setting = json.load(f)
    return setting


# 函数常量
MAX_NUMBER = 10000000000000

globalSetting = loadJsonSetting('VT_setting.json')
MAX_DECIMAL = globalSetting.get('maxDecimal', 4)
